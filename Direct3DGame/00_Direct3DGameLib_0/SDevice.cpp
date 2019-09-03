#include "SDevice.h"

#pragma region Initialize

HRESULT SDevice::InitDevice(HWND hWnd, UINT iWidth, UINT iHeight, BOOL isFullScreen)
{
	HRESULT hr = S_OK;

	if (FAILED(hr = CreateDevice()))
	{
		MessageBox(0, L"Fail Create Device", L"Fatal error", MB_OK);
		return hr;
	}

	if (FAILED(hr = CreateSwapChain(hWnd, iWidth, iHeight, isFullScreen)))
	{
		MessageBox(0, L"Fail Create SwapChain", L"Fatal error", MB_OK);
		return hr;
	}

	if (FAILED(hr = SetRenderTargetView()))
	{
		MessageBox(0, L"Fail Create RenderTargetView", L"Fatal error", MB_OK);
		return hr;
	}

	if (FAILED(hr = SetViewPort()))
	{
		MessageBox(0, L"Fail Set ViewPort", L"Fatal error", MB_OK);
		return hr;
	}

	return hr;
}

HRESULT SDevice::CreateDevice()
{
	HRESULT hr = S_OK;

	UINT CreateDeviceFlag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	CreateDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

	// Ư�� ���� �迭
	D3D_FEATURE_LEVEL d3dFeatureLevelArray[]
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	UINT nd3dFeatureLevelArraySize = ARRAYSIZE(d3dFeatureLevelArray);

	// ����̹� Ÿ�� �迭
	D3D_DRIVER_TYPE d3dDriverTypeArray[]
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};

	UINT nd3dDriverTypeArraySize = ARRAYSIZE(d3dDriverTypeArray);

	// ����̹� Ÿ�� ������ŭ �ݺ��ϸ鼭 ����̽��� �����ϴ� �Լ��� ����

	for (UINT nDeviceNum = 0; nDeviceNum < nd3dDriverTypeArraySize; nDeviceNum++)
	{
		m_DriverType = d3dDriverTypeArray[nDeviceNum];
		hr = D3D11CreateDevice(nullptr, m_DriverType, NULL, CreateDeviceFlag,
			d3dFeatureLevelArray, nd3dFeatureLevelArraySize, D3D11_SDK_VERSION,
			&m_pD3DDevice, &m_FeatureLevel, &m_pImmediateContext);

		if (SUCCEEDED(hr))
		{
			break;
		}
	}

	if (FAILED(hr))
	{
		return hr;
	}

	return CreateGIFactory();
}

HRESULT	SDevice::CreateGIFactory()
{
	if (!m_pD3DDevice)
	{
		return E_FAIL;
	}

	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&m_pDxgiFactory);
	return hr;
}

HRESULT	SDevice::CreateSwapChain(HWND hWnd, UINT iWidth, UINT iHeight, BOOL IsFullScreen)
{
	HRESULT hr = S_OK;
	
	SetFullScreenModeFlag(IsFullScreen);
	if (m_pDxgiFactory == NULL)
	{
		return S_FALSE;
	}

	// ����ü�� ��ũ���� ����
	ZeroMemory(&m_SwapChainDesc, sizeof(m_SwapChainDesc));
	
	m_SwapChainDesc.BufferCount = 1;
	m_SwapChainDesc.BufferDesc.Height = iHeight;
	m_SwapChainDesc.BufferDesc.Width = iWidth;
	m_SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	m_SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	m_SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_SwapChainDesc.OutputWindow = hWnd;
	m_SwapChainDesc.SampleDesc.Count = 1;
	m_SwapChainDesc.Windowed = !IsFullScreen;
	m_SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// ���丮���� ����ü�� ����
	hr = m_pDxgiFactory->CreateSwapChain(m_pD3DDevice, &m_SwapChainDesc, &m_pDxgiSwapChain);

	return hr;
}

HRESULT SDevice::SetRenderTargetView()
{
	HRESULT hr = S_OK;

	// 2D �ؽ��� �������̽� �ӽ� �����ͷ� ����ü������ ���� ���۸� ��´�
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = m_pDxgiSwapChain->GetBuffer(NULL, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);

	if (FAILED(hr))
	{
		return hr;
	}
	
	// ���� ���۷� ���� Ÿ�� �並 �����ϰ� �ӽ÷� ���� �������� ������ �����Ѵ�.
	hr = m_pD3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
	pBackBuffer->Release();

	if (FAILED(hr))
	{
		return hr;
	}

	// ���ؽ�Ʈ�� ���� Ÿ�� �並 �����Ѵ�.
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);
	return hr;
}

HRESULT SDevice::SetViewPort()
{
	HRESULT hr = S_OK;

	// ����ü�� ��ũ���͸� ��´�.
	DXGI_SWAP_CHAIN_DESC Desc;
	m_pDxgiSwapChain->GetDesc(&Desc);

	// ����ü�� ��ũ���Ϳ� �ִ� ���� ���� ���̷� ����Ʈ�� �ʱ�ȭ ��Ų��.

	m_ViewPort.Width = static_cast<float>(Desc.BufferDesc.Width);
	m_ViewPort.Height = static_cast<float>(Desc.BufferDesc.Height);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;

	//����̽� ���ؽ�Ʈ�� ����Ʈ�� �����Ѵ�
	m_pImmediateContext->RSSetViewports(1, &m_ViewPort);

	return hr;
}
HRESULT SDevice::DeleteDxResource()
{
	return S_OK;
}
HRESULT SDevice::CreateDxResource()
{
	return S_OK;
}
#pragma endregion

