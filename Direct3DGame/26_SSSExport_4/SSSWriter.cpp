#include "header.h"
#include "SSSWriter.h"
#include <iostream>
#include <fstream>

struct AscendingSort
{
	bool operator()(TriList& rpStart, TriList& rpEnd)
	{
		return rpStart.iSubIndex < rpEnd.iSubIndex;
	}
};
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
	AddMaterial(pNode);
	// 자식 노드의 수를 얻은 다음 반복한다.
	int iNumChildren = pNode->NumberOfChildren();			 
	for (int iChild = 0; iChild < iNumChildren; iChild++)
	{
		INode* pChild = pNode->GetChildNode(iChild);
		PreProcess(pChild);
	}
}
void SSSWriter::AddMaterial(INode* pNode)
{
	Mtl* pMtl = pNode->GetMtl();
	if (pMtl)
	{
		for (int iMtl = 0; iMtl < m_MaterialList.size(); iMtl++)
		{
			if (m_MaterialList[iMtl] == pMtl)
			{
				return;
			}
		}
		m_MaterialList.push_back(pMtl);
		GetMaterial(pNode);
	}
}
int	SSSWriter::FindMaterial(INode* pNode)
{
	Mtl* pMtl = pNode->GetMtl();
	if (pMtl)
	{
		for (int iMtl = 0; iMtl < m_MaterialList.size(); iMtl++)
		{
			if (m_MaterialList[iMtl] == pMtl)
			{
				return iMtl;
			}
		}
	}
	return -1;
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

bool SSSWriter::Convert()
{
	for (int iObj = 0; iObj < m_ObjectList.size(); iObj++)
	{
		INode* pNode = m_ObjectList[iObj];
		SMesh sMesh;
		sMesh.name = FixupName(pNode->GetName());
		INode* pParentNode = pNode->GetParentNode();
		if (pParentNode &&
			// 가상의 부모가 존재
			pParentNode->IsRootNode() == false)
		{
			sMesh.ParentName =
				FixupName(pParentNode->GetName());
		}
		// objTM = s*r*t*p * c;
		// GetNodeTM = srt * p;
		Matrix3 wtm = pNode->GetNodeTM(0);
		DumpMatrix3(sMesh.matWorld, &wtm);

		sMesh.iMtrlID = FindMaterial(pNode);
		if (sMesh.iMtrlID >= 0)
		{
			if (m_MtrlList[sMesh.iMtrlID].subMtrl.size() > 0)
			{
				sMesh.iSubMesh = m_MtrlList[sMesh.iMtrlID].subMtrl.size();
			}
		}
		else
		{
			sMesh.iSubMesh = 1;
		}
		GetMesh(pNode, sMesh);

		m_sMeshList.push_back(sMesh);
	}
	return true;
}
//===========================================================================
// Exprot File Write
//===========================================================================
bool  SSSWriter::Export()
{

	Convert();

	FILE* pStream = nullptr;
	_wfopen_s(&pStream, m_filename.c_str(), _T("wb"));
	_ftprintf_s(pStream, _T("%s"), _T("SSSExporter100"));
	_ftprintf_s(pStream, _T("\n%s"), _T("#HEADER_INFO"));
	_ftprintf_s(pStream, _T(" %d %d"), m_sMeshList.size(), m_MtrlList.size());

	// root material
	_ftprintf(pStream, _T("\n%s"), L"#MATERIAL_INFO");
	for (int iMtl = 0; iMtl < m_MtrlList.size(); iMtl++)
	{
		_ftprintf(pStream, _T("\n%s %d"),
			m_MtrlList[iMtl].szName,
			m_MtrlList[iMtl].subMtrl.size());

		if (m_MtrlList[iMtl].subMtrl.size() > 0)
		{
			for (int iSubMtrl = 0; iSubMtrl < m_MtrlList[iMtl].subMtrl.size(); iSubMtrl++)
			{
				_ftprintf(pStream, _T("\n%s %d"),
					m_MtrlList[iMtl].subMtrl[iSubMtrl].szName,
					m_MtrlList[iMtl].subMtrl[iSubMtrl].texList.size()
				);

				for (int iTex = 0; iTex < m_MtrlList[iMtl].subMtrl[iSubMtrl].texList.size(); iTex++)
				{
					_ftprintf(pStream, _T("\n%d %s"),
						m_MtrlList[iMtl].subMtrl[iSubMtrl].texList[iTex].iMapID,
						m_MtrlList[iMtl].subMtrl[iSubMtrl].texList[iTex].szName);
				}
			}
		}
		else
		{
			_ftprintf(pStream, _T("\n%s %d"),
				L"none",
				m_MtrlList[iMtl].texList.size()
			);

			for (int iTex = 0; iTex < m_MtrlList[iMtl].texList.size(); iTex++)
			{
				_ftprintf(pStream, _T("\n%d %s"),
					m_MtrlList[iMtl].texList[iTex].iMapID,
					m_MtrlList[iMtl].texList[iTex].szName);
			}
		}
	}


	// mesh list
	_ftprintf(pStream, _T("\n%s"), L"#OBJECT_INFO");
	for (int iObj = 0; iObj < m_sMeshList.size(); iObj++)
	{
		_ftprintf(pStream, _T("\n%s %s %d %d %d"),
			m_sMeshList[iObj].name,
			m_sMeshList[iObj].ParentName,
			m_sMeshList[iObj].iMtrlID,
			m_sMeshList[iObj].bufferList.size(),
			m_sMeshList[iObj].triList.size());

		// World Matrix
		_ftprintf(pStream, _T("\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f"),
			m_sMeshList[iObj].matWorld._11,
			m_sMeshList[iObj].matWorld._12,
			m_sMeshList[iObj].matWorld._13,
			m_sMeshList[iObj].matWorld._14,

			m_sMeshList[iObj].matWorld._21,
			m_sMeshList[iObj].matWorld._22,
			m_sMeshList[iObj].matWorld._23,
			m_sMeshList[iObj].matWorld._24,

			m_sMeshList[iObj].matWorld._31,
			m_sMeshList[iObj].matWorld._32,
			m_sMeshList[iObj].matWorld._33,
			m_sMeshList[iObj].matWorld._34,

			m_sMeshList[iObj].matWorld._41,
			m_sMeshList[iObj].matWorld._42,
			m_sMeshList[iObj].matWorld._43,
			m_sMeshList[iObj].matWorld._44);

		for (int iSubTri = 0; iSubTri < m_sMeshList[iObj].bufferList.size(); iSubTri++)
		{
			vertexList& vList =
				m_sMeshList[iObj].vbList[iSubTri];
			_ftprintf(pStream, _T("\nVertexList %d"),
				vList.size());
			for (int iVer = 0; iVer < vList.size(); iVer++)
			{
				_ftprintf(pStream, _T("\n%10.4f %10.4f %10.4f"),
					vList[iVer].p.x,
					vList[iVer].p.y,
					vList[iVer].p.z);
				_ftprintf(pStream, _T("%10.4f %10.4f %10.4f"),
					vList[iVer].n.x,
					vList[iVer].n.y,
					vList[iVer].n.z);
				_ftprintf(pStream, _T("%10.4f %10.4f %10.4f %10.4f"),
					vList[iVer].c.x,
					vList[iVer].c.y,
					vList[iVer].c.z,
					vList[iVer].c.w);
				_ftprintf(pStream, _T("%10.4f %10.4f"),
					vList[iVer].t.x,
					vList[iVer].t.y);
			}

			IndexList& iList =
				m_sMeshList[iObj].ibList[iSubTri];
			_ftprintf(pStream, _T("\nIndexList %d"), iList.size());
			for (int iIndex = 0; iIndex < iList.size(); iIndex += 3)
			{
				_ftprintf(pStream, _T("\n%d %d %d"),
					iList[iIndex + 0],
					iList[iIndex + 1],
					iList[iIndex + 2]);
			}
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

void SSSWriter::GetMesh(INode* pNode, SMesh& sMesh)
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
		sMesh.bufferList.resize(sMesh.iSubMesh);

		vectorTriList& triList = sMesh.triList;
		triList.resize(iNumFace);

		for (int iFace = 0; iFace < iNumFace; iFace++)
		{
			// position
			int iNumPos = mesh->getNumVerts();
			if (iNumPos > 0)
			{
				Point3 v = mesh->verts[mesh->faces[iFace].v[v0]] * tm;
				DumpPoint3(triList[iFace].v[0].p, v);
				
				v = mesh->verts[mesh->faces[iFace].v[v2]] * tm;
				DumpPoint3(triList[iFace].v[1].p, v);

				v = mesh->verts[mesh->faces[iFace].v[v1]] * tm;
				DumpPoint3(triList[iFace].v[2].p, v);
			}
			// color
			int iNumColor = mesh->getNumVertCol();
			triList[iFace].v[0].c = Point4(1, 1, 1, 1);
			triList[iFace].v[1].c = Point4(1, 1, 1, 1);
			triList[iFace].v[2].c = Point4(1, 1, 1, 1);
			if (iNumColor > 0)
			{
				triList[iFace].v[0].c =
					mesh->vertCol[mesh->vcFace[iFace].t[v0]];
				triList[iFace].v[1].c =
					mesh->vertCol[mesh->vcFace[iFace].t[v2]];
				triList[iFace].v[2].c =
					mesh->vertCol[mesh->vcFace[iFace].t[v1]];
			}
			// texcoord
			int iNumTex = mesh->getNumTVerts();
			if (iNumTex > 0)
			{
				Point2 v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[v0]];
				triList[iFace].v[0].t.x = v.x;
				triList[iFace].v[0].t.y = 1.0f - v.y;

				v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[v2]];
				triList[iFace].v[1].t.x = v.x;
				triList[iFace].v[1].t.y = 1.0f - v.y;
				v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[v1]];
				triList[iFace].v[2].t.x = v.x;
				triList[iFace].v[2].t.y = 1.0f - v.y;
			}

			mesh->buildNormals();
			int vert = mesh->faces[iFace].getVert(v0);
			RVertex* rVertex = mesh->getRVertPtr(vert);
			Point3 vn = GetVertexNormal(mesh, iFace, rVertex);
			DumpPoint3(triList[iFace].v[v0].n, vn);

			vert = mesh->faces[iFace].getVert(v2);
			rVertex = mesh->getRVertPtr(vert);
			vn = GetVertexNormal(mesh, iFace, rVertex);
			DumpPoint3(triList[iFace].v[v1].n, vn);

			vert = mesh->faces[iFace].getVert(v1);
			rVertex = mesh->getRVertPtr(vert);
			vn = GetVertexNormal(mesh, iFace, rVertex);
			DumpPoint3(triList[iFace].v[v2].n, vn);

			// sub material index
			if (sMesh.iMtrlID > 0)
			{
				triList[iFace].iSubIndex =
					mesh->faces[iFace].getMatID();
				if (m_MtrlList[sMesh.iMtrlID].subMtrl.size() <= 0)
				{
					triList[iFace].iSubIndex = 0;
				}

				sMesh.bufferList[
					triList[iFace].iSubIndex].push_back(
						triList[iFace]);
			}
			else
			{
				triList[iFace].iSubIndex = 0;
				sMesh.bufferList[
					triList[iFace].iSubIndex].push_back(
						triList[iFace]);
			}
			
		}

		std::sort(triList.begin(), triList.end(), AscendingSort());
		// vb, ib
		SetUniqueBuffer(sMesh);
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


void SSSWriter::SetUniqueBuffer(SMesh& sMesh)
{
	sMesh.vbList.resize(sMesh.bufferList.size());
	sMesh.ibList.resize(sMesh.bufferList.size());
	for (int iSub = 0; iSub < sMesh.bufferList.size();
		iSub++)
	{
		for (int iFace = 0; iFace < sMesh.bufferList[iSub].size(); iFace++)
		{
			vectorTriList& triArray = sMesh.bufferList[iSub];
			TriList& tri = triArray[iFace];
			vertexList& vList = sMesh.vbList[iSub];
			IndexList& iList = sMesh.ibList[iSub];
			for (int iVer = 0; iVer < 3; iVer++)
			{
				int iPos = IsEqulVerteList(tri.v[iVer], vList);
				if (iPos < 0)
				{
					vList.push_back(tri.v[iVer]);
					iPos = vList.size() - 1;
				}
				iList.push_back(iPos);
			}
		}
	}
}


void  SSSWriter::GetTexture(Mtl* pMtl, SMtrl& sMtrl)
{
	int iNumMap = pMtl->NumSubTexmaps();
	for (int iSubMap = 0; iSubMap < iNumMap; iSubMap++)
	{
		Texmap* tex = pMtl->GetSubTexmap(iSubMap);
		if (tex)
		{
			if (tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0X00))
			{
				STexMap sTexmap;
				sTexmap.iMapID = iSubMap;

				TSTR fullName;
				TSTR mapName = ((BitmapTex*)(tex))->GetMapName();
				SplitPathFile(mapName, &fullName, &sTexmap.szName);
				sMtrl.texList.push_back(sTexmap);
			}
		}
	}
}

