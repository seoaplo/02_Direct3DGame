#include "SCore.h"



SCore::SCore()
{
}


SCore::~SCore()
{
}


// CoreFrameWork
bool	SCore::CoreInit()
{
	if (FAILED(InitDevice(m_hWnd, m_nWindowWidth, m_nWindowHeight, false)))
	{
		MessageBox(0, L"Create Device ½ÇÆÐ", L"Fatal error", MB_OK);
		return false;
	}

	if (!I_Timer.Init())
		return false;
	if (!I_DirectWrite.Init())
		return false;
	if (!I_SoundManager.Init())
		return false;
	if (!I_InputManager.Init())
		return false;
	if (!I_InputManager.InitDirectInput(m_hInstance, m_hWnd, true, true, false))
		return false;

	if (!BeginInit())	return false;
	if (!Init())	return false;
	if (!EndInit())	return false;

	return true;
}
bool	SCore::CoreFrame()
{
	if (!I_Timer.Frame())
		return false;
	if (!I_DirectWrite.Frame())
		return false;
	if (!I_SoundManager.Frame())
		return false;
	if (!I_InputManager.Frame())
		return false;

	if (!BeginFrame())	return false;
	if (!Frame())	return false;
	if (!EndFrame())	return false;

	return true;
}
bool	SCore::CoreRender()
{
	if(!BeginRender())	return false;
	if (!Render())	return false;

	if (!I_Timer.Render())
		return false;
	if (!I_DirectWrite.Render())
		return false;
	if (!I_SoundManager.Render())
		return false;
	if (!I_InputManager.Render(m_rcClientRect.left, m_rcClientRect.top, m_rcClientRect.right, m_rcClientRect.bottom))
		return false;

	if (!EndRender())	return false;

	return true;
}
bool	SCore::CoreRelease()
{

	if (!BeginRelease())	return false;
	if (!Release())	return false;
	if (!EndRelease())	return false;

	if (!I_Timer.Release())
		return false;
	if (!I_DirectWrite.Release())
		return false;
	if (!I_SoundManager.Release())
		return false;
	if (!I_InputManager.Release())
		return false;

#if defined(DEBUG) || defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D11Debug> dxgiDebug;

	if (SUCCEEDED(GetDevice()->QueryInterface(IID_PPV_ARGS(&dxgiDebug))))
	{
		dxgiDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		dxgiDebug = nullptr;
	}
#endif

	if(!CleanupDevice())
		return false;

	return true;
}

// GameFrameWork

bool SCore::BeginInit()
{
	

	if (!I_Timer.Init()) return false;

	IDXGISurface1*	pBackBuffer = nullptr;
	HRESULT hr = GetSwapChain()->GetBuffer(0, __uuidof(IDXGISurface), (LPVOID*)&pBackBuffer);
	I_DirectWrite.Set(m_hWnd, m_nWindowHeight, m_nWindowHeight, pBackBuffer);
	SAFE_RELEASE(pBackBuffer);

	return true;
}
bool SCore::BeginFrame(){ return true; }
bool SCore::BeginRender()
{
	float Color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, Color);
	return true;
}
bool SCore::BeginRelease(){ return true; }

bool SCore::Init(){ return true; }
bool SCore::Frame(){ return true; }
bool SCore::Render()
{
	return true;
}
bool SCore::Release(){ return true; }

bool SCore::EndInit(){ return true; }
bool SCore::EndFrame(){ return true; }
bool SCore::EndRender()
{

	m_pDxgiSwapChain->Present(0, 0);
	return true; 
}
bool SCore::EndRelease(){ return true; }

int SCore::WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return -1;
}

bool SCore::Run()
{
	if (!CoreInit())
	{
		return false;
	}
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, NULL, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (!CoreFrame() || !CoreRender())
			{
				break;
			}
		}
	}
	if (!CoreRelease())
	{
		return false;
	}
	return true;
}
