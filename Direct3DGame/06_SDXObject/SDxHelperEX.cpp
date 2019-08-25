#include "SDxHelperEX.h"

namespace DXGame
{

	HRESULT CompilerShaderFromFile(const TCHAR* sFileName,
		const char* szEntryPoint, const char* szShaderModel,
		ID3DBlob** ppBlobOut)
	{
		HRESULT hr = S_OK;
		ID3DBlob* pErrorMsgs = nullptr;

		hr = D3DX11CompileFromFile(sFileName, nullptr, nullptr, szEntryPoint, szShaderModel, NULL, NULL, nullptr,
			ppBlobOut, &pErrorMsgs, nullptr);
		if (FAILED(hr) && pErrorMsgs)
		{
			OutputDebugStringA((char*)pErrorMsgs->GetBufferPointer());
			pErrorMsgs->Release();
		}

		return hr;
	}

	ID3D11VertexShader* LoadVertexShaderFile(ID3D11Device* pDevice, const void* pShaderFileData,
		ID3DBlob** ppBlobOut, const char* pFunctionName,
		bool bBinary)
	{
		HRESULT hr;
		ID3DBlob* pBlob = nullptr;
		ID3D11VertexShader* pVertexShader;
		DWORD dwSize = 0;
		LPCVOID lpData = 0;

		if (bBinary == false)
		{
			if (pFunctionName == nullptr)
			{
				hr = CompilerShaderFromFile((TCHAR*)pShaderFileData, "VS", "vs_5_0", &pBlob);
				if (FAILED(hr)) return nullptr;
			}
			else
			{
				hr = CompilerShaderFromFile((TCHAR*)pShaderFileData, pFunctionName, "vs_5_0", &pBlob);
				if (FAILED(hr)) return nullptr;
			}
		}
		else
		{
			pBlob = *ppBlobOut;
			if (pBlob == nullptr) return nullptr;
		}

		dwSize = pBlob->GetBufferSize();
		lpData = pBlob->GetBufferPointer();

		hr = pDevice->CreateVertexShader(lpData, dwSize, NULL, &pVertexShader);
		if (FAILED(hr))
		{
			pBlob->Release();
			return nullptr;
		}
		
		if (ppBlobOut == nullptr)
		{
			pBlob->Release();
			return nullptr;
		}
		else
		{
			*ppBlobOut = pBlob;
		}

		return pVertexShader;
	}

	ID3D11PixelShader* LoadPixelShaderFile(ID3D11Device* pDevice, const void* pShaderFileData,
		ID3DBlob** ppBlobOut, const char* pFunctionName,
		bool bBinary)
	{
		HRESULT hr;
		ID3DBlob* pBlob = nullptr;
		ID3D11PixelShader* pPixelShader;
		DWORD dwSize = 0;
		LPCVOID lpData = 0;

		if (bBinary == false)
		{
			if (pFunctionName == nullptr)
			{
				hr = CompilerShaderFromFile((TCHAR*)pShaderFileData, "PS", "ps_5_0", &pBlob);
				if (FAILED(hr)) return nullptr;
			}
			else
			{
				hr = CompilerShaderFromFile((TCHAR*)pShaderFileData, pFunctionName, "ps_5_0", &pBlob);
				if (FAILED(hr)) return nullptr;
			}
		}
		else
		{
			pBlob = *ppBlobOut;
			if (pBlob == nullptr) return nullptr;
		}

		dwSize = pBlob->GetBufferSize();
		lpData = pBlob->GetBufferPointer();

		hr = pDevice->CreatePixelShader(lpData, dwSize, NULL, &pPixelShader);
		if (FAILED(hr))
		{
			pBlob->Release();
			return nullptr;
		}

		if (ppBlobOut == nullptr)
		{
			pBlob->Release();
		}
		else
		{
			*ppBlobOut = pBlob;
		}

		return pPixelShader;
	}

	ID3D11InputLayout* CreateInputLayout(ID3D11Device* pDevice, DWORD dwSize,
		const void* lpData, D3D11_INPUT_ELEMENT_DESC* layout,
		UINT numElements)
	{
		HRESULT hr;
		ID3D11InputLayout* pInputLayout = nullptr;

		hr = pDevice->CreateInputLayout(layout, numElements, lpData, dwSize, &pInputLayout);
		if (FAILED(hr)) return nullptr;

		return pInputLayout;
	}

	ID3D11Buffer* CreateVertexBuffer(ID3D11Device* pDevice, void* pData,
		UINT iNumCount, UINT iSize,
		bool bDynamic)
	{
		HRESULT hr;
		ID3D11Buffer* pBuffer = nullptr;
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.ByteWidth = iSize * iNumCount;
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
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
		sd.pSysMem = pData;

		if (pData != nullptr)
		{
			hr = pDevice->CreateBuffer(&bd, &sd, &pBuffer);
			if (FAILED(hr)) return nullptr;
		}
		else
		{
			hr = pDevice->CreateBuffer(&bd, NULL, &pBuffer);
			if (FAILED(hr)) return nullptr;
		}
		return pBuffer;
	}

	ID3D11Buffer* CreateIndexBuffer(ID3D11Device* pDevice, void* pData,
		UINT iNumCount, UINT iSize,
		bool bDynamic)
	{
		HRESULT hr;
		ID3D11Buffer* pBuffer = nullptr;
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.ByteWidth = iSize * iNumCount;
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
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
		sd.pSysMem = pData;

		if (pData != nullptr)
		{
			hr = pDevice->CreateBuffer(&bd, &sd, &pBuffer);
			if (FAILED(hr)) return nullptr;
		}
		else
		{
			hr = pDevice->CreateBuffer(&bd, NULL, &pBuffer);
			if (FAILED(hr)) return nullptr;
		}
		return pBuffer;
	}

	ID3D11Buffer* CreateConstantBuffer(ID3D11Device* pDevice, void* pData,
		UINT iNumCount, UINT iSize,
		bool bDynamic)
	{
		HRESULT hr;
		ID3D11Buffer* pBuffer = nullptr;
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.ByteWidth = iSize * iNumCount;
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
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));
		sd.pSysMem = pData;

		if (pData != nullptr)
		{
			hr = pDevice->CreateBuffer(&bd, &sd, &pBuffer);
			if (FAILED(hr)) return nullptr;
		}
		else
		{
			hr = pDevice->CreateBuffer(&bd, NULL, &pBuffer);
			if (FAILED(hr)) return nullptr;
		}
		return pBuffer;
	}
	void SDxHelperEX::PreRender(ID3D11DeviceContext* pContext, UINT iSize)
	{
		UINT stride = iSize;
		UINT offset = 0;

		pContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
		pContext->PSSetShaderResources(0, 1, m_pSRV.GetAddressOf());
		pContext->IASetInputLayout(m_pInputLayout.Get());
		pContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
		pContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
		pContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
		pContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	}
	void SDxHelperEX::PostRender(ID3D11DeviceContext* pContext, UINT iCount)
	{
		if (iCount == 0) iCount = m_iNumIndex;
		if (iCount != 0)
			pContext->DrawIndexed(iCount, 0, 0);
		else
			pContext->Draw(m_iNumVertex, 0);
	}
	bool SDxHelperEX::Render(ID3D11DeviceContext* pContext, UINT iSize, UINT iCount)
	{
		PreRender(pContext, iSize);
		PostRender(pContext, iCount);
		return true;
	}
	SDxHelperEX::SDxHelperEX()
	{
	}


	SDxHelperEX::~SDxHelperEX()
	{
	}
}