void  SSSWriter::GetMaterial(INode* pNode)
{
	Mtl* pRootMtl = pNode->GetMtl();
	SMtrl sMtrl;
	sMtrl.szName = FixupName(pRootMtl->GetName());	// material 이름에 오류가 날 문자를 없애거나 바꿔야 한다.
	// sub-material
	int iNumSub = pRootMtl->NumSubMtls();
	if (iNumSub > 0)
	{
		for (int iSub = 0; iSub < iNumSub; iSub++)
		{
			Mtl* pSubMtl = pRootMtl->GetSubMtl(iSub);
			SMtrl  sSubMtrl;
			sSubMtrl.szName = FixupName(pSubMtl->GetName());
			GetTexture(pSubMtl, sSubMtrl);
			sMtrl.subMtrl.push_back(sSubMtrl);
		}
	}
	else
	{
		GetTexture(pRootMtl, sMtrl);
	}
	m_MtrlList.push_back(sMtrl);
}

int	SSSWriter::IsEqulVerteList(PNCT& vertex, vertexList& vList)
{
	for (int iVer = 0; iVer < vList.size(); iVer++)
	{
		if (EqualPoint3(vertex.p, vList[iVer].p) &&
			EqualPoint3(vertex.n, vList[iVer].n) &&
			EqualPoint4(vertex.c, vList[iVer].c) &&
			EqualPoint2(vertex.t, vList[iVer].t))
		{
			return iVer;
		}
	}
	return -1;
}
SSSWriter::SSSWriter()
{
}


SSSWriter::~SSSWriter()
{
}
