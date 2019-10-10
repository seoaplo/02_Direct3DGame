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
bool SDrawObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TCHAR* pShaderFile)
{
	bool bMaterial = SetMaterial(m_iMaterialID);
	bool bParent = SetParent(m_ParentName);

	for (auto& TargetMesh : m_MeshList)
	{
		if (bMaterial && m_iClassType == CLASS_GEOM)
		{
			STextureList& TargetTexList = m_pMaterial->m_SubMaterialList[TargetMesh.iSubMtrlIndex].m_TextureList;
			TargetMesh.Create(pDevice, pContext, &TargetTexList, pShaderFile);
		}
		else
		{
			TargetMesh.Create(pDevice, pContext, nullptr, nullptr);
		}
	}

	D3DXMATRIX matInvParent;
	if (bParent) matInvParent = m_pParentObject->m_matInvWorld;
	else D3DXMatrixIdentity(&matInvParent);

	m_matAnimSelf =	m_matWorld * matInvParent;
	D3DXMatrixInverse(
		&m_matInvWorld, NULL,
		&m_matWorld);

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

	D3DXMATRIX matinvParent;
	D3DXMatrixIdentity(&matinvParent);
	D3DXMatrixInverse(&matinvParent, nullptr, &matParent);

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
	m_pMaterial = pMaterial;
	if (m_pMaterial == nullptr) return false;
	else m_iMaterialID = m_pMaterial->m_iIndex;
	return true;
}
bool SDrawObject::SetMaterial(T_STR MaterialName)
{
	m_pMaterial = I_MaterialManager.GetMaterial(MaterialName);
	if (m_pMaterial == nullptr) return false;
	else m_iMaterialID = m_pMaterial->m_iIndex;
	return true;
}
bool SDrawObject::SetMaterial(int iMaterialNumber)
{
	m_iMaterialID = iMaterialNumber;
	m_pMaterial = I_MaterialManager.GetMaterial(iMaterialNumber);
	if (m_pMaterial == nullptr) return false;
	return true;
}
SDrawObject::SDrawObject()
{
	m_iIndex = -1;
	m_iMaterialID = -1;
	m_ObjectName = L"none";
	m_ParentName = L"none";
	m_pMaterial = nullptr;
	m_pParentObject = nullptr;
}


SDrawObject::~SDrawObject()
{
	m_MeshList.clear();
}

SDrawObject* SDrawObjectManager::Create()
{
	m_DrawObjectList.push_back(SDrawObject());
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