#include "SSkinObject.h"

SSkinMesh* SSkinObject::CreateMesh()
{
	m_MeshList.push_back(SSkinMesh());
	return &m_MeshList[m_MeshList.size() - 1];
}
bool SSkinObject::Init()
{
	return true;
}
bool SSkinObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pShaderFile)
{
	bool bMaterial = SetMaterial(m_iMaterialID);
	for (auto& TargetMesh : m_MeshList)
	{
		if (bMaterial)
		{
			STextureList& TargetTexList = m_Material.m_SubMaterialList[TargetMesh.iSubMtrlIndex].m_TextureList;
			TargetMesh.Create(pDevice, pContext, &TargetTexList, pShaderFile);
		}
		else
		{
			TargetMesh.Create(pDevice, pContext, nullptr, pShaderFile);
		}
	}

	return true;
}
bool SSkinObject::Frame()
{
	return true;
}
bool SSkinObject::Render(ID3D11DeviceContext* pContext)
{
	for (auto& TargetMesh : m_MeshList)
	{
		TargetMesh.SetMatrix(&m_matWorld, &m_matView, &m_matProj);
		TargetMesh.Render(pContext);
	}
	return true;
}
bool SSkinObject::Release()
{
	return true;
}

bool SSkinObject::SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj)
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

bool SSkinObject::SetMaterial(SMaterial* pMaterial)
{
	if (pMaterial == nullptr) return false;
	else if (m_Material.m_SubMaterialList.size() <= 0) return false;
	else m_iMaterialID = m_Material.m_iIndex;
	m_Material = *pMaterial;

	for (auto& TargetMesh : m_MeshList)
	{
		if (pMaterial->m_SubMaterialList.size() <= TargetMesh.iSubMtrlIndex) break;
		STextureList& TargetTexList = m_Material.m_SubMaterialList[TargetMesh.iSubMtrlIndex].m_TextureList;
		TargetMesh.m_TextureList = m_Material.m_SubMaterialList[TargetMesh.iSubMtrlIndex].m_TextureList;
	}
	return true;
}
bool SSkinObject::SetMaterial(T_STR MaterialName)
{
	SMaterial* pMaterial = I_MaterialManager.GetMaterial(MaterialName);
	if (pMaterial == nullptr) return false;
	else if (pMaterial->m_SubMaterialList.size() <= 0) return false;

	m_iMaterialID = m_Material.m_iIndex;
	m_Material = *pMaterial;
	for (auto& TargetMesh : m_MeshList)
	{
		if (m_Material.m_SubMaterialList.size() <= TargetMesh.iSubMtrlIndex) break;
		STextureList& TargetTexList = m_Material.m_SubMaterialList[TargetMesh.iSubMtrlIndex].m_TextureList;
		TargetMesh.m_TextureList = m_Material.m_SubMaterialList[TargetMesh.iSubMtrlIndex].m_TextureList;
	}
	return true;
}
bool SSkinObject::SetMaterial(int iMaterialNumber)
{
	m_iMaterialID = iMaterialNumber;

	SMaterial* pMaterial = I_MaterialManager.GetMaterial(iMaterialNumber);

	if (pMaterial == nullptr) return false;
	else if (pMaterial->m_SubMaterialList.size() <= 0) return false;
	m_Material = *pMaterial;

	for (auto& TargetMesh : m_MeshList)
	{
		if (m_Material.m_SubMaterialList.size() <= TargetMesh.iSubMtrlIndex) break;

		STextureList& TargetTexList = m_Material.m_SubMaterialList[TargetMesh.iSubMtrlIndex].m_TextureList;
		TargetMesh.m_TextureList = m_Material.m_SubMaterialList[TargetMesh.iSubMtrlIndex].m_TextureList;
	}
	return true;
}

SSkinObject::SSkinObject()
{
	m_iIndex = -1;
	m_iMaterialID = -1;
	m_ObjectName = L"none";
	m_pConstMatrixBuffer = nullptr;

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);

	for (int iCount = 0; iCount < 255; iCount++)
	{
		D3DXMatrixIdentity(&m_InvMatrixList[iCount]);
	}

}


SSkinObject::~SSkinObject()
{
	m_MeshList.clear();
}

SSkinObject* SSkinObjectManager::Create()
{
	m_SkinObjectList.push_back(SSkinObject());
	m_SkinObjectList[m_SkinObjectList.size() - 1].m_iIndex = m_SkinObjectList.size() - 1;
	return &m_SkinObjectList[m_SkinObjectList.size() - 1];
}

SSkinObject* SSkinObjectManager::GetSkinObject(T_STR DrawObjectName)
{
	for (auto& Target : m_SkinObjectList)
	{
		if (Target.m_ObjectName == DrawObjectName)
		{
			return &Target;
		}
	}
	return nullptr;
}
SSkinObject* SSkinObjectManager::GetSkinObject(int	iIndex)
{
	if (iIndex >= m_SkinObjectList.size()) return nullptr;
	else return &m_SkinObjectList[iIndex];
}
int	SSkinObjectManager::GetSize()
{
	return m_SkinObjectList.size();
}
