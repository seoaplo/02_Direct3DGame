#include "header.h"
#include "SSSWriter.h"
#include <iostream>
#include <fstream>

//===========================================================================
// Max File Read
//===========================================================================
void  SSSWriter::Set(const TCHAR* name, Interface* pMax)
{
	m_pMax = pMax;
	m_filename = name;
	m_pRootNode = m_pMax->GetRootNode();		// 최상단 노드를 저장해둔다.
	PreProcess(m_pRootNode);					// 재귀함수
}
void  SSSWriter::PreProcess(INode* pNode)
{
	// 끝에 도달하면 재귀 함수 정지
	if (pNode == NULL) return;			

	// 노드 저장
	AddObject(pNode);										

	// 자식 노드의 수를 얻은 다음 반복한다.
	int iNumChildren = pNode->NumberOfChildren();			 
	for (int iChild = 0; iChild < iNumChildren; iChild++)
	{
		INode* pChild = pNode->GetChildNode(iChild);
		PreProcess(pChild);
	}
}
void SSSWriter::AddObject(INode* pNode)
{
	ObjectState os = pNode->EvalWorldState(0);
	if (os.obj)
	{
		// 불필요한 노드(시야의 방향 등에 쓰이는 노드)
		if (os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))
		{
			return;
		}

		switch (os.obj->SuperClassID())
		{
		// 해당 노드이면 저장
		case GEOMOBJECT_CLASS_ID:
			//case HELPER_CLASS_ID:
			m_ObjectList.push_back(pNode);
		default:
			break;
		}
	}
}
//===========================================================================
// Exprot File Write
//===========================================================================
bool  SSSWriter::Export()
{
	FILE* pStream = nullptr;
	_wfopen_s(&pStream, m_filename.c_str(), _T("wb"));
	_ftprintf_s(pStream, _T("%s %d"), _T("SSSExporter100"),
		m_ObjectList.size());

	std::vector<PNCT>	VertexBufferArray;
	std::vector<int>	IndexBufferArray;

	for (int iObj = 0; iObj < m_ObjectList.size(); iObj++)
	{
		VertexBufferArray.resize(0);
		IndexBufferArray.resize(0);

		INode* pNode = m_ObjectList[iObj];
		GetMesh(pNode);
		GetMaterial(pNode);

		_ftprintf(pStream, _T("\n%s %d %d"),
			pNode->GetName(),
			m_triList.size(),
			m_texList.size());
		for (int iSubMap = 0; iSubMap < m_texList.size(); iSubMap++)
		{
			_ftprintf(pStream, _T("\n%d %s"),
				m_texList[iSubMap].iMapID,
				m_texList[iSubMap].szName);
		}

		for (int iTri = 0; iTri < m_triList.size(); iTri++)
		{
			for (int iVer = 0; iVer < 3; iVer++)
			{
				int iCheck;
				int iVertexSize = iTri * 3 + iVer;
				for (iCheck = 0; iCheck < VertexBufferArray.size(); iCheck++)
				{
					int iCheckTirVertex = (int)(iCheck / 3);
					int iCheckVertex = (iCheck % 3);

					if (EqualPoint3(m_triList[iTri].v[iVer].p, VertexBufferArray[iCheck].p) &&
						EqualPoint3(m_triList[iTri].v[iVer].n, VertexBufferArray[iCheck].n) &&
						EqualPoint4(m_triList[iTri].v[iVer].c, VertexBufferArray[iCheck].c) &&
						EqualPoint2(m_triList[iTri].v[iVer].t, VertexBufferArray[iCheck].t))
					{
						break;
					}
				}
				
				if (iCheck == VertexBufferArray.size())
				{
					VertexBufferArray.push_back(m_triList[iTri].v[iVer]);
					IndexBufferArray.push_back(VertexBufferArray.size() - 1);
				}
				else
				{
					IndexBufferArray.push_back(iCheck);
				}
			}
		}

		_ftprintf(pStream, _T("\n Vertex %d"), VertexBufferArray.size());
		for (int iVertex = 0; iVertex < VertexBufferArray.size(); iVertex++)
		{
			_ftprintf(pStream, _T("\n%10.4f %10.4f %10.4f"),
				VertexBufferArray[iVertex].p.x,
				VertexBufferArray[iVertex].p.y,
				VertexBufferArray[iVertex].p.z);
			_ftprintf(pStream, _T("%10.4f %10.4f %10.4f"),
				VertexBufferArray[iVertex].n.x,
				VertexBufferArray[iVertex].n.y,
				VertexBufferArray[iVertex].n.z);
			_ftprintf(pStream, _T("%10.4f %10.4f %10.4f %10.4f"),
				VertexBufferArray[iVertex].c.x,
				VertexBufferArray[iVertex].c.y,
				VertexBufferArray[iVertex].c.z,
				VertexBufferArray[iVertex].c.w);
			_ftprintf(pStream, _T("%10.4f %10.4f"),
				VertexBufferArray[iVertex].t.x,
				VertexBufferArray[iVertex].t.y);
		}
		_ftprintf(pStream, _T("\n index %d\n"), IndexBufferArray.size());
		for (int iIndex = 0; iIndex < IndexBufferArray.size(); iIndex++)
		{
			_ftprintf(pStream, _T("%d\t"),
				IndexBufferArray[iIndex]);
		}
	}
	fclose(pStream);

	MessageBox(GetActiveWindow(), m_filename.c_str(),
		_T("Succeed!"), MB_OK);
	return true;
}

TriObject* SSSWriter::GetTriObjectFromNode(INode* pNode, TimeValue time, bool& deleteit)
{
	// 오브젝트를 받는다.
	Object* obj = pNode->EvalWorldState(time).obj;
	// 변환 가능하면 실행
	if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
	{
		TriObject* tri = (TriObject*)obj->ConvertToType(time, Class_ID(TRIOBJ_CLASS_ID, 0));
		if (obj != tri) deleteit = true;
		return tri;
	}
	return nullptr;
}

