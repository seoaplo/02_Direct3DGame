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
	m_pRootNode = m_pMax->GetRootNode();		// �ֻ�� ��带 �����صд�.
	PreProcess(m_pRootNode);					// ����Լ�
}
void  SSSWriter::PreProcess(INode* pNode)
{
	// ���� �����ϸ� ��� �Լ� ����
	if (pNode == NULL) return;			

	// ��� ����
	AddObject(pNode);										

	// �ڽ� ����� ���� ���� ���� �ݺ��Ѵ�.
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
		// ���ʿ��� ���(�þ��� ���� � ���̴� ���)
		if (os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))
		{
			return;
		}

		switch (os.obj->SuperClassID())
		{
		// �ش� ����̸� ����
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
	_ftprintf(pStream, _T("%s %d"), _T("sssexporter100"),
		m_ObjectList.size());
	for (int iObj = 0; iObj < m_ObjectList.size(); iObj++)
	{
		INode* pNode = m_ObjectList[iObj];

		GetMesh(pNode);

		_ftprintf(pStream, _T("\n%s %d"),
			pNode->GetName(),
			m_triList.size());

		for (int iTri = 0; iTri < m_triList.size(); iTri++)
		{
			for (int iVer = 0; iVer < 3; iVer++)
			{
				_ftprintf(pStream, _T("\n%10.4f %10.4f %10.4f"),
					m_triList[iTri].v[iVer].p.x,
					m_triList[iTri].v[iVer].p.y,
					m_triList[iTri].v[iVer].p.z);
				_ftprintf(pStream, _T("%10.4f %10.4f %10.4f"),
					m_triList[iTri].v[iVer].n.x,
					m_triList[iTri].v[iVer].n.y,
					m_triList[iTri].v[iVer].n.z);
				_ftprintf(pStream, _T("%10.4f %10.4f %10.4f %10.4f"),
					m_triList[iTri].v[iVer].c.x,
					m_triList[iTri].v[iVer].c.y,
					m_triList[iTri].v[iVer].c.z,
					m_triList[iTri].v[iVer].c.w);
				_ftprintf(pStream, _T("%10.4f %10.4f"),
					m_triList[iTri].v[iVer].t.x,
					m_triList[iTri].v[iVer].t.y);
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
	// ������Ʈ�� �޴´�.
	Object* obj = pNode->EvalWorldState(time).obj;
	// ��ȯ �����ϸ� ����
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
	// ���� ��ǥ���̸� ���� ���, �ƴϸ� ���� ���
	Matrix3 tm = pNode->GetObjTMAfterWSM(0);
	
	bool deleteit = false;
	// Ʈ���̾ޱ� ������Ʈ
	TriObject* tri = GetTriObjectFromNode(pNode, 0, deleteit);
	if (tri == nullptr) return;
	// �޽� ������Ʈ
	Mesh* mesh = &tri->GetMesh();
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
				Point3 v = mesh->verts[mesh->faces[iFace].v[0]] * tm;
				DumpPoint3(m_triList[iFace].v[0].p, v);
				
				v = mesh->verts[mesh->faces[iFace].v[2]] * tm;
				DumpPoint3(m_triList[iFace].v[1].p, v);

				v = mesh->verts[mesh->faces[iFace].v[1]] * tm;
				DumpPoint3(m_triList[iFace].v[2].p, v);

				v = mesh->verts[mesh->faces[iFace].v[2]] * tm;
			}
			// color
			int iNumColor = mesh->getNumVertCol();
			m_triList[iFace].v[0].c = Point4(1, 1, 1, 1);
			m_triList[iFace].v[1].c = Point4(1, 1, 1, 1);
			m_triList[iFace].v[2].c = Point4(1, 1, 1, 1);
			if (iNumColor > 0)
			{
				m_triList[iFace].v[0].c =
					mesh->vertCol[mesh->vcFace[iFace].t[0]];
				m_triList[iFace].v[1].c =
					mesh->vertCol[mesh->vcFace[iFace].t[2]];
				m_triList[iFace].v[2].c =
					mesh->vertCol[mesh->vcFace[iFace].t[1]];
			}
			// texcoord
			int iNumTex = mesh->getNumTVerts();
			if (iNumTex > 0)
			{
				Point2 v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[0]];
				m_triList[iFace].v[0].t.x = v.x;
				m_triList[iFace].v[0].t.y = 1.0f - v.y;

				v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[2]];
				m_triList[iFace].v[1].t.x = v.x;
				m_triList[iFace].v[1].t.y = 1.0f - v.y;
				v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[1]];
				m_triList[iFace].v[2].t.x = v.x;
				m_triList[iFace].v[2].t.y = 1.0f - v.y;
			}

			m_triList[iFace].v[0].n = Point3(1, 1, 1);
			m_triList[iFace].v[1].n = Point3(1, 1, 1);
			m_triList[iFace].v[2].n = Point3(1, 1, 1);
			// n list,   face normal list
		}
		// vb, ib		
	}

	if (deleteit) delete tri;
}


SSSWriter::SSSWriter()
{
}


SSSWriter::~SSSWriter()
{
}
