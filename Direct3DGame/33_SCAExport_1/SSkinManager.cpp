#include "SSkinManager.h"

struct BipAscendingSort
{
	bool operator()(SCABiped& rpStart, SCABiped& rpEnd)
	{
		return rpStart.m_fWeight < rpEnd.m_fWeight;
	}
};

void SSkinManager::AddObject(INode* pNode, SAScene& Scene, Interval& interval, int iObjectNum, int iMaterialID, NodeList& MatrixNodeList)
{
	ObjectState os = pNode->EvalWorldState(interval.Start());
	Object* CheckID = nullptr;
	int ChildNum = pNode->NumChildren();
	int iObjectNumber = -1;

	SCAObject Object;
	iObjectNumber = iObjectNum;
	CheckID = pNode->GetObjectRef();
	Control* pControl = pNode->GetTMController();

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
			if (pControl && pControl->ClassID()
				== BIPBODY_CONTROL_CLASS_ID)
			{
				Object.m_ClassType = CLASS_BIPED;
			}
			if (pControl && pControl->ClassID()
				== BIPSLAVE_CONTROL_CLASS_ID)
			{
				Object.m_ClassType = CLASS_BIPED;
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
	Object.pINode = pNode;
	Matrix3 wtm = pNode->GetNodeTM(interval.Start());
	Matrix3 Invwtm = Inverse(wtm);
	SAGlobal::DumpMatrix3(Object.matWorld, &wtm);
	SAGlobal::DumpMatrix3(Object.matInvWorld, &Invwtm);

	Object.m_Mesh.iMaterialID = iMaterialID;
	if (iMaterialID < 0) return;
	GetMesh(pNode, Object.m_Mesh, interval, MatrixNodeList);

	m_ObjectList.push_back(Object);
}
void SSkinManager::GetMesh(INode* pNode, SCAMesh& sMesh, Interval& interval, NodeList& MatrixNodeList)
{
	m_TriLists.clear();
	m_bipedList.clear();
	// 로컬 좌표계이면 월드 행렬, 아니면 단위 행렬
	Matrix3 tm = pNode->GetObjTMAfterWSM(interval.Start());

	bool deleteit = false;
	// 트라이앵글 오브젝트
	TriObject* tri = GetTriObjectFromNode(pNode, interval.Start(), deleteit);
	if (tri == nullptr)
	{
		Object* helperObj = pNode->EvalWorldState(interval.Start()).obj;
		helperObj->GetDeformBBox(0,
			sMesh.m_box,
			&pNode->GetObjectTM(0));
		return;
	}
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

	SetBippedInfo(pNode, sMesh, MatrixNodeList);

	int iSubMtlSize = pNode->GetMtl()->NumSubMtls();
	if (iSubMtlSize <= 0) iSubMtlSize = 1;
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

		int iV0 = mesh->faces[iFace].v[v0];
		int iV1 = mesh->faces[iFace].v[v1];
		int iV2 = mesh->faces[iFace].v[v2];

		SCATriangleList& TriangleList = m_TriLists[iSubIndex];
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

		//biped index & weight save
		if (m_bipedList.size() <= 0) continue;
		if (m_bipedList[iV0].m_BipedList.size() > 0)
		{
			for (int iWeight = 0;
				iWeight <
				m_bipedList[iV0].m_dwNumWeight;
				iWeight++)
			{
				TriangleList.List[iFaceNum].v[0].i[iWeight] =
					m_bipedList[iV0].m_BipedList[iWeight].BipID;
				TriangleList.List[iFaceNum].v[0].w[iWeight] =
					m_bipedList[iV0].m_BipedList[iWeight].m_fWeight;
			}
		}
		if (m_bipedList[iV2].m_BipedList.size() > 0)
		{
			for (int iWeight = 0;
				iWeight <
				m_bipedList[iV2].m_dwNumWeight;
				iWeight++)
			{
				TriangleList.List[iFaceNum].v[1].i[iWeight] =
					m_bipedList[iV2].m_BipedList[iWeight].BipID;
				TriangleList.List[iFaceNum].v[1].w[iWeight] =
					m_bipedList[iV2].m_BipedList[iWeight].m_fWeight;
			}
		}
		if (m_bipedList[iV1].m_BipedList.size() > 0)
		{
			for (int iWeight = 0;
				iWeight <
				m_bipedList[iV1].m_dwNumWeight;
				iWeight++)
			{
				TriangleList.List[iFaceNum].v[2].i[iWeight] =
					m_bipedList[iV1].m_BipedList[iWeight].BipID;
				TriangleList.List[iFaceNum].v[2].w[iWeight] =
					m_bipedList[iV1].m_BipedList[iWeight].m_fWeight;
			}
		}
	}

	// vb, ib
	SetUniqueBuffer(sMesh);

	if (deleteit) delete tri;
}
void SSkinManager::SetUniqueBuffer(SCAMesh& sMesh)
{

	for (int iSub = 0; iSub < sMesh.SubMeshList.size(); iSub++)
	{
		if (m_TriLists[iSub].iSize <= 0) continue;

		std::vector<PNCTIW_VERTEX>& vList = sMesh.SubMeshList[iSub].VertexList;
		std::vector<DWORD>& iList = sMesh.SubMeshList[iSub].IndexList;
		int& iVertexSize = sMesh.SubMeshList[iSub].iVertexSize;
		int& iIndexSize = sMesh.SubMeshList[iSub].iIndexSize;

		for (int iFace = 0; iFace < m_TriLists[iSub].iSize; iFace++)
		{
			SCATriangleList& triArray = m_TriLists[iSub];
			SCATriangle& tri = triArray.List[iFace];

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
int	SSkinManager::IsEqulVerteList(PNCTIW_VERTEX& vertex, std::vector<PNCTIW_VERTEX>& vList, int iVertexMax)
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
Point3	SSkinManager::GetVertexNormal(Mesh* mesh, int iFace, RVertex* rVertex)
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
TriObject* SSkinManager::GetTriObjectFromNode(INode* pNode, TimeValue time, bool& deleteit)
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

void SSkinManager::SetBippedInfo(INode* pNode, SCAMesh& sMesh, NodeList& MatrixNodeList)
{
	// 에니메이션 제작 도구
	Modifier* Pointer_Physique = FindModifier(pNode,
		Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B));
	if (Pointer_Physique)
	{
		PhysiqueData(pNode, Pointer_Physique, sMesh, MatrixNodeList);
		return;
	}
	Modifier* skinMod = FindModifier(pNode,
		SKIN_CLASSID);
	if (skinMod)
	{
		SkinData(pNode, skinMod, sMesh, MatrixNodeList);
		return;
	}
}
Modifier*  SSkinManager::FindModifier(
	INode* pNode, Class_ID classID)

{
	Object *ObjectPtr = pNode->GetObjectRef();
	if (!ObjectPtr)
	{
		return nullptr;
	}
	while (ObjectPtr->SuperClassID() == GEN_DERIVOB_CLASS_ID && ObjectPtr)
	{
		IDerivedObject *DerivedObjectPtr = (IDerivedObject *)(ObjectPtr);

		int ModStackIndex = 0;
		while (ModStackIndex < DerivedObjectPtr->NumModifiers())
		{
			Modifier* ModifierPtr = DerivedObjectPtr->GetModifier(ModStackIndex);

			if (ModifierPtr->ClassID() == classID)
			{
				return ModifierPtr;
			}

			ModStackIndex++;
		}
		ObjectPtr = DerivedObjectPtr->GetObjRef();
	}
	return nullptr;
}
void SSkinManager::PhysiqueData(INode* pNode, Modifier* Pointer_Physique, SCAMesh& sMesh, NodeList& MatrixNodeList)
{
	IPhysiqueExport *phyExport =
		(IPhysiqueExport*)Pointer_Physique->GetInterface(I_PHYINTERFACE);
	IPhyContextExport *contextExport =
		(IPhyContextExport *)phyExport->GetContextInterface(pNode);

	contextExport->ConvertToRigid(true);
	contextExport->AllowBlending(true);

	// mesh 정점개수와 동일해야 함.
	int iNumVert = contextExport->GetNumberVertices();
	m_bipedList.resize(iNumVert);

	for (int iVertex = 0; iVertex < iNumVert; iVertex++)
	{
		IPhyVertexExport *vtxExport =
			(IPhyVertexExport *)contextExport->GetVertexInterface(iVertex);
		if (vtxExport)
		{
			int iVertexType = vtxExport->GetVertexType();
			switch (iVertexType)
			{
			case RIGID_NON_BLENDED_TYPE:
			{
				IPhyRigidVertex* pVertex =
					(IPhyRigidVertex*)vtxExport;
				INode* node = pVertex->GetNode();

				NodeList::iterator itor = MatrixNodeList.find(node);

				if (itor != MatrixNodeList.end())
				{
					SCABiped Biped;
					Biped.BipID = itor->second;
					Biped.m_fWeight = 1.0f;
					m_bipedList[iVertex].m_BipedList.push_back(Biped);
					m_bipedList[iVertex].m_dwNumWeight = 1;
				}
				else
				{
					return;
				}

			}break;
			case RIGID_BLENDED_TYPE:
			{
				IPhyBlendedRigidVertex* pVertex =
					(IPhyBlendedRigidVertex*)vtxExport;
				for (int iWeight = 0;
					iWeight < pVertex->GetNumberNodes();
					iWeight++)
				{
					INode* node = pVertex->GetNode(iWeight);

					NodeList::iterator itor = MatrixNodeList.find(node);
					if (itor != MatrixNodeList.end())
					{
						SCABiped Biped;
						Biped.BipID = itor->second;
						Biped.m_fWeight = pVertex->GetWeight(iWeight);

						//if (ALMOST_ZERO > Biped.m_fWeight) continue;
						//else m_bipedList[iVertex].m_BipedList.push_back(Biped);
						m_bipedList[iVertex].m_BipedList.push_back(Biped);
					}
				}
				/*std::sort(m_bipedList[iVertex].m_BipedList.begin(),
					m_bipedList[iVertex].m_BipedList.end(),
					BipAscendingSort());*/
				if (m_bipedList[iVertex].m_BipedList.size() > MAX_WEIGHT_BIPED)
				{
					m_bipedList[iVertex].m_BipedList.resize(MAX_WEIGHT_BIPED);
				}
				m_bipedList[iVertex].m_dwNumWeight =
					m_bipedList[iVertex].m_BipedList.size();

			}break;
			default:break;
			}
		}
		contextExport->ReleaseVertexInterface(vtxExport);
	}
	phyExport->ReleaseContextInterface(contextExport);
	Pointer_Physique->ReleaseInterface(I_PHYINTERFACE, phyExport);
}
void SSkinManager::SkinData(INode* pNode, Modifier* Pointer_Skin, SCAMesh& sMesh, NodeList& MatrixNodeList)
{
	ISkin* skin = (ISkin*)Pointer_Skin->GetInterface(I_SKIN);
	ISkinContextData* skinData = skin->GetContextInterface(pNode);

	if (!skin || !skinData) return;

	int iNumVertex = skinData->GetNumPoints();
	m_bipedList.resize(iNumVertex);

	for (int iVertex = 0; iVertex < iNumVertex; iVertex++)
	{
		m_bipedList[iVertex].m_dwNumWeight =
			skinData->GetNumAssignedBones(iVertex);
		for (int iWeight = 0; iWeight < m_bipedList[iVertex].m_dwNumWeight; iWeight++)
		{
			int iBoneID = skinData->GetAssignedBone(iVertex, iWeight);
			INode* node = skin->GetBone(iBoneID);
			NodeList::iterator itor = MatrixNodeList.find(node);
			SCABiped Biped;
			if (itor != MatrixNodeList.end())
			{
				Biped.BipID = itor->second;
				Biped.m_fWeight = skinData->GetBoneWeight(iVertex, iWeight);
			}

			//if (ALMOST_ZERO > Biped.m_fWeight) continue;
			//else m_bipedList[iVertex].m_BipedList.push_back(Biped);
			m_bipedList[iVertex].m_BipedList.push_back(Biped);
		}
		/*std::sort(m_bipedList[iVertex].m_BipedList.begin(),
			m_bipedList[iVertex].m_BipedList.end(),
			BipAscendingSort());*/
	}
}
bool SSkinManager::ExportObject(FILE* pStream)
{
	if (pStream == nullptr)
	{
		return false;
	}
	_ftprintf(pStream, _T("\n%s"), L"#OBJECT_INFO");

	for (int iObj = 0; iObj < m_ObjectList.size(); iObj++)
	{

		_ftprintf(pStream, _T("\n%s %s %d %d"),
			m_ObjectList[iObj].name,
			m_ObjectList[iObj].ParentName,
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
	}
	return true;
}
bool SSkinManager::ExportMesh(FILE* pStream, SCAMesh& sMesh)
{
	if (pStream == nullptr)
	{
		return false;
	}

	for (int iSubMesh = 0; iSubMesh < sMesh.SubMeshList.size(); iSubMesh++)
	{
		if (sMesh.SubMeshList[iSubMesh].iVertexSize <= 0) continue;
		std::vector<PNCTIW_VERTEX>& vList = sMesh.SubMeshList[iSubMesh].VertexList;
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

			_ftprintf(pStream, _T("%10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f"),
				vList[iVer].i[0],
				vList[iVer].i[1],
				vList[iVer].i[2],
				vList[iVer].i[3],
				vList[iVer].i[4],
				vList[iVer].i[5],
				vList[iVer].i[6],
				vList[iVer].i[7]);

			_ftprintf(pStream, _T("%10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f"),
				vList[iVer].w[0],
				vList[iVer].w[1],
				vList[iVer].w[2],
				vList[iVer].w[3],
				vList[iVer].w[4],
				vList[iVer].w[5],
				vList[iVer].w[6],
				vList[iVer].w[7]);
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
void SSkinManager::Release()
{
	m_ObjectList.clear();
	m_TriLists.clear();
}

SSkinManager::SSkinManager()
{
}


SSkinManager::~SSkinManager()
{
}
