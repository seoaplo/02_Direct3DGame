#include "SDrawObject.h"
SMesh* SDrawObject::CreateMesh()
{
	m_MeshList.push_back(SMesh());
	return &m_MeshList[m_MeshList.size() - 1];
}
bool SDrawObject::Init()
{
	m_fAnimationSpeed = 1.0f;
	m_fAnimElapseTime = 0.0f;
	return true;
}
bool SDrawObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pShaderFile)
{
	bool bMaterial = SetMaterial(m_iMaterialID);
	bool bParent = SetParent(m_ParentName);

	for (auto& TargetMesh : m_MeshList)
	{
		if (bMaterial && m_iClassType == CLASS_GEOM)
		{
			STextureList& TargetTexList = m_Material.m_SubMaterialList[TargetMesh.iSubMtrlIndex].m_TextureList;
			TargetMesh.Create(pDevice, pContext, &TargetTexList, pShaderFile);
		}
		else
		{
			TargetMesh.Create(pDevice, pContext, nullptr, nullptr);
		}
	}

	D3DXMATRIX matInvParent;
	if (bParent) matInvParent = m_pParentObject->m_matAnimInv;
	else D3DXMatrixIdentity(&matInvParent);

	m_matAnimSelf =	m_matAnim * matInvParent;
	D3DXMatrixInverse(
		&m_matAnimInv, NULL,
		&m_matAnim);

	D3DXMatrixDecompose(
		&m_vScaleTrack,
		&m_qRotTrack,
		&m_vPosTrack,
		&m_matAnimSelf);

	D3DXMatrixTranslation(&m_matPos,
		m_vPosTrack.x,
		m_vPosTrack.y,
		m_vPosTrack.z);
	D3DXMatrixScaling(&m_matScl,
		m_vScaleTrack.x,
		m_vScaleTrack.y,
		m_vScaleTrack.z);
	D3DXMatrixRotationQuaternion(
		&m_matRot,
		&m_qRotTrack);

	return true;
}
bool SDrawObject::Frame()
{
	m_fAnimElapseTime += m_fAnimationSpeed *  I_Timer.GetSPF() * m_Scene.iFrameSpeed * m_Scene.iTickPerFrame;

	float fEndTime = m_Scene.iLastFrame * m_Scene.iTickPerFrame;
	if (m_fAnimElapseTime >= fEndTime)
	{
		m_fAnimElapseTime = 0.0f;
	}
	D3DXMATRIX matParent;
	D3DXMatrixIdentity(&matParent);

	if (m_pParentObject != nullptr)
	{
		Interpolate(m_pParentObject->m_matCalculation, m_fAnimElapseTime);
	}
	else
	{
		Interpolate(matParent,	m_fAnimElapseTime);
	}
	return true;
}
bool SDrawObject::Render(ID3D11DeviceContext* pContext)
{
	D3DXMATRIX matWorld = m_matAnimInv * m_matCalculation * m_matWorld;
	for (auto& TargetMesh : m_MeshList)
	{
		TargetMesh.SetMatrix(&matWorld, &m_matView, &m_matProj);
		TargetMesh.Render(pContext);
	}
	return true;
}
bool SDrawObject::Release()
{
	return true;
}


