#include "SDxHelperEX.h"

namespace DXGame
{
	void ClearD3D11DeviceContext(ID3D11DeviceContext*	pd3dDeviceContext)
	{
		// Unbind all objects from the immediate context
		ID3D11ShaderResourceView*	pSRVs[16] = { nullptr, };
		ID3D11RenderTargetView*		pRTVs[16] = { nullptr, };
		ID3D11DepthStencilView*		pDSV = nullptr;
		ID3D11Buffer*				pBuffers[16] = { nullptr, };
		ID3D11SamplerState*			pSamplers[16] = { nullptr, };
		UINT StrideOffset[16] = { 0, };

		// Shaders
		pd3dDeviceContext->VSSetShader(NULL, NULL, 0);
		pd3dDeviceContext->HSSetShader(NULL, NULL, 0);
		pd3dDeviceContext->GSSetShader(NULL, NULL, 0);
		pd3dDeviceContext->DSSetShader(NULL, NULL, 0);
		pd3dDeviceContext->PSSetShader(NULL, NULL, 0);

		// IA clear
		pd3dDeviceContext->IASetVertexBuffers(0, 16, pBuffers, StrideOffset, StrideOffset);
		pd3dDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		pd3dDeviceContext->IASetInputLayout(NULL);

		// Constant buffers
		pd3dDeviceContext->VSSetConstantBuffers(0, 14, pBuffers);
		pd3dDeviceContext->HSSetConstantBuffers(0, 14, pBuffers);
		pd3dDeviceContext->DSSetConstantBuffers(0, 14, pBuffers);
		pd3dDeviceContext->GSSetConstantBuffers(0, 14, pBuffers);
		pd3dDeviceContext->PSSetConstantBuffers(0, 14, pBuffers);

		// Resources
		pd3dDeviceContext->VSSetShaderResources(0, 16, pSRVs);
		pd3dDeviceContext->HSSetShaderResources(0, 16, pSRVs);
		pd3dDeviceContext->DSSetShaderResources(0, 16, pSRVs);
		pd3dDeviceContext->GSSetShaderResources(0, 16, pSRVs);
		pd3dDeviceContext->PSSetShaderResources(0, 16, pSRVs);

		// Samplers
		pd3dDeviceContext->VSSetSamplers(0, 16, pSamplers);
		pd3dDeviceContext->HSSetSamplers(0, 16, pSamplers);
		pd3dDeviceContext->DSSetSamplers(0, 16, pSamplers);
		pd3dDeviceContext->GSSetSamplers(0, 16, pSamplers);
		pd3dDeviceContext->PSSetSamplers(0, 16, pSamplers);

		// Reder targets
		pd3dDeviceContext->OMSetRenderTargets(8, pRTVs, pDSV);

		// States
		FLOAT blendFactor[4] = { 0, 0, 0, 0 };
		pd3dDeviceContext->OMSetBlendState(NULL, blendFactor, 0xFFFFFFFF);
		pd3dDeviceContext->OMSetDepthStencilState(NULL, 0);
		pd3dDeviceContext->RSSetState(NULL);
	}

	HRESULT CompileShaderFromFile(const WCHAR* szFileName,
		LPCSTR szEntryPoint,
		LPCSTR szShaderModel,
		ID3DBlob**	ppBlobOut)
	{
		HRESULT hr = S_OK;

		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
		dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
		ID3DBlob*	pErrorBlob = nullptr;
		hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
			dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
		if (FAILED(hr))
		{
			if (pErrorBlob != nullptr)
			{
				OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			}
		}
		SAFE_RELEASE(pErrorBlob);

		return hr;
	}

