#include "SSkinMesh.h"

const TCHAR BoxShader[] = L"../../shader/Shape/Box.hlsl";

//----------------------------------------------------------------------------------------------------------
// Create
//----------------------------------------------------------------------------------------------------------
bool SSkinMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, STextureList* pLoadTextureList, const TCHAR* pLoadShaderFile)
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
	if(pLoadTextureList != nullptr) m_TextureList = *pLoadTextureList;

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
bool SSkinMesh::CreateVertexData()
{
	m_dxobj.m_iNumVertex = m_VertexList.size();
	m_dxobj.m_iVertexSize = sizeof(PNCT_VERTEX);

	return true;
}
bool SSkinMesh::CreateIndexData()
{

	m_dxobj.m_iNumIndex = m_IndexList.size();
	m_dxobj.m_iIndexSize = sizeof(DWORD);

	return true;
}
HRESULT SSkinMesh::CreateVertexBuffer()
{
	HRESULT hr = S_OK;

	if (m_dxobj.m_iNumVertex <= 0) return hr;
	void** pData = nullptr;
	if (m_VertexList.size() > 0) pData = (void**)&m_VertexList.at(0);

	m_dxobj.g_pVertexBuffer.Attach(DXGame::CreateVertexBuffer(m_pDevice,
		pData,
		m_dxobj.m_iNumVertex,
		m_dxobj.m_iVertexSize));
	
	int iIW_VERTEX_Size = sizeof(IW_VERTEX);
	int iIW_VERTEX_Num = m_IW_VertexList.size();
	if (m_IW_VertexList.size() > 0) pData = (void**)&m_IW_VertexList.at(0);
	m_pIW_VertexBuffer = DXGame::CreateVertexBuffer(m_pDevice,
		pData,
		iIW_VERTEX_Num,
		iIW_VERTEX_Size);

	return hr;
}
HRESULT SSkinMesh::CreateIndexBuffer()
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
HRESULT SSkinMesh::CreateResource()
{
	HRESULT hr = S_OK;
	m_dxobj.g_pConstantBuffer.Attach(DXGame::CreateConstantBuffer(m_pDevice, &m_cbData, 1, sizeof(VS_CONSTANT_BUFFER)));
	return hr;
}
//----------------------------------------------------------------------------------------------------------
// Load
//----------------------------------------------------------------------------------------------------------

bool SSkinMesh::Load(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* szLoadName, const TCHAR* pLoadShaderFile, bool bThread)
{
	return true;
}
HRESULT SSkinMesh::LoadTextures(ID3D11Device* pDevice)
{
	HRESULT hr = S_OK;
	m_dxobj.g_pTextureSRV = nullptr;
	return hr;
}
HRESULT SSkinMesh::SetInputLayout()
{
	HRESULT hr = S_OK;
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0  },

		{"TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);
	m_dxobj.g_pInputlayout.Attach(DXGame::CreateInputlayout(m_pDevice, m_dxobj.g_pVSBlob.Get()->GetBufferSize(),
		m_dxobj.g_pVSBlob.Get()->GetBufferPointer(), layout, numElements));
	return hr;
}

void SSkinMesh::SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj)
{
	if (pWorld != NULL)
	{
		m_matWorld = *pWorld;
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

//----------------------------------------------------------------------------------------------------------
// Frame Work
//----------------------------------------------------------------------------------------------------------
bool SSkinMesh::Init()
{
	return true;
}
bool SSkinMesh::Frame()
{
	return true;
}
bool SSkinMesh::PreRender(ID3D11DeviceContext* pContext)
{
	pContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_dxobj.m_iPrimitiveType);
	m_dxobj.PreRender(pContext, m_dxobj.m_iVertexSize);

	ID3D11Buffer* Buffer[2] = { m_dxobj.g_pVertexBuffer.Get(), m_pIW_VertexBuffer.Get() };
	UINT stride[2] = { sizeof(PNCT_VERTEX), sizeof(IW_VERTEX) };
	UINT offset[2] = { 0, 0 };

	pContext->IASetVertexBuffers(0, 2, Buffer, stride, offset);
	for (int iCount = 0; iCount < TexType_Size; iCount++)
	{
		if (m_TextureList.List[iCount] != nullptr)
		{
			pContext->PSSetShaderResources(iCount, 1, &m_TextureList.List[iCount]->m_pSRV);
		}
	}

	return true;
}
bool SSkinMesh::Render(ID3D11DeviceContext*		pContext)
{
	PreRender(pContext);
	PostRender(pContext);
	return true;

}
bool SSkinMesh::PostRender(ID3D11DeviceContext* pContext)
{
	UpdateConstantBuffer(pContext);
	m_dxobj.PostRender(pContext, m_dxobj.m_iNumIndex);
	return true;
}
bool SSkinMesh::Release()
{
	return true;
}

SSkinMesh::SSkinMesh()
{
}


SSkinMesh::~SSkinMesh()
{
}