void SDrawObject::Interpolate(D3DXMATRIX &matParent, float fElapseTime)
{
	D3DXMATRIX matAnimScale, matAnimRot, matAnimPos;
	matAnimScale = m_matScl;
	matAnimRot = m_matRot;
	matAnimPos = m_matPos;

	D3DXMatrixIdentity(&m_matCalculation);


	SAnimTrack StartTrack, EndTrack;

	ZeroMemory(&StartTrack, sizeof(StartTrack));
	ZeroMemory(&EndTrack, sizeof(EndTrack));
	D3DXQUATERNION qRotatin;
	D3DXQUATERNION qScale;
	D3DXQuaternionRotationMatrix(&qRotatin, &matAnimRot);
	D3DXQuaternionRotationMatrix(&qScale, &matAnimScale);

	if (m_RotAnimList.size())
	{
		if (GetAnimationTrack(
			fElapseTime,
			m_RotAnimList,
			StartTrack,
			EndTrack))
		{
			float t = (fElapseTime - StartTrack.iTick) / (EndTrack.iTick - StartTrack.iTick);
			D3DXQuaternionSlerp(&qRotatin,
				&StartTrack.Quaternion,
				&EndTrack.Quaternion, t);
		}
		D3DXMatrixRotationQuaternion(&matAnimRot, &qRotatin);
	}
	ZeroMemory(&StartTrack, sizeof(StartTrack));
	ZeroMemory(&EndTrack, sizeof(EndTrack));

	D3DXMATRIX matScaleRotation, matScaleRotInverse, matScaleVector;
	D3DXVECTOR3 vTrans(matAnimPos._41, matAnimPos._42, matAnimPos._43);
	if (m_ScaleAnimList.size())
	{
		if (GetAnimationTrack(
			fElapseTime,
			m_ScaleAnimList,
			StartTrack,
			EndTrack))
		{
			float t = (fElapseTime - StartTrack.iTick) / (EndTrack.iTick - StartTrack.iTick);
			D3DXQuaternionSlerp(&qRotatin,
				&StartTrack.Quaternion,
				&EndTrack.Quaternion, t);
			D3DXVec3Lerp(&vTrans,
				&StartTrack.Position,
				&EndTrack.Position, t);
		}
		D3DXMatrixScaling(&matScaleVector, vTrans.x, vTrans.y, vTrans.z);
		D3DXMatrixRotationQuaternion(&matScaleRotation, &qRotatin);
		D3DXMatrixInverse(&matScaleRotInverse, NULL, &matScaleRotation);
		matAnimScale = matScaleRotInverse * matScaleVector * matScaleRotation;
	}

	ZeroMemory(&StartTrack, sizeof(StartTrack));
	ZeroMemory(&EndTrack, sizeof(EndTrack));
	if (m_PosAnimList.size())
	{
		if (GetAnimationTrack(
			fElapseTime, m_PosAnimList, StartTrack, EndTrack))
		{
			float t = (fElapseTime - StartTrack.iTick) / (EndTrack.iTick - StartTrack.iTick);
			D3DXVec3Lerp(&vTrans,
				&StartTrack.Position,
				&EndTrack.Position, t);
		}
		D3DXMatrixTranslation(&matAnimPos, vTrans.x,
			vTrans.y, vTrans.z);
	}

	D3DXMATRIX matAnim;
	D3DXMatrixIdentity(&matAnim);
	matAnim = matAnimScale * matAnimRot;
	//matAnim = matAnimRot;
	matAnim._41 = matAnimPos._41;
	matAnim._42 = matAnimPos._42;
	matAnim._43 = matAnimPos._43;

	m_matCalculation = matAnim * matParent;
	return;
}


bool SDrawObject::GetAnimationTrack(
	float fElapseTime,
	std::vector<SAnimTrack>& trackList,
	SAnimTrack& StartTrack,
	SAnimTrack& EndTrack)
{
	for (auto& track : trackList)
	{
		if (track.iTick > fElapseTime)
		{
			EndTrack = track;
			break;
		}
		StartTrack = track;
	}
	return true;
}
bool SDrawObject::SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj)
{
	if (pWorld != NULL)
	{
		m_matWorld = *pWorld;
		m_vCenter.x = pWorld->_41;
		m_vCenter.y = pWorld->_42;
		m_vCenter.z = pWorld->_43;
	}
	if (pView != NULL)
	{
		m_matView = *pView;
	}
	if (pProj != NULL)
	{
		m_matProj = *pProj;
	}
	return true;
}

bool SDrawObject::SetParent(SDrawObject* pParentObject)
{
	m_pParentObject = pParentObject;
	if (m_pParentObject == nullptr) return false;
	else m_ParentName = m_pParentObject->m_ObjectName;
	return true;
}
bool SDrawObject::SetParent(int	iIndex)
{
	m_pParentObject = I_DrawObjectManager.GetDrawObject(iIndex);
	if (m_pParentObject == nullptr) return false;
	else m_ParentName = m_pParentObject->m_ObjectName;
	return true;
}
bool SDrawObject::SetParent(T_STR ParentName)
{
	m_pParentObject = I_DrawObjectManager.GetDrawObject(ParentName);
	if (m_pParentObject == nullptr) return false;
	else m_ParentName = ParentName;
	return true;
}

