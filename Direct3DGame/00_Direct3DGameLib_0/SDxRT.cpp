#include "SDxRT.h"

namespace DXGame
{
	SDxRT::SDxRT()
	{
		m_pRenderTargetView = nullptr;
		m_pDepthStencilView = nullptr;
		m_pSRV = nullptr;
		m_pTexture = nullptr;
		m_DSFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	}
	void SDxRT::Set(ID3D11Device* pDevice,
		float TopLeftX, float TopLeftY, float Width, float Height,
		float MinDepth, float MaxDepth)
	{
		m_vp.TopLeftX = TopLeftX;
		m_vp.TopLeftY = TopLeftY;
		m_vp.Height = Height;
		m_vp.Width = Width;
		m_vp.MinDepth = MinDepth;
		m_vp.MaxDepth = MaxDepth;
		D3DXMatrixPerspectiveFovLH(&m_matProj, (float)D3DX_PI * 0.25, Width / Height, 0.1f, 1000.0f);
	}
	HRESULT SDxRT::CreateCubeMap(ID3D11Device* pDevice, float fWidth, float fHeight)
	{
		HRESULT hr = S_OK;
		Set(pDevice, 0, 0, fWidth, fHeight, 0.0f, 1.0f);

		// 렌더타겟용 텍스쳐 생성
		D3D11_TEXTURE2D_DESC TexDesc;
		ZeroMemory(&TexDesc, sizeof(D3D11_TEXTURE2D_DESC));
		TexDesc.Width = fWidth;
		TexDesc.Height = fHeight;
		TexDesc.MipLevels = 1;
		TexDesc.SampleDesc.Count = 1;
		TexDesc.ArraySize = 6;
		TexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		TexDesc.Usage = D3D11_USAGE_DEFAULT;
		TexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		TexDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		hr = pDevice->CreateTexture2D(&TexDesc, nullptr, m_pTexture.GetAddressOf());
		HRESULT_FAILED_RETURN(hr);

		// 렌더 타켓용 뷰 생성
		D3D11_RENDER_TARGET_VIEW_DESC RtvDesc;
		ZeroMemory(&RtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		RtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		RtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		RtvDesc.Texture2DArray.ArraySize = 6;
		
		hr = pDevice->CreateRenderTargetView(m_pTexture.Get(), &RtvDesc, m_pRenderTargetView.GetAddressOf());
		HRESULT_FAILED_RETURN(hr);

		// 렌더 타켓용 뷰 생성
		D3D11_SHADER_RESOURCE_VIEW_DESC SrvDesc;
		ZeroMemory(&SrvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		SrvDesc.Format = TexDesc.Format;
		SrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		SrvDesc.Texture2DArray.MipLevels = 1;

		hr = pDevice->CreateShaderResourceView(m_pTexture.Get(), &SrvDesc, m_pSRV.GetAddressOf());
		HRESULT_FAILED_RETURN(hr);

		// 깊이 스텐실 뷰 텍스쳐 생성
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pDSTexture = nullptr;
		D3D11_TEXTURE2D_DESC DSDesc;
		ZeroMemory(&DSDesc, sizeof(D3D11_TEXTURE2D_DESC));
		DSDesc.Width = fWidth;
		DSDesc.Height = fHeight;
		DSDesc.MipLevels = 1;
		DSDesc.ArraySize = 6;
		DSDesc.Format = DXGI_FORMAT_D32_FLOAT;
		DSDesc.SampleDesc.Count = 1;
		DSDesc.SampleDesc.Quality = 0;
		DSDesc.Usage = D3D11_USAGE_DEFAULT;
		DSDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		DSDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		hr = pDevice->CreateTexture2D(&DSDesc, nullptr, pDSTexture.GetAddressOf());
		HRESULT_FAILED_RETURN(hr);

		// 깊이 스텐실 뷰 생성
		D3D11_DEPTH_STENCIL_VIEW_DESC DsvDesc;
		ZeroMemory(&DsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		DsvDesc.Format = DSDesc.Format;
		DsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		DsvDesc.Texture2DArray.ArraySize = 6;

		hr = pDevice->CreateDepthStencilView(m_pTexture.Get(), &DsvDesc, m_pDepthStencilView.GetAddressOf());
		HRESULT_FAILED_RETURN(hr);

		return S_OK;
	}
	HRESULT SDxRT::Create(ID3D11Device* pDevice, float Width, float Height)
	{
		HRESULT hr = S_OK;
		Set(pDevice, 0, 0, Width, Height, 0.0f, 1.0f);

		m_TexDesc.Width = (UINT)Width;
		m_TexDesc.Height = (UINT)Height;
		m_TexDesc.MipLevels = 1;
		m_TexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_TexDesc.SampleDesc.Count = 1;
		m_TexDesc.SampleDesc.Quality = 0;
		m_TexDesc.Usage = D3D11_USAGE_DEFAULT;
		m_TexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		m_TexDesc.CPUAccessFlags = 0;
		m_TexDesc.MiscFlags = 0;
		m_TexDesc.ArraySize = 1;

		hr = pDevice->CreateTexture2D(&m_TexDesc, nullptr, m_pTexture.GetAddressOf());
		HRESULT_FAILED_RETURN(hr);

		hr = pDevice->CreateShaderResourceView(m_pTexture.Get(), nullptr, m_pSRV.GetAddressOf());
		HRESULT_FAILED_RETURN(hr);

		hr = pDevice->CreateRenderTargetView(m_pTexture.Get(), nullptr, m_pRenderTargetView.GetAddressOf());
		HRESULT_FAILED_RETURN(hr);

		// 백버퍼의 크기가 변경되면 반드시 깊이 스텐실 뷰어도 다시 작성되야 한다.
		hr = UpdateDepthStencilView(pDevice, (UINT)Width, (UINT)Height);
		HRESULT_FAILED_RETURN(hr);

		return hr;
	}
	HRESULT SDxRT::UpdateDepthStencilView(ID3D11Device* pDevice, UINT Width, UINT Height)
	{

		HRESULT hr;
		if (m_pDepthStencilView != nullptr)
		{
			m_pDepthStencilView->Release();
		}

		m_vp.Width = Width;
		m_vp.Height = Height;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> pDSTexture = nullptr;
		D3D11_TEXTURE2D_DESC DescDepth;
		DescDepth.Width = Width;
		DescDepth.Height = Height;
		DescDepth.MipLevels = 1;
		DescDepth.ArraySize = 1;
		DescDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DescDepth.SampleDesc.Count = 1;
		DescDepth.SampleDesc.Quality = 0;
		DescDepth.Usage = D3D11_USAGE_DEFAULT;

		// 백 버퍼 깊이 및 스텐실 버퍼 생성
		if (DescDepth.Format == DXGI_FORMAT_D24_UNORM_S8_UINT)
			DescDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		else // 깊이맵 전용 깊이맵 생성
			DescDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		DescDepth.CPUAccessFlags = 0;
		DescDepth.MiscFlags = 0;
		if (FAILED(hr = pDevice->CreateTexture2D(&DescDepth, NULL, &pDSTexture)))
		{
			return hr;
		}

		///// 쉐이더 리소스 생성 : 깊이 맵 쉐도우에서 사용한다. ///
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		switch (DescDepth.Format)
		{
		case DXGI_FORMAT_R32_TYPELESS:
			dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
			srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
			break;
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			break;
		}
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;

		if (FAILED(hr = pDevice->CreateDepthStencilView(pDSTexture.Get(), &dsvDesc, m_pDepthStencilView.GetAddressOf())))
		{
			return hr;
		}

		// 깊이맵 일 경우만 생성한다.
		if (DescDepth.Format == DXGI_FORMAT_R32_TYPELESS)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;
			if (m_pDsvSRV != nullptr) m_pDsvSRV->Release();
			pDevice->CreateShaderResourceView(pDSTexture.Get(), &srvDesc, &m_pDsvSRV);
		}
		m_pDepthStencilView->GetDesc(&m_DepthStencilDesc);
		return hr;
	}

	void SDxRT::Apply(ID3D11DeviceContext* pImmediateContext,
		ID3D11RenderTargetView* pRTV,
		ID3D11DepthStencilView* pDepthStencilView,
		bool bDepthClear, bool bStencilClear)
	{
		assert(pImmediateContext);
		assert(pRTV);
		ID3D11RenderTargetView* pNullRTV = nullptr;
		pImmediateContext->OMSetRenderTargets(1, &pNullRTV, NULL);

		if (pRTV != nullptr)
		{
			if (pDepthStencilView != nullptr) pImmediateContext->OMSetRenderTargets(1, &pRTV, pDepthStencilView);
			else pImmediateContext->OMSetRenderTargets(1, &pRTV, m_pDepthStencilView.Get());
		}
		else
		{
			if(pDepthStencilView != nullptr) pImmediateContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), pDepthStencilView);
			else pImmediateContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());
		}
		if (bDepthClear)
		{
			if (pDepthStencilView != nullptr) pImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0);
			else pImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		}
		if (bStencilClear)
		{
			if (pDepthStencilView != nullptr) pImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_STENCIL, 1.0, 0);
			else pImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
		pImmediateContext->RSSetViewports(1, &m_vp);
	}
	bool SDxRT::Begin(ID3D11DeviceContext* pContext,
		D3DXVECTOR4 vColor,
		bool bTargetClear,
		bool bDepthClear,
		bool bStencilClear)
	{
		m_nViewPorts = 1;
		pContext->RSGetViewports(&m_nViewPorts, m_vpOld);
		pContext->OMGetRenderTargets(1, &m_pOldRTV, &m_pOldDSV);

		ID3D11RenderTargetView* pNullRTV = nullptr;
		pContext->OMSetRenderTargets(1, &pNullRTV, nullptr);
		pContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());
		
		Clear(pContext, vColor, bTargetClear, bDepthClear, bStencilClear);
		pContext->RSSetViewports(1, &m_vp);

		return true;
	}
	bool SDxRT::Clear(ID3D11DeviceContext* pContext,
		D3DXVECTOR4 vColor,
		bool bTargetClear,
		bool bDepthClear,
		bool bStencilClear)
	{
		if (bTargetClear)
		{
			pContext->ClearRenderTargetView(m_pRenderTargetView.Get(), vColor);
		}
		if (bDepthClear)
		{
			pContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0, 0);
		}
		if (bStencilClear)
		{
			pContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_STENCIL, 1.0, 0);
		}
		pContext->RSSetViewports(1, &m_vp);
		return true;
	}
	bool SDxRT::End(ID3D11DeviceContext* pContext)
	{
		pContext->RSSetViewports(m_nViewPorts, m_vpOld);
		pContext->OMSetRenderTargets(1, &m_pOldRTV, m_pOldDSV);
		SAFE_RELEASE(m_pOldRTV);
		SAFE_RELEASE(m_pOldDSV);

		return true;
	}
	HRESULT SDxRT::SaveFile(ID3D11DeviceContext* pContext, T_STR name)
	{
		HRESULT hr = S_OK;
		// 렌더타겟뷰에서 텍스쳐 파일로 저장
		if (m_pTexture != nullptr)
		{
			// 랜더타겟 텍스쳐를 텍스쳐 파일로 저장
			hr = D3DX11SaveTextureToFile(pContext, m_pTexture.Get(), D3DX11_IFF_DDS, name.c_str());
			HRESULT_FAILED_RETURN(hr);
		}
		else if (m_pSRV != nullptr)
		{
			ID3D11Resource*	resource;
			m_pSRV.Get()->GetResource(&resource);

			ID3D11Texture2D* texture;
			hr = resource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&texture));
			HRESULT_FAILED_RETURN(hr);

			hr = D3DX11SaveTextureToFile(pContext, texture, D3DX11_IFF_DDS, name.c_str());
			HRESULT_FAILED_RETURN(hr);
			
			texture->Release();
		}
		return hr;
	}
	HRESULT SDxRT::SaveBackBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, T_STR name)
	{
		HRESULT hr = S_OK;
		ID3D11Resource* backbufferRes;
		m_pRenderTargetView.Get()->GetResource(&backbufferRes);
		D3D11_TEXTURE2D_DESC desc;
		m_pTexture.Get()->GetDesc(&desc);

		ID3D11Texture2D* texture;
		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.ArraySize = 1;
		texDesc.BindFlags = 0;
		texDesc.CPUAccessFlags = desc.CPUAccessFlags;
		texDesc.Format = desc.Format;
		texDesc.Height = desc.Height;
		texDesc.Width = desc.Width;
		texDesc.MipLevels = 1;
		texDesc.MiscFlags = 0;
		texDesc.SampleDesc = desc.SampleDesc;
		texDesc.Usage = D3D11_USAGE_DEFAULT;

		hr = pDevice->CreateTexture2D(&texDesc, 0, &texture);
		HRESULT_FAILED_RETURN(hr);
		pContext->CopyResource(texture, backbufferRes);

		hr = D3DX11SaveTextureToFile(pContext, texture, D3DX11_IFF_BMP, name.c_str());
		HRESULT_FAILED_RETURN(hr);
		texture->Release();
		backbufferRes->Release();

		return hr;
	}
	SDxRT::~SDxRT()
	{
	}
}
