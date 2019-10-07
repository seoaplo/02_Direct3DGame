#include "SSSObjectManager.h"


void SSSObjectManager::AddObject(INode* pNode, SScene& Scene, Interval& interval, int iMaterialID)
{
	ObjectState os = pNode->EvalWorldState(interval.Start());
	int iObjectNumber = -1;
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
		case HELPER_CLASS_ID:
			m_ObjectList.push_back(SObject());
			iObjectNumber = m_ObjectList.size() - 1;
			Object* CheckID = pNode->GetObjectRef();
			if (CheckID->ClassID() == Class_ID(BONE_CLASS_ID, 0))// �� ������Ʈ   
			{
				m_ObjectList[iObjectNumber].m_ClassType = CLASS_BONE;
			}
			else if (CheckID->ClassID() == Class_ID(DUMMY_CLASS_ID, 0))  // ���� ������Ʈ
			{
				m_ObjectList[iObjectNumber].m_ClassType = CLASS_DUMMY;
			}
			else
			{
				m_ObjectList[iObjectNumber].m_ClassType = CLASS_GEOM;
				if (iMaterialID < 0) return;
				else m_ObjectList[iObjectNumber].m_Mesh.iMaterialID = iMaterialID;
			}
			break;
		default:
			return;
		}
	}
	INode* pParentNode = pNode->GetParentNode();
	if (pParentNode &&	pParentNode->IsRootNode() == false)
	{
		m_ObjectList[iObjectNumber].ParentName = SSSGlobal::FixupName(pParentNode->GetName());
	}

	Matrix3 wtm = pNode->GetNodeTM(interval.Start());
	SSSGlobal::DumpMatrix3(m_ObjectList[iObjectNumber].matWorld, &wtm);

	if (m_ObjectList[iObjectNumber].m_ClassType == CLASS_GEOM)
	{
		GetMesh(pNode, m_ObjectList[iObjectNumber].m_Mesh, interval);
	}
	else
	{
		GetBox(m_ObjectList[iObjectNumber].m_Mesh);
	}

	GetAnimation(pNode, m_ObjectList[iObjectNumber].m_Mesh, Scene, interval);
}

