#include "SOAManager.h"

void SOAManager::AddObject(INode* pNode, SAScene& Scene, Interval& interval, int iObjectNum, int iMaterialID)
{
	ObjectState os = pNode->EvalWorldState(interval.Start());
	Object* CheckID = nullptr;
	int ChildNum = pNode->NumChildren();
	int iObjectNumber = -1;

	SOAObject& Object = m_ObjectList[iObjectNum];
	iObjectNumber = iObjectNum;
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
				Object.m_ClassType = CLASS_BONE;
			}
			else if (CheckID->ClassID() == Class_ID(DUMMY_CLASS_ID, 0))  // 더미 오브젝트
			{
				Object.m_ClassType = CLASS_DUMMY;
			}
			else
			{
				Object.m_ClassType = CLASS_GEOM;
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
		Object.ParentName = SAGlobal::FixupName(pParentNode->GetName());
	}
	Object.name = SAGlobal::FixupName(pNode->GetName());
	Matrix3 wtm = pNode->GetNodeTM(interval.Start());
	Matrix3 Invwtm = Inverse(wtm);
	SAGlobal::DumpMatrix3(Object.matWorld, &wtm);
	SAGlobal::DumpMatrix3(Object.InvmatWorld, &Invwtm);

	Object.m_Mesh.iMaterialID = iMaterialID;
	GetMesh(pNode, Object.m_Mesh, interval);

	GetAnimation(pNode, Object.m_AnimTrack, Scene, interval);
}
void SOAManager::GetMesh(INode* pNode, SOAMesh& sMesh, Interval& interval)
{
	m_TriLists.clear();

	// 로컬 좌표계이면 월드 행렬, 아니면 단위 행렬
	Matrix3 tm = pNode->GetObjTMAfterWSM(interval.Start());
	Matrix3 ParentTM = pNode->GetParentTM(interval.Start());
	Matrix3 LocalTM = pNode->GetNodeTM(interval.Start()) * Inverse(ParentTM);

	D3D_MATRIX Parent;
	D3D_MATRIX Local;

	SAGlobal::DumpMatrix3(Parent, &ParentTM);
	SAGlobal::DumpMatrix3(Local, &LocalTM);

	bool deleteit = false;
	// 트라이앵글 오브젝트
	TriObject* tri = GetTriObjectFromNode(pNode, interval.Start(), deleteit);
	if (tri == nullptr) return;
	// 메쉬 오브젝트
	Mesh* mesh = &tri->GetMesh();
	mesh->buildBoundingBox();
	sMesh.m_box = mesh->getBoundingBox(&tm);

	if (mesh == nullptr)
	{
		if (deleteit) delete tri;
		return;
	}
	bool negScale = SAGlobal::TMNegParity(tm);

	int v0, v1, v2;
	if (negScale) { v0 = 2; v1 = 1; v2 = 0; }
	else { v0 = 0; v1 = 1; v2 = 2; }

	int iSubMtlSize = pNode->GetMtl()->NumSubMtls();
	m_TriLists.resize(iSubMtlSize);
	sMesh.SubMeshList.resize(iSubMtlSize);
	std::vector<DWORD> SubMeshNumList;
	SubMeshNumList.resize(iSubMtlSize);
	ZeroMemory(SubMeshNumList.data(), sizeof(DWORD) * iSubMtlSize);

	int iNumFace = mesh->getNumFaces();
	//================================================================
	// 서브 메시의 할당의 수를 확실하게 판단하기 위한 반복문
	//================================================================
	for (int iFace = 0; iFace < iNumFace; iFace++)
	{
		int iSubmeshNum = mesh->faces[iFace].getMatID();
		if (iSubmeshNum >= iSubMtlSize ||
			iSubmeshNum < 0)
		{
			continue;
		}
		SubMeshNumList[iSubmeshNum]++;
	}
	sMesh.iSubNum = 0;
	for (int iSub = 0; iSub < iSubMtlSize; iSub++)
	{
		if (SubMeshNumList[iSub] <= 0) continue;

		sMesh.iSubNum++;
		m_TriLists[iSub].List.resize(SubMeshNumList[iSub]);
		sMesh.SubMeshList[iSub].VertexList.resize(SubMeshNumList[iSub] * 3);
		sMesh.SubMeshList[iSub].IndexList.resize(SubMeshNumList[iSub] * 3);
	}

	for (int iFace = 0; iFace < iNumFace; iFace++)
	{
		int iSubIndex = mesh->faces[iFace].getMatID();
		if (iSubIndex >= iSubMtlSize ||
			iSubIndex < 0)
		{
			continue;
		}

		SOATriangleList& TriangleList = m_TriLists[iSubIndex];
		int iFaceNum = TriangleList.iSize;
		TriangleList.iSize++;

		// position
		int iNumPos = mesh->getNumVerts();
		if (iNumPos > 0)
		{
			Point3 v = mesh->verts[mesh->faces[iFace].v[v0]] * tm;
			SAGlobal::DumpPoint3(TriangleList.List[iFaceNum].v[0].p, v);

			v = mesh->verts[mesh->faces[iFace].v[v2]] * tm;
			SAGlobal::DumpPoint3(TriangleList.List[iFaceNum].v[1].p, v);

			v = mesh->verts[mesh->faces[iFace].v[v1]] * tm;
			SAGlobal::DumpPoint3(TriangleList.List[iFaceNum].v[2].p, v);
		}
		// color
		int iNumColor = mesh->getNumVertCol();
		TriangleList.List[iFaceNum].v[0].c = Point4(1, 1, 1, 1);
		TriangleList.List[iFaceNum].v[1].c = Point4(1, 1, 1, 1);
		TriangleList.List[iFaceNum].v[2].c = Point4(1, 1, 1, 1);
		if (iNumColor > 0)
		{
			TriangleList.List[iFaceNum].v[0].c =
				mesh->vertCol[mesh->vcFace[iFace].t[v0]];
			TriangleList.List[iFaceNum].v[1].c =
				mesh->vertCol[mesh->vcFace[iFace].t[v2]];
			TriangleList.List[iFaceNum].v[2].c =
				mesh->vertCol[mesh->vcFace[iFace].t[v1]];
		}
		// texcoord
		int iNumTex = mesh->getNumTVerts();
		if (iNumTex > 0)
		{
			Point2 v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[v0]];
			TriangleList.List[iFaceNum].v[0].t.x = v.x;
			TriangleList.List[iFaceNum].v[0].t.y = 1.0f - v.y;

			v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[v2]];
			TriangleList.List[iFaceNum].v[1].t.x = v.x;
			TriangleList.List[iFaceNum].v[1].t.y = 1.0f - v.y;
			v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[v1]];
			TriangleList.List[iFaceNum].v[2].t.x = v.x;
			TriangleList.List[iFaceNum].v[2].t.y = 1.0f - v.y;
		}

		mesh->buildNormals();
		int vert = mesh->faces[iFace].getVert(v0);
		RVertex* rVertex = mesh->getRVertPtr(vert);
		Point3 vn = GetVertexNormal(mesh, iFace, rVertex);
		SAGlobal::DumpPoint3(TriangleList.List[iFaceNum].v[v0].n, vn);

		vert = mesh->faces[iFace].getVert(v2);
		rVertex = mesh->getRVertPtr(vert);
		vn = GetVertexNormal(mesh, iFace, rVertex);
		SAGlobal::DumpPoint3(TriangleList.List[iFaceNum].v[v1].n, vn);

		vert = mesh->faces[iFace].getVert(v1);
		rVertex = mesh->getRVertPtr(vert);
		vn = GetVertexNormal(mesh, iFace, rVertex);
		SAGlobal::DumpPoint3(TriangleList.List[iFaceNum].v[v2].n, vn);
	}

	// vb, ib
	SetUniqueBuffer(sMesh);

	if (deleteit) delete tri;
}
void SOAManager::SetUniqueBuffer(SOAMesh& sMesh)
{

	for (int iSub = 0; iSub < sMesh.SubMeshList.size(); iSub++)
	{
		if (m_TriLists[iSub].iSize <= 0) continue;

		std::vector<PNCT>& vList = sMesh.SubMeshList[iSub].VertexList;
		std::vector<DWORD>& iList = sMesh.SubMeshList[iSub].IndexList;
		int& iVertexSize = sMesh.SubMeshList[iSub].iVertexSize;
		int& iIndexSize = sMesh.SubMeshList[iSub].iIndexSize;

		for (int iFace = 0; iFace < m_TriLists[iSub].iSize; iFace++)
		{
			SOATriangleList& triArray = m_TriLists[iSub];
			SOATriangle& tri = triArray.List[iFace];

			for (int iVer = 0; iVer < 3; iVer++)
			{
				int iPos = IsEqulVerteList(tri.v[iVer], vList, iVertexSize);
				if (iPos < 0)
				{
					vList[iVertexSize] = tri.v[iVer];
					iVertexSize++;
					iPos = iVertexSize - 1;
				}
				iList[iIndexSize] = iPos;
				iIndexSize++;
			}
		}
	}
}
int	SOAManager::IsEqulVerteList(PNCT& vertex, std::vector<PNCT>& vList, int iVertexMax)
{
	for (int iVer = iVertexMax - 1; iVer >= 0; iVer--)
	{
		if (SAGlobal::EqualPoint3(vertex.p, vList[iVer].p) &&
			SAGlobal::EqualPoint3(vertex.n, vList[iVer].n) &&
			SAGlobal::EqualPoint4(vertex.c, vList[iVer].c) &&
			SAGlobal::EqualPoint2(vertex.t, vList[iVer].t))
		{
			return iVer;
		}
	}
	return -1;
}
Point3	SOAManager::GetVertexNormal(Mesh* mesh, int iFace, RVertex* rVertex)
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
TriObject* SOAManager::GetTriObjectFromNode(INode* pNode, TimeValue time, bool& deleteit)
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


