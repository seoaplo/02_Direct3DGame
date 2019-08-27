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

	// 특성 레벨 배열
	D3D_FEATURE_LEVEL d3dFeatureLevelArray[]
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	UINT nd3dFeatureLevelArraySize = ARRAYSIZE(d3dFeatureLevelArray);

	// 드라이버 타입 배열
	D3D_DRIVER_TYPE d3dDriverTypeArray[]
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};

	UINT nd3dDriverTypeArraySize = ARRAYSIZE(d3dDriverTypeArray);

	// 드라이버 타입 개수만큼 반복하면서 디바이스를 생성하는 함수를 실행

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

	// 스왑체인 디스크립터 설정
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
	// 팩토리에서 스왑체인 생성
	hr = m_pDxgiFactory->CreateSwapChain(m_pD3DDevice, &m_SwapChainDesc, &m_pDxgiSwapChain);

	return hr;
}

HRESULT SDevice::SetRenderTargetView()
{
	HRESULT hr = S_OK;

	// 2D 텍스쳐 인터페이스 임시 포인터로 스왑체인으로 부터 버퍼를 얻는다
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = m_pDxgiSwapChain->GetBuffer(NULL, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);

	if (FAILED(hr))
	{
		return hr;
	}
	
	// 얻은 버퍼로 렌더 타겟 뷰를 생성하고 임시로 만든 포인터의 참조를 해제한다.
	hr = m_pD3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
	pBackBuffer->Release();

	if (FAILED(hr))
	{
		return hr;
	}

	// 컨텍스트에 렌더 타겟 뷰를 설정한다.
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);
	return hr;
}

HRESULT SDevice::SetViewPort()
{
	HRESULT hr = S_OK;

	// 스왑체인 디스크립터를 얻는다.
	DXGI_SWAP_CHAIN_DESC Desc;
	m_pDxgiSwapChain->GetDesc(&Desc);

	// 스왑체인 디스크립터에 있는 가로 세로 길이로 뷰포트를 초기화 시킨다.

	m_ViewPort.Width = static_cast<float>(Desc.BufferDesc.Width);
	m_ViewPort.Height = static_cast<float>(Desc.BufferDesc.Height);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;

	//디바이스 컨텍스트에 뷰포트를 셋팅한다
	m_pImmediateContext->RSSetViewports(1, &m_ViewPort);

	return hr;
}

#pragma endregion

bool SDevice::CleanupDevice()
{
	// 초기 풀스크린 윈도우에서 응용프로그램이 닫히는 경우에는 윈도우 전환 후에
	// 객체들을 소멸해야 한다.(메모리 누수를 막을 수 있다.)
	m_pDxgiSwapChain->SetFullscreenState(false, nullptr);

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

	//=============================================================
	// 렌더타겟과 깊이스텐실 버퍼를 해제한다.
	//=============================================================

	m_pImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);
	SAFE_RELEASE(m_pRenderTargetView);

	//=============================================================
	// 스왑체인에 디스크립터를 얻어서 버퍼의 크기를 조절한다.
	//=============================================================

	DXGI_SWAP_CHAIN_DESC	CurrentSD;
	m_pDxgiSwapChain->GetDesc(&CurrentSD);
	m_pDxgiSwapChain->ResizeBuffers(CurrentSD.BufferCount, iWidth, iHeight, CurrentSD.BufferDesc.Format, CurrentSD.Flags);


	// 렌더타겟뷰 생성 및 적용한다.

	if (FAILED(hr = SetRenderTargetView()))
	{
		return hr;
	}

	if (FAILED(hr = SetViewPort()))
	{
		return hr;
	}

	return hr;

}


SDevice::SDevice()
{
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

	ZeroMemory(&m_ViewPort, sizeof(m_ViewPort));
}
