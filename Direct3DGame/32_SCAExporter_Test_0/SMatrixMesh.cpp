#include "SMatrixMesh.h"


const TCHAR BoxShader[] = L"Debug.hlsl";

//----------------------------------------------------------------------------------------------------------
// Create
//----------------------------------------------------------------------------------------------------------
bool SMatrixMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, STextureList* pLoadTextureList, const TCHAR* pLoadShaderFile)
{
	if (pDevice == nullptr)
	{
		MessageBox(0, _T("pDevice�� nullptr�Դϴ�"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (pContext == nullptr)
	{
		MessageBox(0, _T("pContext�� nullptr�Դϴ�"), _T("Fatal error"), MB_OK);
		return false;
	}

	m_pDevice = pDevice;
	m_pContext = pContext;

	if (pLoadShaderFile == nullptr) pLoadShaderFile = BoxShader;

	if (FAILED(LoadShaderFile(pDevice, pLoadShaderFile)))
	{
		MessageBox(0, _T("LoadShaderFile ����"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(SetInputLayout()))
	{
		MessageBox(0, _T("SetInputLayout ����"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (!CreateVertexData())
	{
		MessageBox(0, _T("CreateVertexData ����"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (!CreateIndexData())
	{
		MessageBox(0, _T("CreateIndexData ����"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(CreateVertexBuffer()))
	{
		MessageBox(0, _T("CreateVertexBuffer ����"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(CreateIndexBuffer()))
	{
		MessageBox(0, _T("CreateIndexBuffer ����"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(CreateConstantBuffer()))
	{
		MessageBox(0, _T("CreateConstantBuffer ����"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(LoadTextures(pDevice)))
	{
		MessageBox(0, _T("LoadTextures ����"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (!UpdateBuffer())
	{
		MessageBox(0, _T("UpdateBuffer ����"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(CreateResource()))
	{
		MessageBox(0, _T("CreateResource ����"), _T("Fatal error"), MB_OK);
		return false;
	}

	return true;
}

HRESULT SMatrixMesh::LoadTextures(ID3D11Device* pDevice)
{
	HRESULT hr = S_OK;
	m_dxobj.g_pTextureSRV = nullptr;
	return hr;
}
bool SMatrixMesh::PreRender(ID3D11DeviceContext* pContext)
{
	pContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_dxobj.m_iPrimitiveType);
	m_dxobj.PreRender(pContext, m_dxobj.m_iVertexSize);

	return true;
}

SMatrixMesh::SMatrixMesh()
{
}


SMatrixMesh::~SMatrixMesh()
{
}