bool SDrawObject::SetMaterial(SMaterial* pMaterial)
{
	if (pMaterial == nullptr) return false;
	else if (pMaterial->m_SubMaterialList.size() <= 0) return false;
	else m_iMaterialID = m_Material.m_iIndex;
	m_Material = *pMaterial;

	for (auto& TargetMesh : m_MeshList)
	{
		if (m_iClassType == CLASS_GEOM)
		{
			if (m_Material.m_SubMaterialList.size() <= TargetMesh.iSubMtrlIndex) break;
			STextureList& TargetTexList = m_Material.m_SubMaterialList[TargetMesh.iSubMtrlIndex].m_TextureList;
			TargetMesh.m_TextureList = m_Material.m_SubMaterialList[TargetMesh.iSubMtrlIndex].m_TextureList;
		}
	}
	return true;
}
bool SDrawObject::SetMaterial(T_STR MaterialName)
{
	SMaterial* pMaterial = I_MaterialManager.GetMaterial(MaterialName);
	if (pMaterial == nullptr) return false;
	else if (pMaterial->m_SubMaterialList.size() <= 0) return false;

	m_iMaterialID = m_Material.m_iIndex;
	m_Material = *pMaterial;
	for (auto& TargetMesh : m_MeshList)
	{
		if (m_iClassType == CLASS_GEOM)
		{
			if (m_Material.m_SubMaterialList.size() <= TargetMesh.iSubMtrlIndex) break;
			STextureList& TargetTexList = m_Material.m_SubMaterialList[TargetMesh.iSubMtrlIndex].m_TextureList;
			TargetMesh.m_TextureList = m_Material.m_SubMaterialList[TargetMesh.iSubMtrlIndex].m_TextureList;
		}
	}
	return true;
}
bool SDrawObject::SetMaterial(int iMaterialNumber)
{
	m_iMaterialID = iMaterialNumber;
	SMaterial* pMaterial = I_MaterialManager.GetMaterial(iMaterialNumber);

	if (pMaterial == nullptr) return false;
	else if (pMaterial->m_SubMaterialList.size() <= 0) return false;

	m_Material = *pMaterial;
	for (auto& TargetMesh : m_MeshList)
	{
		if ( m_iClassType == CLASS_GEOM)
		{
			if (m_Material.m_SubMaterialList.size() <= TargetMesh.iSubMtrlIndex) break;

			STextureList& TargetTexList = m_Material.m_SubMaterialList[TargetMesh.iSubMtrlIndex].m_TextureList;
			TargetMesh.m_TextureList = m_Material.m_SubMaterialList[TargetMesh.iSubMtrlIndex].m_TextureList;
		}
	}
	return true;
}
SDrawObject::SDrawObject()
{
	m_iIndex = -1;
	m_iMaterialID = -1;
	m_ObjectName = L"none";
	m_ParentName = L"none";
	m_pParentObject = nullptr;

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
}


SDrawObject::~SDrawObject()
{
	m_MeshList.clear();
}

SDrawObject* SDrawObjectManager::Create()
{
	m_DrawObjectList.push_back(SDrawObject());
	UpdateParent();
	m_DrawObjectList[m_DrawObjectList.size() - 1].m_iIndex = m_DrawObjectList.size() - 1;
	return &m_DrawObjectList[m_DrawObjectList.size() - 1];
}

SDrawObject* SDrawObjectManager::GetDrawObject(T_STR DrawObjectName)
{
	for (auto& Target : m_DrawObjectList)
	{
		if (Target.m_ObjectName == DrawObjectName)
		{
			return &Target;
		}
	}
	return nullptr;
}
SDrawObject* SDrawObjectManager::GetDrawObject(int	iIndex)
{
	if (iIndex >= m_DrawObjectList.size()) return nullptr;
	else return &m_DrawObjectList[iIndex];
}
int	SDrawObjectManager::GetSize() 
{
	return m_DrawObjectList.size();
}

void SDrawObjectManager::UpdateParent()
{
	for (auto& TargetObj : m_DrawObjectList)
	{
		if (TargetObj.m_ParentName == L"none") continue;
		TargetObj.SetParent(GetDrawObject(TargetObj.m_ParentName));
	}
}