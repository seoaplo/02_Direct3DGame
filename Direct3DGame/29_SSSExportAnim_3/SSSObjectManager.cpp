#include "SSSObjectManager.h"


void SSSObjectManager::AddObject(INode* pNode, SScene& Scene, Interval& interval, int iMaterialID)
{
	ObjectState os = pNode->EvalWorldState(interval.Start());
	Object* CheckID = nullptr;
	int ChildNum = pNode->NumChildren();
	int iObjectNumber = -1;

	SObject Object;
	iObjectNumber = m_ObjectList.size();
	CheckID = pNode->GetObjectRef();

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
		case HELPER_CLASS_ID:
			
			if (CheckID->ClassID() == Class_ID(BONE_CLASS_ID, 0))// 본 오브젝트   
			{
				if (ChildNum <= 0) return;
				Object.m_ClassType = CLASS_BONE;
			}
			else if (CheckID->ClassID() == Class_ID(DUMMY_CLASS_ID, 0))  // 더미 오브젝트
			{
				if (ChildNum <= 0) return;
				Object.m_ClassType = CLASS_DUMMY;
			}
			else
			{
				Object.m_ClassType = CLASS_GEOM;
				if (iMaterialID < 0) return;
				else Object.m_ClassType = CLASS_GEOM;
			}
			break;
		default:
			return;
		}
	}
	else
	{
		return;
	}
	INode* pParentNode = pNode->GetParentNode();
	if (pParentNode &&	pParentNode->IsRootNode() == false)
	{
		Object.ParentName = SSSGlobal::FixupName(pParentNode->GetName());
	}
	Object.name = SSSGlobal::FixupName(pNode->GetName());
	Matrix3 wtm = pNode->GetNodeTM(interval.Start());
	SSSGlobal::DumpMatrix3(Object.matWorld, &wtm);

	if (Object.m_ClassType == CLASS_GEOM)
	{
		Object.m_Mesh.iMaterialID = iMaterialID;
		GetMesh(pNode, Object.m_Mesh, interval);
	}
	else
	{
		GetBox(Object.m_Mesh);
	}

	GetAnimation(pNode, Object.m_AnimTrack, Scene, interval);

	m_ObjectList.push_back(Object);
}