	ID3D11VertexShader*	LoadVertexShaderFile(ID3D11Device* pd3dDevice,
		const void* pShaderFileData,
		ID3DBlob**	ppBlobOut,
		const char*	pFunctionName,
		bool	bBinary)
	{
		HRESULT hr = S_OK;
		ID3D11VertexShader* pVertexShader = nullptr;
		ID3DBlob* pBlob = nullptr;
		DWORD dwSize = 0;
		LPVOID lpData = nullptr;

		if (bBinary == false)
		{
			if (pFunctionName == 0)
			{
				hr = CompileShaderFromFile((TCHAR*)pShaderFileData, "VS", "vs_5_0", &pBlob);
			}
			else
			{
				hr = CompileShaderFromFile((TCHAR*)pShaderFileData, pFunctionName, "vs_5_0", &pBlob);
			}

			if (FAILED(hr))
			{
				return nullptr;
			}
		}
		else
		{
			pBlob = *ppBlobOut;
			if (pBlob == nullptr) return nullptr;
		}

		dwSize = (DWORD)pBlob->GetBufferSize();
		lpData = pBlob->GetBufferPointer();

		hr = pd3dDevice->CreateVertexShader(lpData, dwSize, NULL, &pVertexShader);

		if (FAILED(hr))
		{
			pBlob->Release();
			return nullptr;
		}

		if (ppBlobOut == nullptr)	pBlob->Release();
		else *ppBlobOut = pBlob;

		return pVertexShader;
	}

	ID3D11PixelShader*	LoadPixelShaderFile(ID3D11Device* pd3dDevice,
		const void* pShaderFileData,
		const char *pFunctionName,
		bool bBinary, ID3DBlob** pRetBlob)
	{
		HRESULT hr = S_OK;
		ID3D11PixelShader* pPixelShader = nullptr;
		ID3DBlob* pBlob = nullptr;
		DWORD dwSize = 0;
		LPCVOID lpData = nullptr;

		if (bBinary == false)
		{
			if (pFunctionName == 0)
			{
				hr = CompileShaderFromFile((TCHAR*)pShaderFileData, "PS", "ps_5_0", &pBlob);
			}
			else
			{
				hr = CompileShaderFromFile((TCHAR*)pShaderFileData, pFunctionName, "ps_5_0", &pBlob);
			}

			if (FAILED(hr))
			{
				return nullptr;
			}

			dwSize = (DWORD)pBlob->GetBufferSize();
			lpData = pBlob->GetBufferPointer();
		}
		else
		{
			dwSize = sizeof(pShaderFileData);
			lpData = pShaderFileData;
		}



		hr = pd3dDevice->CreatePixelShader(lpData, dwSize, NULL, &pPixelShader);

		if (FAILED(hr))
		{
			pBlob->Release();
			return nullptr;
		}

		if (pRetBlob == nullptr)	pBlob->Release();
		else *pRetBlob = pBlob;

		return pPixelShader;
	}

	ID3D11GeometryShader*	LoadGeometryShaderFile(ID3D11Device* pd3dDevice,
		const void* pShaderFileData,
		ID3DBlob**	ppBlobOut,
		const char*	pFunctionName,
		bool	bBinary)
	{
		HRESULT hr = S_OK;
		ID3D11GeometryShader* pGeometryShader;
		ID3DBlob* pBlob = nullptr;
		DWORD dwSize = 0;
		LPCVOID lpData = nullptr;

		if (bBinary == false)
		{
			if (pFunctionName == 0)
			{
				hr = CompileShaderFromFile((TCHAR*)pShaderFileData, "GS", "gs_5_0", &pBlob);
			}
			else
			{
				hr = CompileShaderFromFile((TCHAR*)pShaderFileData, pFunctionName, "gs_5_0", &pBlob);
			}

			if (FAILED(hr))
			{
				return nullptr;
			}

			dwSize = (DWORD)pBlob->GetBufferSize();
			lpData = pBlob->GetBufferPointer();
		}
		else
		{
			dwSize = sizeof(pShaderFileData);
			lpData = pShaderFileData;
		}



		hr = pd3dDevice->CreateGeometryShader(lpData, dwSize, NULL, &pGeometryShader);

		if (FAILED(hr))
		{
			pBlob->Release();
			return nullptr;
		}

		if (ppBlobOut == nullptr)	pBlob->Release();
		else *ppBlobOut = pBlob;

		return pGeometryShader;
	}

