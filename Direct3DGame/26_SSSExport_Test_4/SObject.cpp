#include "SObject.h"

bool SObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pLoadShaderFile)
{
	if (pDevice == nullptr)
	{
		MessageBox(0, _T("pDevice가 nullptr입니다"), _T("Fatal error"), MB_OK);
		return false;
	}

	m_pDevice = pDevice;
	m_pContext = pContext;
	if (FAILED(LoadShaderFile(pDevice, pLoadShaderFile)))
	{
		MessageBox(0, _T("LoadShaderFile 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(SetInputLayout()))
	{
		MessageBox(0, _T("SetInputLayout 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (!CreateVertexData())
	{
		MessageBox(0, _T("CreateVertexData 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (!CreateIndexData())
	{
		MessageBox(0, _T("CreateIndexData 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(CreateVertexBuffer()))
	{
		MessageBox(0, _T("CreateVertexBuffer 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(CreateIndexBuffer()))
	{
		MessageBox(0, _T("CreateIndexBuffer 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(CreateConstantBuffer()))
	{
		MessageBox(0, _T("CreateConstantBuffer 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(LoadTextures(pDevice)))
	{
		MessageBox(0, _T("LoadTextures 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (!UpdateBuffer())
	{
		MessageBox(0, _T("UpdateBuffer 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(CreateResource()))
	{
		MessageBox(0, _T("CreateResource 실패"), _T("Fatal error"), MB_OK);
		return false;
	}

	m_matWorld = m_pMesh->matWorld;

	return Init();

	return true;
}
HRESULT SObject::LoadTextures(ID3D11Device* pDevice)
{

	HRESULT hr = S_OK;
	
	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		m_pMesh->m_dxobjList[iSubMesh].g_pTextureSRV = m_pMaterial->SubMaterial[iSubMesh].TextrueMapList[0].STexture->m_pSRV;
	}

	return hr;
}
HRESULT SObject::LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pLoadShaderFile)
{
	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		DXGame::SDxHelperEX& dxobj = m_pMesh->m_dxobjList[iSubMesh];
		dxobj.g_pVertexShader.Attach(DXGame::LoadVertexShaderFile(pDevice, pLoadShaderFile, dxobj.g_pVSBlob.GetAddressOf()));;
		dxobj.g_pPixelShader.Attach(DXGame::LoadPixelShaderFile(pDevice, pLoadShaderFile));;
		dxobj.g_pGeometryShader.Attach(DXGame::LoadGeometryShaderFile(pDevice, pLoadShaderFile, dxobj.g_pGSBlob.GetAddressOf()));;
		dxobj.g_pHullShader.Attach(DXGame::LoadHullShaderFile(pDevice, pLoadShaderFile, dxobj.g_pHSBlob.GetAddressOf()));;
		dxobj.g_pDomainShader.Attach(DXGame::LoadDomainShaderFile(pDevice, pLoadShaderFile, dxobj.g_pDSBlob.GetAddressOf()));;
		dxobj.g_pComputeShader.Attach(DXGame::LoadComputeShaderFile(pDevice, pLoadShaderFile, dxobj.g_pCSBlob.GetAddressOf()));;
	}
	return S_OK;
}
HRESULT SObject::CreateVertexBuffer()
{
	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		DXGame::SDxHelperEX& dxobj = m_pMesh->m_dxobjList[iSubMesh];
		dxobj.m_iNumVertex = m_pMesh->m_VertexList[iSubMesh].size();
		dxobj.m_iVertexSize = sizeof(PNCT_VERTEX);

		if (dxobj.m_iNumVertex <= 0) return S_OK;
		void** pData = nullptr;
		if (m_pMesh->m_VertexList[iSubMesh].size() > 0) pData = (void**)&m_pMesh->m_VertexList[iSubMesh].at(0);

		
		dxobj.g_pVertexBuffer.Attach(DXGame::CreateVertexBuffer(m_pDevice,
			pData,
			dxobj.m_iNumVertex,
			dxobj.m_iVertexSize));
	}

	return S_OK;
}
HRESULT	SObject::CreateIndexBuffer()
{

	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		DXGame::SDxHelperEX& dxobj = m_pMesh->m_dxobjList[iSubMesh];
		dxobj.m_iNumIndex = m_pMesh->m_IndexList[iSubMesh].size();
		dxobj.m_iIndexSize = sizeof(DWORD);

		if (dxobj.m_iNumIndex <= 0) return S_OK;
		void** pData = nullptr;
		if (m_pMesh->m_IndexList[iSubMesh].size() > 0) pData = (void**)&m_pMesh->m_IndexList[iSubMesh].at(0);

		dxobj.g_pIndexBuffer.Attach(DXGame::CreateIndexBuffer(m_pDevice,
			pData,
			dxobj.m_iNumIndex,
			dxobj.m_iIndexSize));
	}

	return S_OK;
}
HRESULT SObject::CreateConstantBuffer()
{
	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		DXGame::SDxHelperEX& dxobj = m_pMesh->m_dxobjList[iSubMesh];
		dxobj.g_pConstantBuffer.Attach(DXGame::CreateConstantBuffer(m_pDevice, &m_cbData, 1, sizeof(VS_CONSTANT_BUFFER)));
	}
	return S_OK;
}
bool SObject::Draw(ID3D11DeviceContext* pContext, D3DXVECTOR3 vStart, D3DXVECTOR3 vPost, D3DXVECTOR4 vColor) { return true; }
HRESULT SObject::SetInputLayout()
{
	HRESULT hr = S_OK;
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);
	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		DXGame::SDxHelperEX& dxobj = m_pMesh->m_dxobjList[iSubMesh];
		dxobj.g_pInputlayout.Attach(DXGame::CreateInputlayout(m_pDevice, dxobj.g_pVSBlob.Get()->GetBufferSize(),
			dxobj.g_pVSBlob.Get()->GetBufferPointer(), layout, numElements));
	}
	return hr;
}
bool SObject::UpdateBuffer() { return true; }
bool SObject::Init() { return true; }
bool SObject::Frame() { return true; }
bool SObject::Release() { return true; }
bool SObject::PreRender(ID3D11DeviceContext* pContext)
{
	pContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_dxobj.m_iPrimitiveType);
	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		DXGame::SDxHelperEX& dxobj = m_pMesh->m_dxobjList[iSubMesh];
		dxobj.PreRender(pContext, dxobj.m_iVertexSize);
	}
	return true;
}
void SObject::UpdateConstantBuffer(ID3D11DeviceContext* pContext, SObject* pParent)
{
	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		DXGame::SDxHelperEX& dxobj = m_pMesh->m_dxobjList[iSubMesh];
		if (pParent != nullptr)
		{
			pContext->UpdateSubresource(pParent->m_dxobj.g_pConstantBuffer.Get(), 0, nullptr, &pParent->m_cbData, 0, 0);
		}
		else
		{
			pContext->UpdateSubresource(dxobj.g_pConstantBuffer.Get(), 0, nullptr, &m_cbData, 0, 0);
		}
	}
}
bool SObject::PostRender(ID3D11DeviceContext* pContext)
{
	UpdateConstantBuffer(pContext);
	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		DXGame::SDxHelperEX& dxobj = m_pMesh->m_dxobjList[iSubMesh];
		dxobj.PostRender(pContext, dxobj.m_iNumIndex);
	}
	return true;
}
bool SObject::Render(ID3D11DeviceContext* pContext)
{
	UpdateConstantBuffer(pContext);
	pContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_dxobj.m_iPrimitiveType);
	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		DXGame::SDxHelperEX& dxobj = m_pMesh->m_dxobjList[iSubMesh];
		dxobj.PreRender(pContext, dxobj.m_iVertexSize);
		dxobj.PostRender(pContext, dxobj.m_iNumIndex);
	}
	return true;
}
HRESULT SObject::CreateResource()
{
	m_dxobj.m_iPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	return S_OK;
}
HRESULT SObject::DeleteResource()
{
	return S_OK;
}
SObject::SObject()
{
	m_pDevice = nullptr;
	m_cbData.Color = D3DXVECTOR4(1, 1, 1, 1);
	m_vCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
	D3DXMatrixIdentity(&m_matWorld);

}

SObject::~SObject()
{
}
