#include "SCAObjectManager.h"

struct BipAscendingSort
{
	bool operator()(SCABiped& rpStart, SCABiped& rpEnd)
	{
		return rpStart.m_fWeight < rpEnd.m_fWeight;
	}
};

void SCAObjectManager::AddObject(INode* pNode, SCAScene& Scene, Interval& interval, int iMaterialID)
{
	ObjectState os = pNode->EvalWorldState(interval.Start());
	Object* CheckID = nullptr;
	int ChildNum = pNode->NumChildren();
	int iObjectNumber = -1;

	SCAObject Object;
	iObjectNumber = m_CharacterList.size();
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
		Object.ParentName = SCAGlobal::FixupName(pParentNode->GetName());
	}
	Object.name = SCAGlobal::FixupName(pNode->GetName());
	Object.pINode = pNode;
	Matrix3 wtm = pNode->GetNodeTM(interval.Start());
	Matrix3 Invwtm = Inverse(wtm);
	SCAGlobal::DumpMatrix3(Object.matWorld, &wtm);
	SCAGlobal::DumpMatrix3(Object.matInvWorld, &Invwtm);

	Object.m_Mesh.iMaterialID = iMaterialID;
	GetMesh(pNode, Object.m_Mesh, interval);

	Object.iIndex = m_CharacterList.size();
	m_CharacterList.push_back(Object);
}
void SCAObjectManager::GetMesh(INode* pNode, SCAMesh& sMesh, Interval& interval)
{
	for (int iCount = 0; iCount < SUBMATERIAL_SIZE; iCount++)
	{
		m_SCATriList[iCount].clear();
	}

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
	bool negScale = SCAGlobal::TMNegParity(tm);

	mesh->buildBoundingBox();
	sMesh.m_box = mesh->getBoundingBox(&tm);

	int v0, v1, v2;
	if (negScale) { v0 = 2; v1 = 1; v2 = 0; }
	else { v0 = 0; v1 = 1; v2 = 2; }
	
	SetBippedInfo(pNode, sMesh);

	if (mesh)
	{
		int iNumFace = mesh->getNumFaces();

		for (int iFace = 0; iFace < iNumFace; iFace++)
		{
			int iV0 = mesh->faces[iFace].v[v0];
			int iV1 = mesh->faces[iFace].v[v2];
			int iV2 = mesh->faces[iFace].v[v1];

			int iSubIndex = mesh->faces[iFace].getMatID();
			if (iSubIndex > SUBMATERIAL_SIZE ||
				iSubIndex < 0)
			{
				continue;
			}
			m_SCATriList[iSubIndex].push_back(SCATriangle());
			int iFaceNum = m_SCATriList[iSubIndex].size() - 1;

			// position
			int iNumPos = mesh->getNumVerts();
			if (iNumPos > 0)
			{
				Point3 v = mesh->verts[iV0] * tm;
				SCAGlobal::DumpPoint3(m_SCATriList[iSubIndex][iFaceNum].v[0].p, v);

				v = mesh->verts[iV1] * tm;
				SCAGlobal::DumpPoint3(m_SCATriList[iSubIndex][iFaceNum].v[1].p, v);

				v = mesh->verts[mesh->faces[iFace].v[v2]] * tm;
				SCAGlobal::DumpPoint3(m_SCATriList[iSubIndex][iFaceNum].v[2].p, v);
			}
			// color
			int iNumColor = mesh->getNumVertCol();
			m_SCATriList[iSubIndex][iFaceNum].v[0].c = Point4(1, 1, 1, 1);
			m_SCATriList[iSubIndex][iFaceNum].v[1].c = Point4(1, 1, 1, 1);
			m_SCATriList[iSubIndex][iFaceNum].v[2].c = Point4(1, 1, 1, 1);
			if (iNumColor > 0)
			{
				m_SCATriList[iSubIndex][iFaceNum].v[0].c =
					mesh->vertCol[mesh->vcFace[iFace].t[v0]];
				m_SCATriList[iSubIndex][iFaceNum].v[1].c =
					mesh->vertCol[mesh->vcFace[iFace].t[v2]];
				m_SCATriList[iSubIndex][iFaceNum].v[2].c =
					mesh->vertCol[mesh->vcFace[iFace].t[v1]];
			}
			// texcoord
			int iNumTex = mesh->getNumTVerts();
			if (iNumTex > 0)
			{
				Point2 v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[v0]];
				m_SCATriList[iSubIndex][iFaceNum].v[0].t.x = v.x;
				m_SCATriList[iSubIndex][iFaceNum].v[0].t.y = 1.0f - v.y;

				v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[v2]];
				m_SCATriList[iSubIndex][iFaceNum].v[1].t.x = v.x;
				m_SCATriList[iSubIndex][iFaceNum].v[1].t.y = 1.0f - v.y;
				v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[v1]];
				m_SCATriList[iSubIndex][iFaceNum].v[2].t.x = v.x;
				m_SCATriList[iSubIndex][iFaceNum].v[2].t.y = 1.0f - v.y;
			}

			mesh->buildNormals();
			int vert = mesh->faces[iFace].getVert(v0);
			RVertex* rVertex = mesh->getRVertPtr(vert);
			Point3 vn = GetVertexNormal(mesh, iFace, rVertex);
			SCAGlobal::DumpPoint3(m_SCATriList[iSubIndex][iFaceNum].v[v0].n, vn);

			vert = mesh->faces[iFace].getVert(v2);
			rVertex = mesh->getRVertPtr(vert);
			vn = GetVertexNormal(mesh, iFace, rVertex);
			SCAGlobal::DumpPoint3(m_SCATriList[iSubIndex][iFaceNum].v[v1].n, vn);

			vert = mesh->faces[iFace].getVert(v1);
			rVertex = mesh->getRVertPtr(vert);
			vn = GetVertexNormal(mesh, iFace, rVertex);
			SCAGlobal::DumpPoint3(m_SCATriList[iSubIndex][iFaceNum].v[v2].n, vn);

			//biped index & weight save
			if (m_bipedList.size() <= 0) continue;
			if (m_bipedList[iV0].m_BipedList.size() > 0)
			{
				for (int iWeight = 0;
					iWeight <
					m_bipedList[iV0].m_dwNumWeight;
					iWeight++)
				{
					m_SCATriList[iSubIndex][iFaceNum].v[v0].i[iWeight] =
						m_bipedList[iV0].m_BipedList[iWeight].BipID;
					m_SCATriList[iSubIndex][iFaceNum].v[v0].w[iWeight] =
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
					m_SCATriList[iSubIndex][iFaceNum].v[v1].i[iWeight] =
						m_bipedList[iV2].m_BipedList[iWeight].BipID;
					m_SCATriList[iSubIndex][iFaceNum].v[v1].w[iWeight] =
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
					m_SCATriList[iSubIndex][iFaceNum].v[v2].i[iWeight] =
						m_bipedList[iV1].m_BipedList[iWeight].BipID;
					m_SCATriList[iSubIndex][iFaceNum].v[v2].w[iWeight] =
						m_bipedList[iV1].m_BipedList[iWeight].m_fWeight;
				}
			}
		}
		// vb, ib
		SetUniqueBuffer(sMesh);
	}

	if (deleteit) delete tri;
}
void SCAObjectManager::SetUniqueBuffer(SCAMesh& sMesh)
{

	for (int iSub = 0; iSub < SUBMATERIAL_SIZE; iSub++)
	{
		if (m_SCATriList[iSub].size() <= 0) continue;

		sMesh.iSubNum++;
		sMesh.SubMeshList[iSub].bUse = true;

		for (int iFace = 0; iFace < m_SCATriList[iSub].size(); iFace++)
		{
			std::vector<SCATriangle>& triArray = m_SCATriList[iSub];
			SCATriangle& tri = triArray[iFace];
			std::vector<PNCTIW_VERTEX>& vList = sMesh.SubMeshList[iSub].VertexList;
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
int	 SCAObjectManager::IsEqulVerteList(PNCTIW_VERTEX& vertex, std::vector<PNCTIW_VERTEX>& vList)
{
	for (int iVer = 0; iVer < vList.size(); iVer++)
	{
		if (SCAGlobal::EqualPoint3(vertex.p, vList[iVer].p) &&
			SCAGlobal::EqualPoint3(vertex.n, vList[iVer].n) &&
			SCAGlobal::EqualPoint4(vertex.c, vList[iVer].c) &&
			SCAGlobal::EqualPoint2(vertex.t, vList[iVer].t))
		{
			return iVer;
		}
	}
	return -1;
}

void SCAObjectManager::SetBippedInfo(INode* pNode, SCAMesh& sMesh)
{
	// 에니메이션 제작 도구
	Modifier* Pointer_Physique = FindModifier(pNode,
		Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B));
	if (Pointer_Physique)
	{
		PhysiqueData(pNode, Pointer_Physique, sMesh);
		return;
	}
	Modifier* skinMod = FindModifier(pNode,
		SKIN_CLASSID);
	if (skinMod)
	{
		SkinData(pNode, skinMod, sMesh);
		return;
	}
}
Modifier*  SCAObjectManager::FindModifier(
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
void SCAObjectManager::PhysiqueData(INode* pNode, Modifier* Pointer_Physique, SCAMesh& sMesh)
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
				SOAObject* pTargetObject = I_Writer.FindObject(node);
				if (pTargetObject != nullptr)
				{
					SCABiped Biped;
					Biped.BipID = pTargetObject->iIndex;
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
					SOAObject* pTargetObject = I_Writer.FindObject(node);
					if (pTargetObject != nullptr)
					{
						SCABiped Biped;
						Biped.BipID = pTargetObject->iIndex;
						Biped.m_fWeight = pVertex->GetWeight(iWeight);

						if (ALMOST_ZERO > Biped.m_fWeight) continue;
						else m_bipedList[iVertex].m_BipedList.push_back(Biped);
					}
					else
					{
						break;
					}
				}
				std::sort(m_bipedList[iVertex].m_BipedList.begin(),
							m_bipedList[iVertex].m_BipedList.end(),
							BipAscendingSort());
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
void SCAObjectManager::SkinData(INode* pNode, Modifier* Pointer_Skin, SCAMesh& sMesh)
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
			SOAObject* pTargetObject = I_Writer.FindObject(node);

			SCABiped Biped;
			Biped.BipID = pTargetObject->iIndex;
			Biped.m_fWeight = skinData->GetBoneWeight(iVertex, iWeight);

			if (ALMOST_ZERO > Biped.m_fWeight) continue;
			else m_bipedList[iVertex].m_BipedList.push_back(Biped);
		}
		std::sort(m_bipedList[iVertex].m_BipedList.begin(),
		m_bipedList[iVertex].m_BipedList.end(),
		BipAscendingSort());
	}
}