	ID3D11HullShader*	LoadHullShaderFile(ID3D11Device* pd3dDevice,
		const void* pLoadShaderFile,
		ID3DBlob** ppBlobOut, const char* pFunctionName
		, bool bBinary)
	{
		HRESULT hr = S_OK;
		ID3D11HullShader* pHullShader;
		ID3DBlob* pBlob = nullptr;
		DWORD dwSize = 0;
		LPCVOID lpData = nullptr;

		if (bBinary == false)
		{
			if (pFunctionName == 0)
			{
				hr = CompileShaderFromFile((TCHAR*)pLoadShaderFile, "HS", "hs_5_0", &pBlob);
			}
			else
			{
				hr = CompileShaderFromFile((TCHAR*)pLoadShaderFile, pFunctionName, "hs_5_0", &pBlob);
			}

			if (FAILED(hr))
			{
				return nullptr;
			}

			dwSize = (DWORD)pBlob->GetBufferSize();
			lpData = pBlob->GetBufferPointer();
		}
		else
		{
			dwSize = sizeof(pLoadShaderFile);
			lpData = pLoadShaderFile;
		}



		hr = pd3dDevice->CreateHullShader(lpData, dwSize, NULL, &pHullShader);

		if (FAILED(hr))
		{
			pBlob->Release();
			return nullptr;
		}

		if (ppBlobOut == nullptr)	pBlob->Release();
		else *ppBlobOut = pBlob;

		return pHullShader;
	}

	ID3D11DomainShader*	LoadDomainShaderFile(ID3D11Device* pd3dDevice,
		const void* pShaderFileData,
		ID3DBlob**	ppBlobOut,
		const char*	pFunctionName,
		bool	bBinary)
	{
		HRESULT hr = S_OK;
		ID3D11DomainShader* pDomainShader;
		ID3DBlob* pBlob = nullptr;
		DWORD dwSize = 0;
		LPCVOID lpData = nullptr;

		if (bBinary == false)
		{
			if (pFunctionName == 0)
			{
				hr = CompileShaderFromFile((TCHAR*)pShaderFileData, "DS", "ds_5_0", &pBlob);
			}
			else
			{
				hr = CompileShaderFromFile((TCHAR*)pShaderFileData, pFunctionName, "ds_5_0", &pBlob);
			}

			if (FAILED(hr))
			{
				return nullptr;
			}

			dwSize = (DWORD)pBlob->GetBufferSize();
			lpData = pBlob->GetBufferPointer();
		}
		else
		{
			dwSize = sizeof(pShaderFileData);
			lpData = pShaderFileData;
		}



		hr = pd3dDevice->CreateDomainShader(lpData, dwSize, NULL, &pDomainShader);

		if (FAILED(hr))
		{
			pBlob->Release();
			return nullptr;
		}

		if (ppBlobOut == nullptr)	pBlob->Release();
		else *ppBlobOut = pBlob;

		return pDomainShader;
	}

	ID3D11ComputeShader*	LoadComputeShaderFile(ID3D11Device* pd3dDevice,
		const void* pShaderFileData,
		ID3DBlob**	ppBlobOut,
		const char*	pFunctionName,
		bool	bBinary)
	{
		HRESULT hr = S_OK;
		ID3D11ComputeShader* pComputShader;
		ID3DBlob* pBlob = nullptr;
		DWORD dwSize = 0;
		LPCVOID lpData = nullptr;

		if (bBinary == false)
		{
			if (pFunctionName == 0)
			{
				hr = CompileShaderFromFile((TCHAR*)pShaderFileData, "CS", "cs_5_0", &pBlob);
			}
			else
			{
				hr = CompileShaderFromFile((TCHAR*)pShaderFileData, pFunctionName, "cs_5_0", &pBlob);
			}

			if (FAILED(hr))
			{
				return nullptr;
			}

			dwSize = (DWORD)pBlob->GetBufferSize();
			lpData = pBlob->GetBufferPointer();
		}
		else
		{
			dwSize = sizeof(pShaderFileData);
			lpData = pShaderFileData;
		}



		hr = pd3dDevice->CreateComputeShader(lpData, dwSize, NULL, &pComputShader);

		if (FAILED(hr))
		{
			pBlob->Release();
			return nullptr;
		}

		if (ppBlobOut == nullptr)	pBlob->Release();
		else *ppBlobOut = pBlob;

		return pComputShader;
	}

