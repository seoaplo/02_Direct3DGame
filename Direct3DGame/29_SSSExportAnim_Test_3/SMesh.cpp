#include "SMesh.h"

const TCHAR BoxShader[] = L"../../shader/Shape/Box.hlsl";

//----------------------------------------------------------------------------------------------------------
// Create
//----------------------------------------------------------------------------------------------------------
bool SMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, STextureList* pLoadTextureList, const TCHAR* pLoadShaderFile)
{
	if (pDevice == nullptr)
	{
		MessageBox(0, _T("pDevice가 nullptr입니다"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (pContext == nullptr)
	{
		MessageBox(0, _T("pContext가 nullptr입니다"), _T("Fatal error"), MB_OK);
		return false;
	}

	m_pDevice = pDevice;
	m_pContext = pContext;
	m_pTextureList = pLoadTextureList;

	if (pLoadShaderFile == nullptr) pLoadShaderFile = BoxShader;

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

	return true;
}
bool SMesh::CreateVertexData()
{
	m_dxobj.m_iNumVertex = m_VertexList.size();
	m_dxobj.m_iVertexSize = sizeof(PNCT_VERTEX);

	return true;
}
bool SMesh::CreateIndexData()
{

	m_dxobj.m_iNumIndex = m_IndexList.size();
	m_dxobj.m_iIndexSize = sizeof(DWORD);

	return true;
}
HRESULT SMesh::CreateVertexBuffer()
{
	HRESULT hr = S_OK;

	if (m_dxobj.m_iNumVertex <= 0) return hr;
	void** pData = nullptr;
	if (m_VertexList.size() > 0) pData = (void**)&m_VertexList.at(0);

	m_dxobj.g_pVertexBuffer.Attach(DXGame::CreateVertexBuffer(m_pDevice,
		pData,
		m_dxobj.m_iNumVertex,
		m_dxobj.m_iVertexSize));

	return hr;
}
HRESULT SMesh::CreateIndexBuffer()
{
	HRESULT hr = S_OK;
	if (m_dxobj.m_iNumIndex <= 0) return hr;
	void** pData = nullptr;
	if (m_IndexList.size() > 0) pData = (void**)&m_IndexList.at(0);

	m_dxobj.g_pIndexBuffer.Attach(DXGame::CreateIndexBuffer(m_pDevice,
		pData,
		m_dxobj.m_iNumIndex,
		m_dxobj.m_iIndexSize));

	return hr;
}
HRESULT SMesh::CreateResource()
{
	HRESULT hr = S_OK;
	m_dxobj.g_pConstantBuffer.Attach(DXGame::CreateConstantBuffer(m_pDevice, &m_cbData, 1, sizeof(VS_CONSTANT_BUFFER)));
	return hr;
}
//----------------------------------------------------------------------------------------------------------
// Load
//----------------------------------------------------------------------------------------------------------

bool SMesh::Load(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* szLoadName, const TCHAR* pLoadShaderFile, bool bThread)
{
	return true;
}
HRESULT SMesh::LoadTextures(ID3D11Device* pDevice)
{
	HRESULT hr = S_OK;
	return hr;
}
HRESULT SMesh::SetInputLayout()
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

//----------------------------------------------------------------------------------------------------------
// Frame Work
//----------------------------------------------------------------------------------------------------------
bool SMesh::Init()
{
	return true;
}
bool SMesh::Frame()
{
	return true;
}
bool SMesh::PreRender(ID3D11DeviceContext* pContext)
{
	return true;
}
bool SMesh::Render(ID3D11DeviceContext*		pContext)
{
	return true;

}
bool SMesh::PostRender(ID3D11DeviceContext* pContext)
{
	return true;
}
bool SMesh::Release()
{
	return true;
}