bool SCAObjectManager::ExportObject(FILE* pStream)
{
	if (pStream == nullptr)
	{
		return false;
	}
	_ftprintf(pStream, _T("\n%s"), L"#OBJECT_INFO");

	for (int iObj = 0; iObj < m_CharacterList.size(); iObj++)
	{

		_ftprintf(pStream, _T("\n%s %s %d %d %d"),
			m_CharacterList[iObj].name,
			m_CharacterList[iObj].ParentName,
			m_CharacterList[iObj].m_ClassType,
			m_CharacterList[iObj].m_Mesh.iMaterialID,
			m_CharacterList[iObj].m_Mesh.iSubNum);

		// World Matrix
		_ftprintf(pStream, _T("\nWolrdMatrix"));
		_ftprintf(pStream, _T("\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f"),
			m_CharacterList[iObj].matWorld._11,
			m_CharacterList[iObj].matWorld._12,
			m_CharacterList[iObj].matWorld._13,
			m_CharacterList[iObj].matWorld._14,

			m_CharacterList[iObj].matWorld._21,
			m_CharacterList[iObj].matWorld._22,
			m_CharacterList[iObj].matWorld._23,
			m_CharacterList[iObj].matWorld._24,

			m_CharacterList[iObj].matWorld._31,
			m_CharacterList[iObj].matWorld._32,
			m_CharacterList[iObj].matWorld._33,
			m_CharacterList[iObj].matWorld._34,

			m_CharacterList[iObj].matWorld._41,
			m_CharacterList[iObj].matWorld._42,
			m_CharacterList[iObj].matWorld._43,
			m_CharacterList[iObj].matWorld._44);

		// World Invese Matrix
		_ftprintf(pStream, _T("\nWolrdInverseMatrix"));
		_ftprintf(pStream, _T("\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f"),
			m_CharacterList[iObj].matInvWorld._11,
			m_CharacterList[iObj].matInvWorld._12,
			m_CharacterList[iObj].matInvWorld._13,
			m_CharacterList[iObj].matInvWorld._14,

			m_CharacterList[iObj].matInvWorld._21,
			m_CharacterList[iObj].matInvWorld._22,
			m_CharacterList[iObj].matInvWorld._23,
			m_CharacterList[iObj].matInvWorld._24,

			m_CharacterList[iObj].matInvWorld._31,
			m_CharacterList[iObj].matInvWorld._32,
			m_CharacterList[iObj].matInvWorld._33,
			m_CharacterList[iObj].matInvWorld._34,

			m_CharacterList[iObj].matInvWorld._41,
			m_CharacterList[iObj].matInvWorld._42,
			m_CharacterList[iObj].matInvWorld._43,
			m_CharacterList[iObj].matInvWorld._44);

		ExportMesh(pStream, m_CharacterList[iObj].m_Mesh);
		//ExportAnimation(pStream, m_CharacterList[iObj].m_AnimTrack);
	}

	return true;
}
bool SCAObjectManager::ExportMesh(FILE* pStream, SCAMesh& sMesh)
{
	if (pStream == nullptr)
	{
		return false;
	}
	for (int iSubMesh = 0; iSubMesh < SUBMATERIAL_SIZE; iSubMesh++)
	{
		if (sMesh.SubMeshList[iSubMesh].bUse == false) continue;
		std::vector<PNCTIW_VERTEX>& vList = sMesh.SubMeshList[iSubMesh].VertexList;
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

		for (int iIndex = 0; iIndex < iList.size(); iIndex += 3)
		{
			_ftprintf(pStream, _T("\n%d %d %d"),
				iList[iIndex + 0],
				iList[iIndex + 1],
				iList[iIndex + 2]);
		}
	}
}
bool SCAObjectManager::ExportAnimation(FILE* pStream, SCAAnimationTrack& AnimTrack)
{
	return true;
}

SCAObjectManager::SCAObjectManager()
{

}


SCAObjectManager::~SCAObjectManager()
{

}