bool SDevice::CleanupDevice()
{
	// �ʱ� Ǯ��ũ�� �����쿡�� �������α׷��� ������ ��쿡�� ������ ��ȯ �Ŀ�
	// ��ü���� �Ҹ��ؾ� �Ѵ�.(�޸� ������ ���� �� �ִ�.)
	m_pDxgiSwapChain->SetFullscreenState(false, nullptr);


	if (FAILED(DeleteDxResource()))
	{
		return false;
	}

	if (m_pImmediateContext)
	{
		m_pImmediateContext->ClearState();
	}
	if (m_pRenderTargetView)
	{
		m_pRenderTargetView->Release();
	}
	if (m_pDxgiSwapChain)
	{
		m_pDxgiSwapChain->Release();
	}
	if (m_pImmediateContext)
	{
		m_pImmediateContext->Release();
	}
	if (m_pD3DDevice)
	{
		m_pD3DDevice->Release();
	}
	if (m_pDxgiFactory)
	{
		m_pDxgiFactory->Release();
	}

	m_pD3DDevice = nullptr;
	m_pImmediateContext = nullptr;
	m_pRenderTargetView = nullptr;
	m_pDxgiSwapChain = nullptr;
	m_pDxgiFactory = nullptr;

	return true;
}

HRESULT	SDevice::ReSizeDevice(UINT iWidth, UINT iHeight)
{
	if ( m_pImmediateContext == nullptr)
	{
		return true;
	}

	HRESULT hr = S_OK;

	if (FAILED(hr = DeleteDxResource()))
	{
		return hr;
	}

	//=============================================================
	// ����Ÿ�ٰ� ���̽��ٽ� ���۸� �����Ѵ�.
	//=============================================================

	m_pImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);
	SAFE_RELEASE(m_pRenderTargetView);

	//=============================================================
	// ����ü�ο� ��ũ���͸� �� ������ ũ�⸦ �����Ѵ�.
	//=============================================================

	DXGI_SWAP_CHAIN_DESC	CurrentSD;
	m_pDxgiSwapChain->GetDesc(&CurrentSD);
	m_pDxgiSwapChain->ResizeBuffers(CurrentSD.BufferCount, iWidth, iHeight, CurrentSD.BufferDesc.Format, CurrentSD.Flags);


	// ����Ÿ�ٺ� ���� �� �����Ѵ�.

	if (FAILED(hr = SetRenderTargetView()))
	{
		return hr;
	}

	if (FAILED(hr = SetViewPort()))
	{
		return hr;
	}

	if (FAILED(hr = CreateDxResource()))
	{
		return hr;
	}
	return hr;

}

HRESULT SDevice::UpdateDepthStencilView(ID3D11Device* pDevice, UINT Width, UINT Height)
{
	HRESULT hr;
	if (m_pDepthStencilView != nullptr)
	{
		m_pDepthStencilView->Release();
	}

	m_ViewPort.Width = Width;
	m_ViewPort.Height = Height;

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

	// �� ���� ���� �� ���ٽ� ���� ����
	if (DescDepth.Format == DXGI_FORMAT_D24_UNORM_S8_UINT)
		DescDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	else // ���̸� ���� ���̸� ����
		DescDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	DescDepth.CPUAccessFlags = 0;
	DescDepth.MiscFlags = 0;
	if (FAILED(hr = pDevice->CreateTexture2D(&DescDepth, NULL, &pDSTexture)))
	{
		return hr;
	}

	///// ���̴� ���ҽ� ���� : ���� �� �����쿡�� ����Ѵ�. ///
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
	//pDevice->CreateDepthStencilView(m_pTexture, &dsvDesc, &m_pDSV);
	//D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	//ZeroMemory(&dsvd, sizeof(dsvd));
	//dsvd.Format = DescDepth.Format;
	//dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//dsvd.Texture2D.MipSlice = 0;

	if (FAILED(hr = pDevice->CreateDepthStencilView(pDSTexture.Get(), &dsvDesc, &m_pDepthStencilView)))
	{
		return hr;
	}

	// ���̸� �� ��츸 �����Ѵ�.
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
SDevice::SDevice()
{
	m_DriverType = D3D_DRIVER_TYPE_NULL;
	m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	m_pD3DDevice = nullptr;
	m_pImmediateContext = nullptr;
	m_pRenderTargetView = nullptr;
	m_pDxgiSwapChain = nullptr;
	m_pDxgiFactory = nullptr;
	m_pDepthStencilView = nullptr;
	m_pDsvSRV = nullptr;
	ZeroMemory(&m_ViewPort, sizeof(m_ViewPort));
}


SDevice::~SDevice()
{
	m_DriverType = D3D_DRIVER_TYPE_NULL;
	m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	m_pD3DDevice = nullptr;
	m_pImmediateContext = nullptr;
	m_pRenderTargetView = nullptr;
	m_pDxgiSwapChain = nullptr;
	m_pDxgiFactory = nullptr;
	m_pDepthStencilView = nullptr;
	m_pDsvSRV = nullptr;
	ZeroMemory(&m_ViewPort, sizeof(m_ViewPort));
}
