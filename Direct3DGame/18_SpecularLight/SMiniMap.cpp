#include "SMiniMap.h"

void SMiniMap::Set(ID3D11Device* pDevice, float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth)
{
	m_vp.TopLeftX = TopLeftX;
	m_vp.TopLeftY = TopLeftY;
	m_vp.Width = Width;
	m_vp.Height = Height;
	m_vp.MinDepth = MinDepth;
	m_vp.MaxDepth = MaxDepth;
}

bool SMiniMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const TCHAR* pLoadTexture, const TCHAR* pLoadShaderFile)
{
	m_pPlaneShape = make_shared<SPlane>();
	if (FAILED(m_pPlaneShape->Create(pDevice, pDeviceContext, pLoadTexture, pLoadShaderFile)))
	{
		MessageBox(0, _T("Plane ½ÇÆÐ"), _T("Fatal error"), MB_OK);
		return 0;
	}

	m_pMiniMapRT = make_shared<DXGame::SDxRT>();
	if (FAILED(m_pMiniMapRT->Create(pDevice)))
	{
		return false;
	}
	D3DXVECTOR3 vUpVector(0.0f, 1.0f, 0.0f);
	DWORD dwWidth = m_pMiniMapRT->m_TexDesc.Width;
	DWORD dwHeight = m_pMiniMapRT->m_TexDesc.Height;
	SetViewMatrix(D3DXVECTOR3(0.0f, 70.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), vUpVector);
	SetProjMatrix(D3DX_PI * 0.25f, (float)dwWidth / (float)dwHeight, 0.1f, 1000.0f);

	return true;
}

bool SMiniMap::BeginRender(ID3D11DeviceContext* pContext, D3DXVECTOR4 vColor)
{
	if (m_pMiniMapRT->Begin(pContext, vColor) == false)
	{
		m_pMiniMapRT->End(pContext);
		return false;
	}
	return true;
}
bool SMiniMap::EndRender(ID3D11DeviceContext* pContext)
{
	return m_pMiniMapRT->End(pContext);
}
bool SMiniMap::Frame()
{
	return true;
}
bool SMiniMap::Render(ID3D11DeviceContext* pContext)
{
	m_pPlaneShape->_dxobj.g_pTextureSRV = m_pMiniMapRT->m_pSRV;
	pContext->RSSetViewports(1, &m_vp);
	m_pPlaneShape->SetMatrix(nullptr, nullptr, nullptr);
	pContext->UpdateSubresource(m_pPlaneShape->_dxobj.g_pConstantBuffer.Get(), 0, nullptr, &m_pPlaneShape->_cbData, 0, 0);
	m_pPlaneShape->Render(pContext);
	return true;
}

bool SMiniMap::Release()
{
	return true;
}
HRESULT SMiniMap::CreateResource(ID3D11Device* pd3dDevice, ID3D11DeviceContext*    pImmediateContext, UINT iWidth, UINT iHeight)
{
	HRESULT hr = S_OK;
	return hr;
}
HRESULT SMiniMap::DeleteResource()
{
	HRESULT hr = S_OK;
	return S_OK;
}

SMiniMap::SMiniMap()
{
}


SMiniMap::~SMiniMap()
{
}