	ID3D11InputLayout* CreateInputlayout(ID3D11Device* pd3dDevice,
		DWORD dwSize,
		LPCVOID lpData,
		D3D11_INPUT_ELEMENT_DESC* layout,
		UINT numElements)
	{
		HRESULT hr = S_OK;
		ID3D11InputLayout* pInputLayout = nullptr;
		hr = pd3dDevice->CreateInputLayout(layout, numElements, lpData, dwSize, &pInputLayout);

		if (FAILED(hr))
		{
			HRESULT_FAILE_MESSAGE(hr);
			return nullptr;
		}

		return pInputLayout;
	}

	ID3D11Buffer* CreateVertexBuffer(
		ID3D11Device*	pd3dDevice,
		void *vertices,
		UINT iNumVertex,
		UINT iVertexSize,
		bool bDynamic)
	{
		HRESULT hr = S_OK;
		ID3D11Buffer* pBuffer = nullptr;
		D3D11_BUFFER_DESC bd;

		ZeroMemory(&bd, sizeof(bd));

		if (bDynamic)
		{
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else
		{
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0;
		}

		bd.ByteWidth = iVertexSize * iNumVertex;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = vertices;
		if (vertices != nullptr)
		{
			hr = pd3dDevice->CreateBuffer(&bd, &InitData, &pBuffer);

			if (FAILED(hr))
			{
				HRESULT_FAILE_MESSAGE(hr);
				return nullptr;
			}
		}
		else
		{
			hr = pd3dDevice->CreateBuffer(&bd, NULL, &pBuffer);
			if (FAILED(hr))
			{
				HRESULT_FAILE_MESSAGE(hr);
				return nullptr;
			}
		}
		return pBuffer;
	}

	ID3D11Buffer* CreateIndexBuffer(ID3D11Device*  pd3dDevice, void *indices, UINT iNumIndex,
		UINT iSize, bool bDynamic)
	{
		HRESULT hr = S_OK;
		ID3D11Buffer* pBuffer = nullptr;
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		if (bDynamic)
		{
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else
		{
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0;
		}
		bd.ByteWidth = iSize * iNumIndex;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

		InitData.pSysMem = indices;

		if (indices != nullptr)
		{
			hr = pd3dDevice->CreateBuffer(&bd, &InitData, &pBuffer);
			if (FAILED(hr))
			{
				HRESULT_FAILE_MESSAGE(hr);
				return nullptr;
			}
		}
		else
		{
			hr = pd3dDevice->CreateBuffer(&bd, NULL, &pBuffer);
			if (FAILED(hr))
			{
				HRESULT_FAILE_MESSAGE(hr);
				return nullptr;
			}
		}
		return pBuffer;
	}

	ID3D11Buffer* CreateConstantBuffer(ID3D11Device*  pd3dDevice, void *data, UINT iNumIndex,
		UINT iSize, bool bDynamic)
	{
		HRESULT hr = S_OK;
		ID3D11Buffer* pBuffer = nullptr;
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		if (bDynamic)
		{
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else
		{
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0;
		}
		bd.ByteWidth = iSize * iNumIndex;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		InitData.pSysMem = data;
		if (data != nullptr)
		{
			hr = pd3dDevice->CreateBuffer(&bd, &InitData, &pBuffer);
			if (FAILED(hr))
			{
				HRESULT_FAILE_MESSAGE(hr);
				return nullptr;
			}
		}
		else
		{
			hr = pd3dDevice->CreateBuffer(&bd, NULL, &pBuffer);
			if (FAILED(hr))
			{
				HRESULT_FAILE_MESSAGE(hr);
				return nullptr;
			}
		}
		return pBuffer;
	}

	ID3D11ShaderResourceView*	CreateShaderResourceView(ID3D11Device* pDevice, const TCHAR* strFilePath)
	{
		HRESULT hr = S_OK;
		if (strFilePath == NULL) return nullptr;
		ID3D11ShaderResourceView* pSRV = nullptr;

		hr = D3DX11CreateShaderResourceViewFromFile(pDevice, strFilePath, NULL, NULL, &pSRV, NULL);

		if (FAILED(hr))
		{
			HRESULT_FAILE_MESSAGE(hr);
			return nullptr;
		}
		return pSRV;
	}

	ID3D11DepthStencilView*		CreateDepthStencilView(ID3D11Device* pDevice, DWORD dwWidth, DWORD dwHeight)
	{
		HRESULT hr = S_OK;
		ID3D11DepthStencilView* pDSV = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pDSTexture = nullptr;
		D3D11_TEXTURE2D_DESC DescDepth;

		DescDepth.Width = dwWidth;
		DescDepth.Height = dwHeight;
		DescDepth.MipLevels = 1;
		DescDepth.ArraySize = 1;
		DescDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DescDepth.SampleDesc.Count = 1;
		DescDepth.SampleDesc.Quality = 0;
		DescDepth.Usage = D3D11_USAGE_DEFAULT;
		DescDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		DescDepth.CPUAccessFlags = 0;
		DescDepth.MiscFlags = 0;

		hr = pDevice->CreateTexture2D(&DescDepth, nullptr, &pDSTexture);

		if (FAILED(hr))
		{
			return nullptr;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(dsvd));
		dsvd.Format = DescDepth.Format;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Texture2D.MipSlice = 0;

		hr = pDevice->CreateDepthStencilView(pDSTexture.Get(), &dsvd, &pDSV);
		if (FAILED(hr))
		{
			return nullptr;
		}
		return pDSV;
	}

	SDxHelperEX::SDxHelperEX()
	{
		g_pVertexBuffer = nullptr;
		g_pIndexBuffer = nullptr;
		g_pConstantBuffer = nullptr;
		g_pVertexShader = nullptr;
		g_pPixelShader = nullptr;
		g_pGeometryShader = nullptr;
		g_pInputlayout = nullptr;
		g_pVSBlob = nullptr;
		m_iPrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		m_iCullMode = 1;
		m_iSamplerMode = 0;
		m_iNumVertex = 0;
		m_iNumIndex = 0;
		m_iVertexSize = 0;
		m_iPreVB = 0;
		m_iPreIB = 0;
		m_iIndexSize = sizeof(DWORD);
	}

	SDxHelperEX::~SDxHelperEX()
	{

	}

	void SDxHelperEX::PreRender(ID3D11DeviceContext* pContext, UINT iVertexSize)
	{
		pContext->IASetInputLayout(g_pInputlayout.Get());
		UINT stride = iVertexSize;
		UINT offset = 0;
		if (iVertexSize == 0) stride = m_iVertexSize;

		pContext->IASetVertexBuffers(0, 1, g_pVertexBuffer.GetAddressOf(), &stride, &offset);
		pContext->IASetIndexBuffer(g_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		pContext->VSSetConstantBuffers(0, 1, g_pConstantBuffer.GetAddressOf());
		pContext->VSSetShader(g_pVertexShader.Get(), nullptr, 0);
		pContext->PSSetShader(g_pPixelShader.Get(), nullptr, 0);
		pContext->GSSetShader(g_pGeometryShader.Get(), nullptr, 0);
		pContext->HSSetShader(g_pHullShader.Get(), nullptr, 0);
		pContext->DSSetShader(g_pDomainShader.Get(), nullptr, 0);
		pContext->PSSetShaderResources(0, 1, &g_pTextureSRV);
	}

	void SDxHelperEX::PostRender(ID3D11DeviceContext* pContext, UINT iCount)
	{
		if (iCount == 0) iCount = m_iNumIndex;
		pContext->DrawIndexed(iCount, 0, 0);
	}

	bool SDxHelperEX::Render(ID3D11DeviceContext*    pContext, UINT iVertexSize, UINT iCount)
	{
		PreRender(pContext, iVertexSize);
		PostRender(pContext, iCount);
		return true;
	}
}