void SSSObjectManager::GetMesh(INode* pNode, SMesh& sMesh, Interval& interval)
{
	for (int iCount = 0; iCount < SUBMATERIAL_SIZE; iCount++)
	{
		m_TriList[iCount].clear();
	}

	// 로컬 좌표계이면 월드 행렬, 아니면 단위 행렬
	Matrix3 tm = pNode->GetObjTMAfterWSM(interval.Start());

	bool deleteit = false;
	// 트라이앵글 오브젝트
	TriObject* tri = GetTriObjectFromNode(pNode, interval.Start(), deleteit);
	if (tri == nullptr) return;
	// 메쉬 오브젝트
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
	sMesh.iSubNum = 1;
	sMesh.SubMeshList[0].bUse = true;

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
		if (m_TriList[iSub].size() <= 0) continue;

		sMesh.iSubNum++;
		sMesh.SubMeshList[iSub].bUse = true;

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
Point3	SSSObjectManager::GetVertexNormal(Mesh* mesh, int iFace, RVertex* rVertex)
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


void SSSObjectManager::GetAnimation(INode* pNode, SAnimationTrack& AnimTrack, SScene& Scene, Interval& interval)
{


	//-------------------> 중요
	TimeValue startFrame = interval.Start();
	// tm = selfTm * parentTm * Inverse(parentTm);
	Matrix3 tm = pNode->GetNodeTM(startFrame)
		* Inverse(pNode->GetParentTM(startFrame));
	// 행렬 분해
	AffineParts StartAP;
	decomp_affine(tm, &StartAP);
	// quaternion -> axis, angle 변환
	Point3  Start_RotAxis;
	float   Start_RotValue;
	AngAxisFromQ(StartAP.q, &Start_RotValue, Start_RotAxis);
	//<----------------------------

	SPositionAnim	PosTrack;
	SRotationAnim	RotTrack;
	SScaleAnim		ScaleTrack;

	ZeroMemory(&PosTrack, sizeof(PosTrack));
	ZeroMemory(&RotTrack, sizeof(RotTrack));
	ZeroMemory(&ScaleTrack, sizeof(ScaleTrack));


	TimeValue start = interval.Start();
	TimeValue end = interval.End();
	// 시작+1프레임
	for (TimeValue t = start; t <= end; t += GetTicksPerFrame())
	{
		Matrix3 tm = pNode->GetNodeTM(t)
			* Inverse(pNode->GetParentTM(t));

		AffineParts FrameAP;
		decomp_affine(tm, &FrameAP);

		PosTrack.i = t;
		PosTrack.p = FrameAP.t;
		AnimTrack.PositionTrack.push_back(PosTrack);

		RotTrack.i = t;
		RotTrack.q = FrameAP.q;
		AnimTrack.RotationTrack.push_back(RotTrack);

		ScaleTrack.p = FrameAP.k;
		ScaleTrack.q = FrameAP.u;
		AnimTrack.ScaleTrack.push_back(ScaleTrack);

		Point3  Frame_RotAxis;
		float   Frame_RotValue;
		AngAxisFromQ(FrameAP.q, &Frame_RotValue, Frame_RotAxis);


		if (!AnimTrack.bPosition) {
			if (!SSSGlobal::EqualPoint3(StartAP.t, FrameAP.t))
			{
				AnimTrack.bPosition = true;
			}
		}

		if (!AnimTrack.bRotation) {
			if (!SSSGlobal::EqualPoint3(Start_RotAxis, Frame_RotAxis))
			{
				AnimTrack.bRotation = true;
			}
			else
			{
				if (Start_RotValue != Frame_RotValue)
				{
					AnimTrack.bRotation = true;
				}
			}
		}

		if (!AnimTrack.bScale) {
			if (!SSSGlobal::EqualPoint3(StartAP.k, FrameAP.k))
			{
				AnimTrack.bScale = true;
			}
		}
	}
}
bool SSSObjectManager::ExportObject(FILE* pStream)
{
	if (pStream == nullptr)
	{
		return false;
	}
	_ftprintf(pStream, _T("\n%s"), L"#OBJECT_INFO");

	for (int iObj = 0; iObj <m_ObjectList.size(); iObj++)
	{

		_ftprintf(pStream, _T("\n%s %s %d %d %d"),
			m_ObjectList[iObj].name,
			m_ObjectList[iObj].ParentName,
			m_ObjectList[iObj].m_ClassType,
			m_ObjectList[iObj].m_Mesh.iMaterialID,
			m_ObjectList[iObj].m_Mesh.iSubNum);

		// World Matrix
		_ftprintf(pStream, _T("\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f"),
			m_ObjectList[iObj].matWorld._11,
			m_ObjectList[iObj].matWorld._12,
			m_ObjectList[iObj].matWorld._13,
			m_ObjectList[iObj].matWorld._14,

			m_ObjectList[iObj].matWorld._21,
			m_ObjectList[iObj].matWorld._22,
			m_ObjectList[iObj].matWorld._23,
			m_ObjectList[iObj].matWorld._24,

			m_ObjectList[iObj].matWorld._31,
			m_ObjectList[iObj].matWorld._32,
			m_ObjectList[iObj].matWorld._33,
			m_ObjectList[iObj].matWorld._34,

			m_ObjectList[iObj].matWorld._41,
			m_ObjectList[iObj].matWorld._42,
			m_ObjectList[iObj].matWorld._43,
			m_ObjectList[iObj].matWorld._44);

		ExportMesh(pStream, m_ObjectList[iObj].m_Mesh);
		ExportAnimation(pStream, m_ObjectList[iObj].m_AnimTrack);
	}

	return true;
}
bool SSSObjectManager::ExportMesh(FILE* pStream, SMesh& sMesh)
{
	if (pStream == nullptr)
	{
		return false;
	}
	for (int iSubMesh = 0; iSubMesh < SUBMATERIAL_SIZE; iSubMesh++)
	{
		if (sMesh.SubMeshList[iSubMesh].bUse == false) continue;
		std::vector<PNCT>& vList = sMesh.SubMeshList[iSubMesh].VertexList;
		std::vector<DWORD>& iList = sMesh.SubMeshList[iSubMesh].IndexList;

		_ftprintf(pStream, _T("\nSubMesh %d %d %d"),
			iSubMesh, vList.size(), iList.size());

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

		for (int iIndex = 0; iIndex < iList.size(); iIndex += 3)
		{
			_ftprintf(pStream, _T("\n%d %d %d"),
				iList[iIndex + 0],
				iList[iIndex + 1],
				iList[iIndex + 2]);
		}
	}


	return true;
}
bool SSSObjectManager::ExportAnimation(FILE* pStream, SAnimationTrack& AnimTrack)
{
	if (pStream == nullptr)
	{
		return false;
	}

	_ftprintf(pStream, _T("\n%s %d %d %d"),
		L"#AnimationData",
		(AnimTrack.bPosition) ? AnimTrack.PositionTrack.size() : 0,
		(AnimTrack.bRotation) ? AnimTrack.RotationTrack.size() : 0,
		(AnimTrack.bScale) ? AnimTrack.ScaleTrack.size() : 0);
	if (AnimTrack.bPosition)
	{
		for (int iTrack = 0; iTrack < AnimTrack.PositionTrack.size(); iTrack++)
		{
			_ftprintf(pStream, _T("\n%d %d %10.4f %10.4f %10.4f"),
				iTrack,
				AnimTrack.PositionTrack[iTrack].i,
				AnimTrack.PositionTrack[iTrack].p.x,
				AnimTrack.PositionTrack[iTrack].p.z,
				AnimTrack.PositionTrack[iTrack].p.y);
		}
	}
	if (AnimTrack.bRotation)
	{
		for (int iTrack = 0; iTrack < AnimTrack.RotationTrack.size(); iTrack++)
		{
			_ftprintf(pStream, _T("\n%d %d %10.4f %10.4f %10.4f %10.4f"),
				iTrack,
				AnimTrack.RotationTrack[iTrack].i,
				AnimTrack.RotationTrack[iTrack].q.x,
				AnimTrack.RotationTrack[iTrack].q.z,
				AnimTrack.RotationTrack[iTrack].q.y,
				AnimTrack.RotationTrack[iTrack].q.w);
		}
	}
	if (AnimTrack.bScale)
	{
		for (int iTrack = 0; iTrack < AnimTrack.ScaleTrack.size(); iTrack++)
		{
			_ftprintf(pStream, _T("\n%d %d %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f"),
				iTrack,
				AnimTrack.ScaleTrack[iTrack].i,
				AnimTrack.ScaleTrack[iTrack].p.x,
				AnimTrack.ScaleTrack[iTrack].p.z,
				AnimTrack.ScaleTrack[iTrack].p.y,
				AnimTrack.ScaleTrack[iTrack].q.x,
				AnimTrack.ScaleTrack[iTrack].q.z,
				AnimTrack.ScaleTrack[iTrack].q.y,
				AnimTrack.ScaleTrack[iTrack].q.w);
		}
	}
	return true;
}

SSSObjectManager::SSSObjectManager()
{
}


SSSObjectManager::~SSSObjectManager()
{
}
