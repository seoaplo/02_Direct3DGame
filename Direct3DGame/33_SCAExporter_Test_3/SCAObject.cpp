#include "SCAObject.h"

bool SCAObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pShaderFile)
{
	SDrawObject::Create(pDevice, pContext, pShaderFile);
	CreateComputeShader(L"CSMatrixCalculation.hlsl", "CS", pDevice, &m_pCS);

	return true;
}

//--------------------------------------------------------------------------------------
// Compile and create the CS
//--------------------------------------------------------------------------------------
HRESULT SCAObject::CreateComputeShader(LPCWSTR pSrcFile, LPCSTR pFunctionName,
	ID3D11Device* pDevice, ID3D11ComputeShader** ppShaderOut)
{
	HRESULT hr;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG ) 
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
#ifdef USE_STRUCTURED_BUFFERS
		"USE_STRUCTURED_BUFFERS", "1",
#endif
		NULL, NULL
	};

	// We generally prefer to use the higher CS shader profile when possible as CS 5.0 is better performance on 11-class hardware
	LPCSTR pProfile = (pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";

	ID3DBlob* pErrorBlob = NULL;
	ID3DBlob* pBlob = NULL;
	hr = D3DX11CompileFromFile(pSrcFile, defines, NULL, pFunctionName, pProfile,
		dwShaderFlags, NULL, NULL, &pBlob, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

		SAFE_RELEASE(pErrorBlob);
		SAFE_RELEASE(pBlob);

		return hr;
	}

	hr = pDevice->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, ppShaderOut);

	SAFE_RELEASE(pErrorBlob);
	SAFE_RELEASE(pBlob);

	return hr;
}
//--------------------------------------------------------------------------------------
// Run CS
//-------------------------------------------------------------------------------------- 
void SCAObject::RunComputeShader(ID3D11DeviceContext* pd3dImmediateContext,
	ID3D11ComputeShader* pComputeShader,
	UINT nNumViews, ID3D11ShaderResourceView** pShaderResourceViews,
	ID3D11Buffer* pCBCS, void* pCSData, DWORD dwNumDataBytes,
	ID3D11UnorderedAccessView* pUnorderedAccessView,
	UINT X, UINT Y, UINT Z)
{
	pd3dImmediateContext->CSSetShader(pComputeShader, NULL, 0);
	pd3dImmediateContext->CSSetShaderResources(0, nNumViews, pShaderResourceViews);
	pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &pUnorderedAccessView, NULL);
	if (pCBCS)
	{
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		pd3dImmediateContext->Map(pCBCS, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		memcpy(MappedResource.pData, pCSData, dwNumDataBytes);
		pd3dImmediateContext->Unmap(pCBCS, 0);
		ID3D11Buffer* ppCB[1] = { pCBCS };
		pd3dImmediateContext->CSSetConstantBuffers(0, 1, ppCB);
	}

	pd3dImmediateContext->Dispatch(X, Y, Z);

	pd3dImmediateContext->CSSetShader(NULL, NULL, 0);

	ID3D11UnorderedAccessView* ppUAViewNULL[1] = { NULL };
	pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, ppUAViewNULL, NULL);

	ID3D11ShaderResourceView* ppSRVNULL[2] = { NULL, NULL };
	pd3dImmediateContext->CSSetShaderResources(0, 2, ppSRVNULL);

	ID3D11Buffer* ppCBNULL[1] = { NULL };
	pd3dImmediateContext->CSSetConstantBuffers(0, 1, ppCBNULL);
}

void SCAObject::CreateComputeData()
{
	if (m_pMatrixObjectList != nullptr)
	{
		m_pMatrixObjectList->Frame();
	}

	for (int iSkinObj = 0; iSkinObj < m_iSkinObjectListSize; iSkinObj++)
	{
		m_SkinObjectList[iSkinObj]->Frame();
	}
}
void SCAObject::CreateComputeBuffer()
{

}
bool SCAObject::Init()
{
	return true;
}
bool SCAObject::Frame()
{
	if (m_pMatrixObjectList != nullptr)
	{
		m_pMatrixObjectList->Frame();
	}

	for (int iSkinObj = 0; iSkinObj < m_iSkinObjectListSize; iSkinObj++)
	{
		m_SkinObjectList[iSkinObj]->Frame();
	}
	return true;
}
bool SCAObject::Render(ID3D11DeviceContext* pContext)
{
	if (m_pMatrixObjectList == nullptr || m_iSkinObjectListSize <= 0) return false;
	
	RunComputeShader(pContext, m_pCS.Get(),
			1, m_SkinObjectList[0]->m_pInvMatBufferSRV.GetAddressOf(),
			NULL, NULL, 0, m_pMatrixObjectList->m_pMatrixUAV.Get(),
			m_pMatrixObjectList->m_MatrixObjectList.size(), 1, 1);
	
	m_pMatrixObjectList->SetMatrix(&m_matWorld, &m_matView, &m_matProj);
	m_pMatrixObjectList->Render(pContext);

	for (int iSkinObj = 0; iSkinObj < m_iSkinObjectListSize; iSkinObj++)
	{
		m_SkinObjectList[iSkinObj]->SetMatrix(&m_matWorld, &m_matView, &m_matProj);
		pContext->VSSetShaderResources(1, 1, m_pMatrixObjectList->m_pMatrixSRV.GetAddressOf());
		//pContext->VSSetConstantBuffers(1, 1, m_pMatrixObjectList->m_pMatListConstantBuffer.GetAddressOf());
		m_SkinObjectList[iSkinObj]->Render(pContext);
	}
	return true;
}
bool SCAObject::Release()
{
	ClearSkinObjectList();
	m_pMatrixObjectList = nullptr;
	return true;
}

bool SCAObject::SetMatrixObjectList(int iMatrixObejct)
{
	m_pMatrixObjectList = I_MatrixObjectListManager.GetMatrixObjectList(iMatrixObejct);
	return true;
}
bool SCAObject::SetSkinObject(int iSkinObejct)
{
	SSkinObject* pSkinObject = I_SkinObjectManager.GetSkinObject(iSkinObejct);
	if (pSkinObject == nullptr) return false;
	m_iSkinObjectListSize++;
	m_SkinObjectList.push_back(pSkinObject);
	return true;
}
void SCAObject::ClearSkinObjectList()
{
	m_SkinObjectList.clear();
	m_iSkinObjectListSize = 0;
}
SCAObject::SCAObject()
{
	m_iSkinObjectListSize = 0;
	m_pMatrixObjectList = nullptr;

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);

}


SCAObject::~SCAObject()
{
}
