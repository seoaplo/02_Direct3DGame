#include "header.h"
#include "SSSWriter.h"
#include <iostream>
#include <fstream>

#include "SSSWriter.h"

TCHAR* SSSWriter::FixupName(MSTR name)
{
	TCHAR m_tmpBuffer[MAX_PATH] = { 0, };
	memset(m_tmpBuffer, 0, sizeof(TCHAR)*MAX_PATH);

	TCHAR* cPtr;
	_tcscpy_s(m_tmpBuffer, name);
	cPtr = m_tmpBuffer;

	while (*cPtr)
	{
		if (*cPtr == '"')		*cPtr = SINGLE_QUOTE;
		else if (*cPtr == ' ' || *cPtr <= CTL_CHARS)
			*cPtr = _T('_');
		cPtr++;
	}
	return m_tmpBuffer;
}

void  SSSWriter::GetMaterial(INode* pNode)
{
	Mtl* pRootMtl = pNode->GetMtl();
	SMtrl tMtrl;
	tMtrl.szName = FixupName(pRootMtl->GetName());
	// sub-material
	int iNumSub = pRootMtl->NumSubMtls();
	if (iNumSub > 0)
	{
		for (int iSub = 0; iSub < iNumSub; iSub++)
		{
			Mtl* pSubMtl = pRootMtl->GetSubMtl(iSub);
			SMtrl  tSubMtrl;
			tSubMtrl.szName = FixupName(pSubMtl->GetName());
			GetTexture(pSubMtl, tSubMtrl);
			tMtrl.subMtrl.push_back(tSubMtrl);
		}
	}
	else
	{
		GetTexture(pRootMtl, tMtrl);
	}
	m_MtrlList.push_back(tMtrl);
}
void  SSSWriter::GetTexture(Mtl* pMtl, SMtrl& tMtrl)
{
	int iNumMap = pMtl->NumSubTexmaps();
	for (int iSubMap = 0; iSubMap < iNumMap; iSubMap++)
	{
		Texmap* tex = pMtl->GetSubTexmap(iSubMap);
		if (tex)
		{
			if (tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0X00))
			{
				STexMap tTex;
				tTex.iMapID = iSubMap;

				TSTR fullName;
				TSTR mapName = ((BitmapTex*)(tex))->GetMapName();
				SplitPathFile(mapName, &fullName, &tTex.szName);
				tMtrl.texList.push_back(tTex);
			}
		}
	}
}
void  SSSWriter::Set(const TCHAR* name, Interface* pMax)
{
	m_pMax = pMax;
	m_filename = name;
	m_pRootNode = m_pMax->GetRootNode();
	PreProcess(m_pRootNode);
}
void  SSSWriter::PreProcess(INode* pNode)
{
	if (pNode == NULL) return;
	AddObject(pNode);
	AddMaterial(pNode);
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
int SSSWriter::FindMaterial(INode* pNode)
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
		if (os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))
		{
			return;
		}

		switch (os.obj->SuperClassID())
		{
		case GEOMOBJECT_CLASS_ID:
			//case HELPER_CLASS_ID:
			m_ObjectList.push_back(pNode);
		default:
			break;
		}
	}
}
bool  SSSWriter::Convert()
{
	for (int iObj = 0; iObj < m_ObjectList.size(); iObj++)
	{
		INode* pNode = m_ObjectList[iObj];
		SMesh tMesh;
		tMesh.name = FixupName(pNode->GetName());
		GetMesh(pNode, tMesh);
		tMesh.iMtrlID = FindMaterial(pNode);
		m_tMeshList.push_back(tMesh);
	}
	return true;
}
bool  SSSWriter::Export()
{
	Convert();

	FILE* pStream = nullptr;
	// header	
	_wfopen_s(&pStream, m_filename.c_str(), _T("wb"));
	_ftprintf(pStream, _T("%s"), _T("kkkexporter100"));

	_ftprintf(pStream, _T("\n%s"), L"#HEADER INFO");
	_ftprintf(pStream, _T("%d %d"),
		m_tMeshList.size(),
		m_MtrlList.size());

	// root material
	_ftprintf(pStream, _T("\n%s"), L"#MATERIAL INFO");
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
				m_MtrlList[iMtl].szName,
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
	_ftprintf(pStream, _T("\n%s"), L"#OBJECT INFO");
	for (int iObj = 0; iObj < m_tMeshList.size(); iObj++)
	{
		_ftprintf(pStream, _T("\n%s %d %d %d"),
			m_tMeshList[iObj].name,
			m_tMeshList[iObj].iMtrlID,
			m_tMeshList[iObj].bufferList.size(),
			m_tMeshList[iObj].triList.size());

		for (int iSubTri = 0; iSubTri < m_tMeshList[iObj].bufferList.size(); iSubTri++)
		{
			vectorTriList& triList = m_tMeshList[iObj].bufferList[iSubTri];
			for (int iTri = 0; iTri <
				triList.size(); iTri++)
			{
				for (int iVer = 0; iVer < 3; iVer++)
				{
					_ftprintf(pStream, _T("\n%10.4f %10.4f %10.4f"),
						triList[iTri].v[iVer].p.x,
						triList[iTri].v[iVer].p.y,
						triList[iTri].v[iVer].p.z);
					_ftprintf(pStream, _T("%10.4f %10.4f %10.4f"),
						triList[iTri].v[iVer].n.x,
						triList[iTri].v[iVer].n.y,
						triList[iTri].v[iVer].n.z);
					_ftprintf(pStream, _T("%10.4f %10.4f %10.4f %10.4f"),
						triList[iTri].v[iVer].c.x,
						triList[iTri].v[iVer].c.y,
						triList[iTri].v[iVer].c.z,
						triList[iTri].iSubIndex);
					_ftprintf(pStream, _T("%10.4f %10.4f"),
						triList[iTri].v[iVer].t.x,
						triList[iTri].v[iVer].t.y);
				}
			}
		}
	}
	fclose(pStream);

	MessageBox(GetActiveWindow(), m_filename.c_str(),
		_T("Succeed!"), MB_OK);
	return true;
}
// INode -> TriObject 변환한다.
TriObject* SSSWriter::GetTriObjectFromNode(
	INode* pNode, TimeValue time, bool& deleteit)
{
	Object* obj = pNode->EvalWorldState(time).obj;
	if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
	{
		TriObject* tri = (TriObject*)obj->ConvertToType(time, Class_ID(TRIOBJ_CLASS_ID, 0));
		if (obj != tri) deleteit = true;
		return tri;
	}
	return nullptr;
}
void SSSWriter::GetMesh(INode* pNode, SMesh& tMesh)
{
	Matrix3 tm = pNode->GetObjTMAfterWSM(0);
	// 1) 트라이엥글 오브젝트]
	bool deleteit = false;
	TriObject* tri = GetTriObjectFromNode(pNode, 0, deleteit);
	if (tri == nullptr) return;
	// 2) 메쉬 오브젝트
	Mesh* mesh = &tri->GetMesh();
	bool negScale = TMNegParity(tm);
	int v0, v1, v2;
	if (negScale) { v0 = 2; v1 = 1; v2 = 0; }
	else { v0 = 0; v1 = 1; v2 = 2; }

	if (mesh)
	{
		int iNumFace = mesh->getNumFaces();

		vectorTriList triList;
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
			triList[iFace].iSubIndex =
				mesh->faces[iFace].getMatID();
		}
		// buffer sort

		tMesh.bufferList.push_back(triList);
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

SSSWriter::SSSWriter()
{
}


SSSWriter::~SSSWriter()
{
}
