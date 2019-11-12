#include "SSkinObject.h"

//--------------------------------------------------------------------------------------
// Create Structured Buffer
//--------------------------------------------------------------------------------------
HRESULT SSkinObject::CreateStructuredBuffer(ID3D11Device* pDevice, UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBufOut)
{
	*ppBufOut = NULL;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = uElementSize * uCount;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = uElementSize;

	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		return pDevice->CreateBuffer(&desc, &InitData, ppBufOut);
	}
	else
		return pDevice->CreateBuffer(&desc, NULL, ppBufOut);
}

//--------------------------------------------------------------------------------------
// Create Shader Resource View for Structured or Raw Buffers
//--------------------------------------------------------------------------------------
HRESULT SSkinObject::CreateBufferSRV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	pBuffer->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer

		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
	}
	else
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer

			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
		else
		{
			return E_INVALIDARG;
		}

	return pDevice->CreateShaderResourceView(pBuffer, &desc, ppSRVOut);
}

//--------------------------------------------------------------------------------------
// Create Unordered Access View for Structured or Raw Buffers
//-------------------------------------------------------------------------------------- 
HRESULT SSkinObject::CreateBufferUAV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	pBuffer->GetDesc(&descBuf);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer

		desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Buffer.NumElements = descBuf.ByteWidth / 4;
	}
	else
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer

			desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
			desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
		else
		{
			return E_INVALIDARG;
		}

	return pDevice->CreateUnorderedAccessView(pBuffer, &desc, ppUAVOut);
}

//--------------------------------------------------------------------------------------
// Create a CPU accessible buffer and download the content of a GPU buffer into it
// This function is very useful for debugging CS programs
//-------------------------------------------------------------------------------------- 
ID3D11Buffer* SSkinObject::CreateAndCopyToBuf(ID3D11Device* pDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11Buffer* pBuffer)
{
	ID3D11Buffer* debugbuf = NULL;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	pBuffer->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	if (SUCCEEDED(pDevice->CreateBuffer(&desc, NULL, &debugbuf)))
	{
		pd3dImmediateContext->CopyResource(debugbuf, pBuffer);
	}

	return debugbuf;
}

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

	CreateStructuredBuffer(pDevice, sizeof(D3DXMATRIX),
		AnimationMatrixList,
		m_InvMatrixList,
		m_pInvMatBuffer.GetAddressOf());

	CreateBufferSRV(pDevice, m_pInvMatBuffer.Get(), m_pInvMatBufferSRV.GetAddressOf());
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

	for (int iCount = 0; iCount < AnimationMatrixList; iCount++)
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
	SSkinObject* NewSkinData = new SSkinObject();
	m_SkinObjectList.push_back(NewSkinData);
	m_SkinObjectList[m_SkinObjectList.size() - 1]->m_iIndex = m_SkinObjectList.size() - 1;
	return m_SkinObjectList[m_SkinObjectList.size() - 1];
}

SSkinObject* SSkinObjectManager::GetSkinObject(T_STR DrawObjectName)
{
	for (auto& Target : m_SkinObjectList)
	{
		if (Target->m_ObjectName == DrawObjectName)
		{
			return Target;
		}
	}
	return nullptr;
}
SSkinObject* SSkinObjectManager::GetSkinObject(int	iIndex)
{
	if (iIndex >= m_SkinObjectList.size()) return nullptr;
	else return m_SkinObjectList[iIndex];
}
int	SSkinObjectManager::GetSize()
{
	return m_SkinObjectList.size();
}