void SOAManager::GetAnimation(INode* pNode, SAAnimationTrack& AnimTrack, SAScene& Scene, Interval& interval)
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

	SAPositionAnim	PosTrack;
	SARotationAnim	RotTrack;
	SAScaleAnim		ScaleTrack;

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
			if (!SAGlobal::EqualPoint3(StartAP.t, FrameAP.t))
			{
				AnimTrack.bPosition = true;
			}
		}

		if (!AnimTrack.bRotation) {
			if (!SAGlobal::EqualPoint3(Start_RotAxis, Frame_RotAxis))
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
			if (!SAGlobal::EqualPoint3(StartAP.k, FrameAP.k))
			{
				AnimTrack.bScale = true;
			}
		}
	}
}
bool SOAManager::ExportObject(FILE* pStream)
{
	if (pStream == nullptr)
	{
		return false;
	}
	_ftprintf(pStream, _T("\n%s"), L"#OBJECT_INFO");

	for (int iObj = 0; iObj < m_ObjectList.size(); iObj++)
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
bool SOAManager::ExportMesh(FILE* pStream, SOAMesh& sMesh)
{
	if (pStream == nullptr)
	{
		return false;
	}
	
	for (int iSubMesh = 0; iSubMesh < sMesh.SubMeshList.size(); iSubMesh++)
	{
		if (sMesh.SubMeshList[iSubMesh].iVertexSize <= 0) continue;
		std::vector<PNCT>& vList = sMesh.SubMeshList[iSubMesh].VertexList;
		std::vector<DWORD>& iList = sMesh.SubMeshList[iSubMesh].IndexList;
		int& iVertexSize = sMesh.SubMeshList[iSubMesh].iVertexSize;
		int& iIndexSize = sMesh.SubMeshList[iSubMesh].iIndexSize;

		_ftprintf(pStream, _T("\nSubMesh %d %d %d"),
			iSubMesh, iVertexSize, iIndexSize);

		for (int iVer = 0; iVer < iVertexSize; iVer++)
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

		for (int iIndex = 0; iIndex < iIndexSize; iIndex += 3)
		{
			_ftprintf(pStream, _T("\n%d %d %d"),
				iList[iIndex + 0],
				iList[iIndex + 1],
				iList[iIndex + 2]);
		}
	}
	return true;
}
bool SOAManager::ExportAnimation(FILE* pStream, SAAnimationTrack& AnimTrack)
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
void SOAManager::Release()
{
	m_ObjectList.clear();
	m_TriLists.clear();
}

SOAManager::SOAManager()
{
}


SOAManager::~SOAManager()
{
}
