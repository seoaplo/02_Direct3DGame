#include "SMatrixMesh.h"


const TCHAR BoxShader[] = L"Debug.hlsl";

//----------------------------------------------------------------------------------------------------------
// Create
//----------------------------------------------------------------------------------------------------------
bool SMatrixMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, STextureList* pLoadTextureList, const TCHAR* pLoadShaderFile)
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