void SSSObjectManager::GetMesh(INode* pNode, SMesh& sMesh, Interval& interval)
{
	for (int iCount = 0; iCount < SUBMATERIAL_SIZE; iCount++)
	{
		m_TriList[iCount].clear();
	}

	// ���� ��ǥ���̸� ���� ���, �ƴϸ� ���� ���
	Matrix3 tm = pNode->GetObjTMAfterWSM(interval.Start());

	bool deleteit = false;
	// Ʈ���̾ޱ� ������Ʈ
	TriObject* tri = GetTriObjectFromNode(pNode, interval.Start(), deleteit);
	if (tri == nullptr) return;
	// �޽� ������Ʈ
	Mesh* mesh = &tri->GetMesh();
	bool negScale = SSSGlobal::TMNegParity(tm);

	int v0, v1, v2;
	if (negScale) { v0 = 2; v1 = 1; v2 = 0; }
	else { v0 = 0; v1 = 1; v2 = 2; }

	if (mesh)
	{
		int iNumFace = mesh->getNumFaces();

		for (int iFace = 0; iFace < iNumFace; iFace++)
		{
			int iSubIndex = mesh->faces[iFace].getMatID();
			if (iSubIndex > SUBMATERIAL_SIZE ||
				iSubIndex < 0)
			{
				continue;
			}
			m_TriList[iSubIndex].push_back(STriangle());
			int iFaceNum = m_TriList[iSubIndex].size() - 1;

			// position
			int iNumPos = mesh->getNumVerts();
			if (iNumPos > 0)
			{
				Point3 v = mesh->verts[mesh->faces[iFace].v[v0]] * tm;
				SSSGlobal::DumpPoint3(m_TriList[iSubIndex][iFaceNum].v[0].p, v);

				v = mesh->verts[mesh->faces[iFace].v[v2]] * tm;
				SSSGlobal::DumpPoint3(m_TriList[iSubIndex][iFaceNum].v[1].p, v);

				v = mesh->verts[mesh->faces[iFace].v[v1]] * tm;
				SSSGlobal::DumpPoint3(m_TriList[iSubIndex][iFaceNum].v[2].p, v);
			}
			// color
			int iNumColor = mesh->getNumVertCol();
			m_TriList[iSubIndex][iFaceNum].v[0].c = Point4(1, 1, 1, 1);
			m_TriList[iSubIndex][iFaceNum].v[1].c = Point4(1, 1, 1, 1);
			m_TriList[iSubIndex][iFaceNum].v[2].c = Point4(1, 1, 1, 1);
			if (iNumColor > 0)
			{
				m_TriList[iSubIndex][iFaceNum].v[0].c =
					mesh->vertCol[mesh->vcFace[iFace].t[v0]];
				m_TriList[iSubIndex][iFaceNum].v[1].c =
					mesh->vertCol[mesh->vcFace[iFace].t[v2]];
				m_TriList[iSubIndex][iFaceNum].v[2].c =
					mesh->vertCol[mesh->vcFace[iFace].t[v1]];
			}
			// texcoord
			int iNumTex = mesh->getNumTVerts();
			if (iNumTex > 0)
			{
				Point2 v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[v0]];
				m_TriList[iSubIndex][iFaceNum].v[0].t.x = v.x;
				m_TriList[iSubIndex][iFaceNum].v[0].t.y = 1.0f - v.y;

				v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[v2]];
				m_TriList[iSubIndex][iFaceNum].v[1].t.x = v.x;
				m_TriList[iSubIndex][iFaceNum].v[1].t.y = 1.0f - v.y;
				v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[v1]];
				m_TriList[iSubIndex][iFaceNum].v[2].t.x = v.x;
				m_TriList[iSubIndex][iFaceNum].v[2].t.y = 1.0f - v.y;
			}

			mesh->buildNormals();
			int vert = mesh->faces[iFace].getVert(v0);
			RVertex* rVertex = mesh->getRVertPtr(vert);
			Point3 vn = GetVertexNormal(mesh, iFace, rVertex);
			SSSGlobal::DumpPoint3(m_TriList[iSubIndex][iFaceNum].v[v0].n, vn);

			vert = mesh->faces[iFace].getVert(v2);
			rVertex = mesh->getRVertPtr(vert);
			vn = GetVertexNormal(mesh, iFace, rVertex);
			SSSGlobal::DumpPoint3(m_TriList[iSubIndex][iFaceNum].v[v1].n, vn);

			vert = mesh->faces[iFace].getVert(v1);
			rVertex = mesh->getRVertPtr(vert);
			vn = GetVertexNormal(mesh, iFace, rVertex);
			SSSGlobal::DumpPoint3(m_TriList[iSubIndex][iFaceNum].v[v2].n, vn);
		}

		// vb, ib
		SetUniqueBuffer(sMesh);
	}

	if (deleteit) delete tri;
}
void SSSObjectManager::GetBox(SMesh& sMesh)
{
	sMesh.SubMeshList[0].VertexList.resize(24);
	sMesh.SubMeshList[0].IndexList.resize(36);

	sMesh.SubMeshList[0].VertexList[0] = PNCT(Point3(-1.0f, 1.0f, -1.0f), Point3(0.0f, 0.0f, -1.0f), Point4(1.0f, 0.0f, 0.0f, 1.0f), Point2(0.0f, 0.0f));
	sMesh.SubMeshList[0].VertexList[1] = PNCT(Point3(1.0f, 1.0f, -1.0f), Point3(0.0f, 0.0f, -1.0f), Point4(1.0f, 0.0f, 0.0f, 1.0f), Point2(1.0f, 0.0f));
	sMesh.SubMeshList[0].VertexList[2] = PNCT(Point3(1.0f, -1.0f, -1.0f), Point3(0.0f, 0.0f, -1.0f), Point4(1.0f, 0.0f, 0.0f, 1.0f), Point2(1.0f, 1.0f));
	sMesh.SubMeshList[0].VertexList[3] = PNCT(Point3(-1.0f, -1.0f, -1.0f), Point3(0.0f, 0.0f, -1.0f), Point4(1.0f, 0.0f, 0.0f, 1.0f), Point2(0.0f, 1.0f));

	sMesh.SubMeshList[0].VertexList[4] = PNCT(Point3(1.0f, 1.0f, 1.0f), Point3(0.0f, 0.0f, 1.0f), Point4(0.0f, 0.0f, 0.0f, 1.0f), Point2(0.0f, 0.0f));
	sMesh.SubMeshList[0].VertexList[5] = PNCT(Point3(-1.0f, 1.0f, 1.0f), Point3(0.0f, 0.0f, 1.0f), Point4(0.0f, 1.0f, 0.0f, 1.0f), Point2(1.0f, 0.0f));
	sMesh.SubMeshList[0].VertexList[6] = PNCT(Point3(-1.0f, -1.0f, 1.0f), Point3(0.0f, 0.0f, 1.0f), Point4(0.0f, 1.0f, 0.0f, 1.0f), Point2(1.0f, 1.0f));
	sMesh.SubMeshList[0].VertexList[7] = PNCT(Point3(1.0f, -1.0f, 1.0f), Point3(0.0f, 0.0f, 1.0f), Point4(0.0f, 1.0f, 0.0f, 1.0f), Point2(0.0f, 1.0f));

	sMesh.SubMeshList[0].VertexList[8] = PNCT(Point3(1.0f, 1.0f, -1.0f), Point3(1.0f, 0.0f, 0.0f), Point4(0.0f, 0.0f, 1.0f, 1.0f), Point2(0.0f, 0.0f));
	sMesh.SubMeshList[0].VertexList[9] = PNCT(Point3(1.0f, 1.0f, 1.0f), Point3(1.0f, 0.0f, 0.0f), Point4(0.0f, 0.0f, 1.0f, 1.0f), Point2(1.0f, 0.0f));
	sMesh.SubMeshList[0].VertexList[10] = PNCT(Point3(1.0f, -1.0f, 1.0f), Point3(1.0f, 0.0f, 0.0f), Point4(0.0f, 0.0f, 1.0f, 1.0f), Point2(1.0f, 1.0f));
	sMesh.SubMeshList[0].VertexList[11] = PNCT(Point3(1.0f, -1.0f, -1.0f), Point3(1.0f, 0.0f, 0.0f), Point4(0.0f, 0.0f, 1.0f, 1.0f), Point2(0.0f, 1.0f));

	sMesh.SubMeshList[0].VertexList[12] = PNCT(Point3(-1.0f, 1.0f, 1.0f), Point3(-1.0f, 0.0f, 0.0f), Point4(1.0f, 1.0f, 0.0f, 1.0f), Point2(0.0f, 0.0f));
	sMesh.SubMeshList[0].VertexList[13] = PNCT(Point3(-1.0f, 1.0f, -1.0f), Point3(-1.0f, 0.0f, 0.0f), Point4(1.0f, 1.0f, 0.0f, 1.0f), Point2(1.0f, 0.0f));
	sMesh.SubMeshList[0].VertexList[14] = PNCT(Point3(-1.0f, -1.0f, -1.0f), Point3(-1.0f, 0.0f, 0.0f), Point4(1.0f, 1.0f, 0.0f, 1.0f), Point2(1.0f, 1.0f));
	sMesh.SubMeshList[0].VertexList[15] = PNCT(Point3(-1.0f, -1.0f, 1.0f), Point3(-1.0f, 0.0f, 0.0f), Point4(1.0f, 1.0f, 0.0f, 1.0f), Point2(0.0f, 1.0f));

	sMesh.SubMeshList[0].VertexList[16] = PNCT(Point3(-1.0f, 1.0f, 1.0f), Point3(0.0f, 1.0f, 0.0f), Point4(1.0f, 0.5f, 1.0f, 1.0f), Point2(0.0f, 0.0f));
	sMesh.SubMeshList[0].VertexList[17] = PNCT(Point3(1.0f, 1.0f, 1.0f), Point3(0.0f, 1.0f, 0.0f), Point4(1.0f, 0.5f, 1.0f, 1.0f), Point2(1.0f, 0.0f));
	sMesh.SubMeshList[0].VertexList[18] = PNCT(Point3(1.0f, 1.0f, -1.0f), Point3(0.0f, 1.0f, 0.0f), Point4(1.0f, 0.5f, 1.0f, 1.0f), Point2(1.0f, 1.0f));
	sMesh.SubMeshList[0].VertexList[19] = PNCT(Point3(-1.0f, 1.0f, -1.0f), Point3(0.0f, 1.0f, 0.0f), Point4(1.0f, 0.5f, 1.0f, 1.0f), Point2(0.0f, 1.0f));

	sMesh.SubMeshList[0].VertexList[20] = PNCT(Point3(-1.0f, -1.0f, -1.0f), Point3(0.0f, -1.0f, 0.0f), Point4(0.0f, 1.0f, 1.0f, 1.0f), Point2(0.0f, 0.0f));
	sMesh.SubMeshList[0].VertexList[21] = PNCT(Point3(1.0f, -1.0f, -1.0f), Point3(0.0f, -1.0f, 0.0f), Point4(0.0f, 1.0f, 1.0f, 1.0f), Point2(1.0f, 0.0f));
	sMesh.SubMeshList[0].VertexList[22] = PNCT(Point3(1.0f, -1.0f, 1.0f), Point3(0.0f, -1.0f, 0.0f), Point4(0.0f, 1.0f, 1.0f, 1.0f), Point2(1.0f, 1.0f));
	sMesh.SubMeshList[0].VertexList[23] = PNCT(Point3(-1.0f, -1.0f, 1.0f), Point3(0.0f, -1.0f, 0.0f), Point4(0.0f, 1.0f, 1.0f, 1.0f), Point2(0.0f, 1.0f));

	int iIndex = 0;
	sMesh.SubMeshList[0].IndexList.resize(36);

	sMesh.SubMeshList[0].IndexList[iIndex++] = 0; 	sMesh.SubMeshList[0].IndexList[iIndex++] = 1; 	sMesh.SubMeshList[0].IndexList[iIndex++] = 2; 	sMesh.SubMeshList[0].IndexList[iIndex++] = 0;	sMesh.SubMeshList[0].IndexList[iIndex++] = 2; 	sMesh.SubMeshList[0].IndexList[iIndex++] = 3;
	sMesh.SubMeshList[0].IndexList[iIndex++] = 4; 	sMesh.SubMeshList[0].IndexList[iIndex++] = 5; 	sMesh.SubMeshList[0].IndexList[iIndex++] = 6; 	sMesh.SubMeshList[0].IndexList[iIndex++] = 4;	sMesh.SubMeshList[0].IndexList[iIndex++] = 6; 	sMesh.SubMeshList[0].IndexList[iIndex++] = 7;
	sMesh.SubMeshList[0].IndexList[iIndex++] = 8; 	sMesh.SubMeshList[0].IndexList[iIndex++] = 9; 	sMesh.SubMeshList[0].IndexList[iIndex++] = 10; sMesh.SubMeshList[0].IndexList[iIndex++] = 8;	sMesh.SubMeshList[0].IndexList[iIndex++] = 10;	sMesh.SubMeshList[0].IndexList[iIndex++] = 11;
	sMesh.SubMeshList[0].IndexList[iIndex++] = 12;	sMesh.SubMeshList[0].IndexList[iIndex++] = 13;	sMesh.SubMeshList[0].IndexList[iIndex++] = 14;	sMesh.SubMeshList[0].IndexList[iIndex++] = 12;	sMesh.SubMeshList[0].IndexList[iIndex++] = 14;	sMesh.SubMeshList[0].IndexList[iIndex++] = 15;
	sMesh.SubMeshList[0].IndexList[iIndex++] = 16;	sMesh.SubMeshList[0].IndexList[iIndex++] = 17;	sMesh.SubMeshList[0].IndexList[iIndex++] = 18;	sMesh.SubMeshList[0].IndexList[iIndex++] = 16;	sMesh.SubMeshList[0].IndexList[iIndex++] = 18;	sMesh.SubMeshList[0].IndexList[iIndex++] = 19;
	sMesh.SubMeshList[0].IndexList[iIndex++] = 20;	sMesh.SubMeshList[0].IndexList[iIndex++] = 21;	sMesh.SubMeshList[0].IndexList[iIndex++] = 22;	sMesh.SubMeshList[0].IndexList[iIndex++] = 20;	sMesh.SubMeshList[0].IndexList[iIndex++] = 22;	sMesh.SubMeshList[0].IndexList[iIndex++] = 23;

	return;
}
void SSSObjectManager::SetUniqueBuffer(SMesh& sMesh)
{

	for (int iSub = 0; iSub < SUBMATERIAL_SIZE; iSub++)
	{
		for (int iFace = 0; iFace < m_TriList[iSub].size(); iFace++)
		{
			std::vector<STriangle>& triArray = m_TriList[iSub];
			STriangle& tri = triArray[iFace];
			std::vector<PNCT>& vList = sMesh.SubMeshList[iSub].VertexList;
			std::vector<DWORD>& iList = sMesh.SubMeshList[iSub].IndexList;
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
int	SSSObjectManager::IsEqulVerteList(PNCT& vertex, std::vector<PNCT>& vList)
{
	for (int iVer = 0; iVer < vList.size(); iVer++)
	{
		if (SSSGlobal::EqualPoint3(vertex.p, vList[iVer].p) &&
			SSSGlobal::EqualPoint3(vertex.n, vList[iVer].n) &&
			SSSGlobal::EqualPoint4(vertex.c, vList[iVer].c) &&
			SSSGlobal::EqualPoint2(vertex.t, vList[iVer].t))
		{
			return iVer;
		}
	}
	return -1;
}
Point3	GetVertexNormal(Mesh* mesh, int iFace, RVertex* rVertex)
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
TriObject* SSSObjectManager::GetTriObjectFromNode(INode* pNode, TimeValue time, bool& deleteit)
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


void SSSObjectManager::GetAnimation(INode* pNode, SObject& sObject, SScene& Scene, Interval& interval)
{


	//-------------------> �߿�
	TimeValue startFrame = interval.Start();
	// tm = selfTm * parentTm * Inverse(parentTm);
	Matrix3 tm = pNode->GetNodeTM(startFrame)
		* Inverse(pNode->GetParentTM(startFrame));
	// ��� ����
	AffineParts StartAP;
	decomp_affine(tm, &StartAP);
	// quaternion -> axis, angle ��ȯ
	Point3  Start_RotAxis;
	float   Start_RotValue;
	AngAxisFromQ(StartAP.q, &Start_RotValue, Start_RotAxis);
	//<----------------------------
	SAnimationTrack StartAnim;
	ZeroMemory(&StartAnim, sizeof(SAnimTrack));
	StartAnim.i = startFrame;
	StartAnim.p = StartAP.t;
	StartAnim.q = StartAP.q;
	sMesh.animPos.push_back(StartAnim);
	sMesh.animRot.push_back(StartAnim);
	StartAnim.p = StartAP.k;
	StartAnim.q = StartAP.u;
	sMesh.animScl.push_back(StartAnim);

	TimeValue start =
		m_Interval.Start() + GetTicksPerFrame();
	TimeValue end = m_Interval.End();
	// ����+1������
	for (TimeValue t = start; t <= end; t += GetTicksPerFrame())
	{
		Matrix3 tm = pNode->GetNodeTM(t)
			* Inverse(pNode->GetParentTM(t));

		AffineParts FrameAP;
		decomp_affine(tm, &FrameAP);

		SAnimTrack anim;
		ZeroMemory(&anim, sizeof(SAnimTrack));
		anim.i = t;
		anim.p = FrameAP.t;
		anim.q = FrameAP.q;
		sMesh.animPos.push_back(anim);
		sMesh.animRot.push_back(anim);
		anim.p = FrameAP.k;
		anim.q = FrameAP.u;
		sMesh.animScl.push_back(anim);

		Point3  Frame_RotAxis;
		float   Frame_RotValue;
		AngAxisFromQ(FrameAP.q, &Frame_RotValue, Frame_RotAxis);


		if (!sMesh.bAnimatin[0]) {
			if (!EqualPoint3(StartAP.t, FrameAP.t))
			{
				sMesh.bAnimatin[0] = true;
			}
		}

		if (!sMesh.bAnimatin[1]) {
			if (!EqualPoint3(Start_RotAxis, Frame_RotAxis))
			{
				sMesh.bAnimatin[1] = true;
			}
			else
			{
				if (Frame_RotValue != Frame_RotValue)
				{
					sMesh.bAnimatin[1] = true;
				}
			}
		}

		if (!sMesh.bAnimatin[2]) {
			if (!EqualPoint3(StartAP.k, FrameAP.k))
			{
				sMesh.bAnimatin[2] = true;
			}
		}
	}
}
SSSObjectManager::SSSObjectManager()
{
}


SSSObjectManager::~SSSObjectManager()
{
}