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
			&m_pDevice, &m_FeatureLevel, &m_pImmediateContext);

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
	if (!m_pDevice)
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
	hr = m_pDxgiFactory->CreateSwapChain(m_pDevice, &m_SwapChainDesc, &m_pDxgiSwapChain);

	return hr;
}

HRESULT SDevice::SetRenderTargetView()
{
	HRESULT hr = S_OK;

	// 2D �ؽ��� �������̽� �ӽ� �����ͷ� ����ü������ ���� ���۸� ��´�
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = m_pDxgiSwapChain->GetBuffer(NULL, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	HRESULT_FAILED_RETURN(hr);
	
	// ���� ���۷� ���� Ÿ�� �並 �����ϰ� �ӽ÷� ���� �������� ������ �����Ѵ�.
	hr = m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, m_DefaultRT.m_pRenderTargetView.GetAddressOf());
	pBackBuffer->Release();
	HRESULT_FAILED_RETURN(hr);

	return hr;
}

HRESULT SDevice::SetViewPort()
{
	HRESULT hr = S_OK;

	// ����ü�� ��ũ���͸� ��´�.
	DXGI_SWAP_CHAIN_DESC Desc;
	m_pDxgiSwapChain->GetDesc(&Desc);

	// ����ü�� ��ũ���Ϳ� �ִ� ���� ���� ���̷� ����Ʈ�� �ʱ�ȭ ��Ų��.

	m_DefaultRT.m_vp.Width = static_cast<float>(Desc.BufferDesc.Width);
	m_DefaultRT.m_vp.Height = static_cast<float>(Desc.BufferDesc.Height);
	m_DefaultRT.m_vp.MinDepth = 0.0f;
	m_DefaultRT.m_vp.MaxDepth = 1.0f;
	m_DefaultRT.m_vp.TopLeftX = 0;
	m_DefaultRT.m_vp.TopLeftY = 0;

	//����̽� ���ؽ�Ʈ�� ����Ʈ�� �����Ѵ�
	m_pImmediateContext->RSSetViewports(1, &m_DefaultRT.m_vp);

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
	if (m_pDxgiSwapChain)
	{
		m_pDxgiSwapChain->Release();
	}
	if (m_pImmediateContext)
	{
		m_pImmediateContext->Release();
	}
	if (m_pDevice)
	{
		m_pDevice->Release();
	}
	if (m_pDxgiFactory)
	{
		m_pDxgiFactory->Release();
	}

	m_pDevice = nullptr;
	m_pImmediateContext = nullptr;
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
	m_DefaultRT.m_pRenderTargetView.ReleaseAndGetAddressOf();

	//=============================================================
	// ����ü�ο� ��ũ���͸� �� ������ ũ�⸦ �����Ѵ�.
	//=============================================================
	m_pDxgiSwapChain->ResizeBuffers(m_SwapChainDesc.BufferCount, iWidth, iHeight, m_SwapChainDesc.BufferDesc.Format, m_SwapChainDesc.Flags);
	m_pDxgiSwapChain->GetDesc(&m_SwapChainDesc);

	// ����Ÿ�ٺ� ���� �� �����Ѵ�.
	hr = SetRenderTargetView();
	HRESULT_FAILED_RETURN(hr);

	hr = SetViewPort();
	HRESULT_FAILED_RETURN(hr);

	hr = CreateDxResource();
	HRESULT_FAILED_RETURN(hr);

	return hr;

}
SDevice::SDevice()
{
	m_DriverType = D3D_DRIVER_TYPE_NULL;
	m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	m_pDevice = nullptr;
	m_pImmediateContext = nullptr;
	m_pDxgiSwapChain = nullptr;
	m_pDxgiFactory = nullptr;
}


SDevice::~SDevice()
{
	m_DriverType = D3D_DRIVER_TYPE_NULL;
	m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	m_pDevice = nullptr;
	m_pImmediateContext = nullptr;
	m_pDxgiSwapChain = nullptr;
	m_pDxgiFactory = nullptr;
}
