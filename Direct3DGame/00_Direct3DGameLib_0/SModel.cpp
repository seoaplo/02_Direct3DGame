#include "SModel.h"

bool SModel::Convert(ID3D11Device* pDevice) {
	return true;
};
bool SModel::Load(ID3D11Device* pDevice, const TCHAR* szLoadName, const TCHAR* pLoadShaderFile, bool bThread)
{
	return Convert(pDevice);
};
bool SModel::Create(ID3D11Device* pDevice,
	const TCHAR* pLoadShaderFile,
	const TCHAR* pLoadTextureString)
{

	if (pDevice == nullptr)
	{
		MessageBox(0, _T("pDevice가 nullptr입니다"), _T("Fatal error"), MB_OK);
		return false;
	}

	m_pDevice = pDevice;

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
	if (FAILED(LoadTextures(pDevice, pLoadTextureString)))
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

	return Init();
}
HRESULT SModel::LoadTextures(ID3D11Device* pDevice, const TCHAR* pLoadTextureString)
{
	HRESULT hr = S_OK;
	int iTextureID = I_TextureManager.Load(pDevice, pLoadTextureString);
	m_dxobj.g_pTextureSRV.Attach(I_TextureManager.GetPtr(iTextureID)->m_pSRV);

	return hr;
}
HRESULT SModel::LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pShaderFile)
{
	m_dxobj.g_pVertexShader.Attach(DXGame::LoadVertexShaderFile(pDevice, pShaderFile, m_dxobj.g_pVSBlob.GetAddressOf()));
	m_dxobj.g_pPixelShader.Attach(DXGame::LoadPixelShaderFile(pDevice, pShaderFile));
	m_dxobj.g_pGeometryShader.Attach(DXGame::LoadGeometryShaderFile(pDevice, pShaderFile, m_dxobj.g_pGSBlob.GetAddressOf()));
	m_dxobj.g_pHullShader.Attach(DXGame::LoadHullShaderFile(pDevice, pShaderFile, m_dxobj.g_pHSBlob.GetAddressOf()));
	m_dxobj.g_pDomainShader.Attach(DXGame::LoadDomainShaderFile(pDevice, pShaderFile, m_dxobj.g_pDSBlob.GetAddressOf()));
	m_dxobj.g_pComputeShader.Attach(DXGame::LoadComputeShaderFile(pDevice, pShaderFile, m_dxobj.g_pCSBlob.GetAddressOf()));
	return S_OK;
}
bool	SModel::CreateVertexData()
{
	return true;
}
bool	SModel::CreateIndexData()
{
	return true;
}
HRESULT SModel::CreateVertexBuffer()
{
	if (m_dxobj.m_iNumVertex <= 0) return S_OK;
	void** pData = nullptr;
	if (m_VertexList.size() > 0) pData = (void**)&m_VertexList.at(0);

	m_dxobj.g_pVertexBuffer.Attach(DXGame::CreateVertexBuffer(m_pDevice,
		pData,
		m_dxobj.m_iNumVertex,
		m_dxobj.m_iVertexSize, true));
	return S_OK;
}
HRESULT	SModel::CreateIndexBuffer()
{
	if (m_dxobj.m_iNumIndex <= 0) return S_OK;
	void** pData = nullptr;
	if (m_IndexList.size() > 0) pData = (void**)&m_IndexList.at(0);

	m_dxobj.g_pIndexBuffer.Attach(DXGame::CreateIndexBuffer(m_pDevice,
		pData,
		m_dxobj.m_iNumIndex,
		m_dxobj.m_iIndexSize));
	return S_OK;
}
HRESULT SModel::CreateConstantBuffer()
{
	m_cbData.Color = D3DXVECTOR4(1, 1, 1, 1);
	m_dxobj.g_pConstantBuffer.Attach(DXGame::CreateConstantBuffer(m_pDevice, &m_cbData, 1, sizeof(VS_CONSTANT_BUFFER)));
	return S_OK;
}
bool SModel::Draw(ID3D11DeviceContext* pContext, D3DXVECTOR3 vStart, D3DXVECTOR3 vPost, D3DXVECTOR4 vColor) { return true; }
HRESULT SModel::SetInputLayout()
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
	m_dxobj.g_pInputlayout.Attach(DXGame::CreateInputlayout(m_pDevice, m_dxobj.g_pVSBlob.Get()->GetBufferSize(),
		m_dxobj.g_pVSBlob.Get()->GetBufferPointer(), layout, numElements));
	return hr;
}
bool SModel::UpdateBuffer() { return true; }
bool SModel::Init() { return true; }
bool SModel::Frame() { return true; }
bool SModel::Release() { return true; }
bool SModel::PreRender(ID3D11DeviceContext* pContext)
{
	pContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_dxobj.m_iPrimitiveType);
	m_dxobj.PreRender(pContext, m_dxobj.m_iVertexSize);
	return true;
}
void SModel::UpdateConstantBuffer(ID3D11DeviceContext* pContext, SModel* pParent)
{
	if (pParent != nullptr)
	{
		pContext->UpdateSubresource(pParent->m_dxobj.g_pConstantBuffer.Get(), 0, nullptr, &pParent->m_cbData, 0, 0);
	}
	else
	{
		pContext->UpdateSubresource(m_dxobj.g_pConstantBuffer.Get(), 0, nullptr, &m_cbData, 0, 0);
	}
}
bool SModel::PostRender(ID3D11DeviceContext* pContext)
{
	UpdateConstantBuffer(pContext);
	m_dxobj.PostRender(pContext, m_dxobj.m_iNumIndex);
	return true;
}
bool SModel::Render(ID3D11DeviceContext* pContext)
{
	PreRender(pContext);
	PostRender(pContext);
	return true;
}

void SModel::SetAmbientColor(float fR, float fG, float fB, float fA)
{
	m_cbData.Color = D3DXVECTOR4(fR, fG, fB, fA);
}
void SModel::SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj)
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
	D3DXMatrixTranspose(&m_cbData.matWorld, &m_matWorld);
	D3DXMatrixTranspose(&m_cbData.matView, &m_matView);
	D3DXMatrixTranspose(&m_cbData.matProj, &m_matProj);
}
ID3D11Buffer* SModel::GetVB()
{
	return m_dxobj.g_pVertexBuffer.Get();
}
ID3D11Buffer* SModel::GetIB()
{
	return m_dxobj.g_pIndexBuffer.Get();
}
HRESULT SModel::CreateResource()
{
	m_dxobj.m_iPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	return S_OK;
}
HRESULT SModel::DeleteResource()
{
	return S_OK;
}
SModel::SModel()
{
	m_pDevice = nullptr;
	m_cbData.Color = D3DXVECTOR4(1, 1, 1, 1);
	m_vCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
	D3DXMatrixIdentity(&m_matWorld);

}

SModel::~SModel()
{
}