void SSSWriter::GetMesh(INode* pNode)
{
	// 로컬 좌표계이면 월드 행렬, 아니면 단위 행렬
	Matrix3 tm = pNode->GetObjTMAfterWSM(0);
	
	bool deleteit = false;
	// 트라이앵글 오브젝트
	TriObject* tri = GetTriObjectFromNode(pNode, 0, deleteit);
	if (tri == nullptr) return;
	// 메쉬 오브젝트
	Mesh* mesh = &tri->GetMesh();
	bool negScale = TMNegParity(tm);

	int v0, v1, v2;
	if (negScale) { v0 = 2; v1 = 1; v2 = 0; }
	else { v0 = 0; v1 = 1; v2 = 2; }

	if (mesh)
	{
		int iNumFace = mesh->getNumFaces();
		m_triList.resize(iNumFace);

		for (int iFace = 0; iFace < iNumFace; iFace++)
		{
			// position
			int iNumPos = mesh->getNumVerts();
			if (iNumPos > 0)
			{
				Point3 v = mesh->verts[mesh->faces[iFace].v[v0]] * tm;
				DumpPoint3(m_triList[iFace].v[0].p, v);
				
				v = mesh->verts[mesh->faces[iFace].v[v2]] * tm;
				DumpPoint3(m_triList[iFace].v[1].p, v);

				v = mesh->verts[mesh->faces[iFace].v[v1]] * tm;
				DumpPoint3(m_triList[iFace].v[2].p, v);
			}
			// color
			int iNumColor = mesh->getNumVertCol();
			m_triList[iFace].v[0].c = Point4(1, 1, 1, 1);
			m_triList[iFace].v[1].c = Point4(1, 1, 1, 1);
			m_triList[iFace].v[2].c = Point4(1, 1, 1, 1);
			if (iNumColor > 0)
			{
				m_triList[iFace].v[0].c =
					mesh->vertCol[mesh->vcFace[iFace].t[v0]];
				m_triList[iFace].v[1].c =
					mesh->vertCol[mesh->vcFace[iFace].t[v2]];
				m_triList[iFace].v[2].c =
					mesh->vertCol[mesh->vcFace[iFace].t[v1]];
			}
			// texcoord
			int iNumTex = mesh->getNumTVerts();
			if (iNumTex > 0)
			{
				Point2 v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[v0]];
				m_triList[iFace].v[0].t.x = v.x;
				m_triList[iFace].v[0].t.y = 1.0f - v.y;

				v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[v2]];
				m_triList[iFace].v[1].t.x = v.x;
				m_triList[iFace].v[1].t.y = 1.0f - v.y;
				v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[v1]];
				m_triList[iFace].v[2].t.x = v.x;
				m_triList[iFace].v[2].t.y = 1.0f - v.y;
			}

			mesh->buildNormals();
			int vert = mesh->faces[iFace].getVert(v0);
			RVertex* rVertex = mesh->getRVertPtr(vert);
			Point3 vn = GetVertexNormal(mesh, iFace, rVertex);
			DumpPoint3(m_triList[iFace].v[v0].n, vn);

			vert = mesh->faces[iFace].getVert(v1);
			rVertex = mesh->getRVertPtr(vert);
			vn = GetVertexNormal(mesh, iFace, rVertex);
			DumpPoint3(m_triList[iFace].v[v1].n, vn);

			vert = mesh->faces[iFace].getVert(v2);
			rVertex = mesh->getRVertPtr(vert);
			vn = GetVertexNormal(mesh, iFace, rVertex);
			DumpPoint3(m_triList[iFace].v[v2].n, vn);
		}
		// vb, ib		
	}

	if (deleteit) delete tri;
}

Point3 SSSWriter::GetVertexNormal(
	Mesh* mesh, int iFace, RVertex* rVertex)
{
	Face* f = &mesh->faces[iFace];
	DWORD smGroup = f->smGroup;
	int numNormals = rVertex->rFlags & NORCT_MASK;

	Point3 vertexNormal;
	if (rVertex->rFlags & SPECIFIED_NORMAL)
	{
		vertexNormal = rVertex->rn.getNormal();
	}
	else if (numNormals && smGroup)
	{
		if (numNormals == 1)
		{
			vertexNormal = rVertex->rn.getNormal();
		}
		else
		{
			for (int i = 0; i < numNormals; i++)
			{
				if (rVertex->ern[i].getSmGroup() & smGroup)
				{
					vertexNormal = rVertex->ern[i].getNormal();
				}
			}
		}
	}
	else
	{
		vertexNormal = mesh->getFaceNormal(iFace);
	}
	return vertexNormal;
}

void  SSSWriter::GetMaterial(INode* pNode)
{
	Mtl* pSrcMtl = pNode->GetMtl();
	GetTexture(pSrcMtl);
}
void  SSSWriter::GetTexture(Mtl* pMtl)
{
	int iNumMap = pMtl->NumSubTexmaps();
	for (int iSubMap = 0; iSubMap < iNumMap; iSubMap++)
	{
		Texmap* tex = pMtl->GetSubTexmap(iSubMap);
		if (tex)
		{
			if (tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0X00))
			{
				SMtrl tMtl;
				tMtl.iMapID = iSubMap;

				TSTR fullName;
				TSTR mapName = ((BitmapTex*)(tex))->GetMapName();
				SplitPathFile(mapName, &fullName, &tMtl.szName);
				m_texList.push_back(tMtl);
			}
		}
	}
}
SSSWriter::SSSWriter()
{
}


SSSWriter::~SSSWriter()
{
}
