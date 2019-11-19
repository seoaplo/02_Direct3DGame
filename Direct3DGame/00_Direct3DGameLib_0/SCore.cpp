#include "SCore.h"

using namespace DXGame;

SCore::SCore()
{

}

SCore::~SCore()
{
}

// CoreFrameWork
bool	SCore::CoreInit()
{
	if (FAILED(InitDevice(m_hWnd, m_nClientWidth, m_nClientHeight, false)))
	{
		MessageBox(0, L"Create Device 실패", L"Fatal error", MB_OK);
		return false;
	}
	SDxState::SetState(m_pDevice);

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
	if (!I_InputManager.Init())
		return false;

	SDxState::SetState(m_pDevice);

	CreateDxResource();


	if (!PreInit())	return false;
	if (!Init())	return false;
	if (!PostInit())	return false;

	m_RSDebugNum = 0;
	m_BSDebugNum = 0;
	m_SSDebugNum = 0;
	m_DSSDebugNum = 0;

	m_Direction.Create(m_pDevice, m_pImmediateContext);
	m_bDebugState = false;

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

	ProcDebug();
	m_Direction.Frame();

	if (!PreFrame())	return false;
	if (!Frame())	return false;
	if (!PostFrame())	return false;

	return true;
}
bool	SCore::CoreRender()
{
	float Color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	m_pImmediateContext->ClearRenderTargetView(GetRenderTargetView(), Color);
	m_pImmediateContext->ClearDepthStencilView(
		m_DefaultRT.m_pDepthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);
	m_pImmediateContext->OMSetRenderTargets(1, GetRenderTargetViewAddress(), m_DefaultRT.m_pDepthStencilView.Get());
	m_pImmediateContext->RSSetViewports(1, &m_DefaultRT.m_vp);

	SDxState::SetRasterizerState(m_pImmediateContext, m_RSDebugNum);
	SDxState::SetBlendState(m_pImmediateContext, m_BSDebugNum);
	SDxState::SetSamplerState(m_pImmediateContext, m_SSDebugNum);
	SDxState::SetDepthStencilState(m_pImmediateContext, m_DSSDebugNum);

	if(!PreRender())	return false;
	if (!Render())	return false;

	if (!I_Timer.Render())
		return false;
	if (!I_DirectWrite.Render())
		return false;
	if (!I_SoundManager.Render())
		return false;
	if (!I_InputManager.Render(m_rcClientRect.left, m_rcClientRect.top, m_rcClientRect.right, m_rcClientRect.bottom))
		return false;
	if (!PostRender())	return false;

	DrawDebug();
	m_pDxgiSwapChain->Present(0, 0);

	return true;
}
bool	SCore::CoreRelease()
{

	if (!PreRelease())	return false;
	if (!Release())	return false;
	if (!PostRelease())	return false;

	m_Direction.Release();

	if (!I_Timer.Release())
		return false;
	if (!I_DirectWrite.Release())
		return false;
	if (!I_SoundManager.Release())
		return false;
	if (!I_InputManager.Release())
		return false;

	if(!CleanupDevice())
		return false;

	return true;
}

// GameFrameWork

bool SCore::PreInit()
{

	return true;
}
bool SCore::PreFrame(){ return true; }
bool SCore::PreRender()
{
	return true;
}
bool SCore::PreRelease(){ return true; }

bool SCore::Init(){ return true; }
bool SCore::Frame(){ return true; }
bool SCore::Render()
{
	return true;
}
bool SCore::Release(){ return true; }

bool SCore::PostInit(){ return true; }
bool SCore::PostFrame(){ return true; }
bool SCore::PostRender()
{
	return true; 
}
bool SCore::PostRelease(){ return true; }


HRESULT SCore::CreateDxResource()
{

	IDXGISurface1*		pBackBuffer = NULL;
	HRESULT hr = GetSwapChain()->GetBuffer(0, __uuidof(IDXGISurface), (LPVOID*)&pBackBuffer);
	I_DirectWrite.Set(m_hWnd, m_nClientWidth, m_nClientHeight, pBackBuffer);
	if (pBackBuffer)	pBackBuffer->Release();

	hr = GetSwapChain()->GetDesc(&m_SwapChainDesc);
	HRESULT_FAILED_RETURN(hr);

	hr = m_DefaultRT.UpdateDepthStencilView(m_pDevice, m_SwapChainDesc.BufferDesc.Width, m_SwapChainDesc.BufferDesc.Height);
	HRESULT_FAILED_RETURN(hr);
	return CreateResource();
}
HRESULT SCore::DeleteDxResource()
{
	if (!I_DirectWrite.Release()) return false;
	return DeleteResource();
}
HRESULT SCore::CreateResource()
{
	return S_OK;
}
HRESULT SCore::DeleteResource()
{
	return S_OK;
}

bool SCore::DrawDebug()
{
	// FPS 출력
	TCHAR pBuffer[256];
	memset(pBuffer, 0, sizeof(TCHAR) * 256);
	_stprintf_s(pBuffer, _T("FPS:%d"), I_Timer.GetFPS());

	RECT rc1 = { 0,0, m_nClientWidth, m_nClientHeight };
	I_DirectWrite.DrawText(rc1, pBuffer, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.5));

	//-----------------------------------------------------------------------
	// 적용된 RasterizerState 타입 표시
	//-----------------------------------------------------------------------	
	RECT rc;
	rc.top = 30;
	rc.bottom = m_nClientWidth;
	rc.left = 0;
	rc.right = m_nClientHeight;
	I_DirectWrite.DrawText(rc, SDxState::RSDebugString.c_str(), D2D1::ColorF(0.0f, 0.0f, 1.0f, 1.0f));

	//-----------------------------------------------------------------------
	// 적용된 SamplerState 타입 표시
	//-----------------------------------------------------------------------	
	rc.top = 50;
	I_DirectWrite.DrawText(rc, SDxState::SSDebugString.c_str(), D2D1::ColorF(0.0f, 0.0f, 1.0f, 1.0f));

	//-----------------------------------------------------------------------
	// 적용된 DepthStencilState 타입 표시
	//-----------------------------------------------------------------------	
	rc.top = 70;
	I_DirectWrite.DrawText(rc, SDxState::DSSDebugString.c_str(), D2D1::ColorF(0.0f, 0.0f, 1.0f, 1.0f));

	//-----------------------------------------------------------------------
	// 적용된 BlendState 타입 표시
	//-----------------------------------------------------------------------	
	rc.top = 90;
	I_DirectWrite.DrawText(rc, SDxState::BSDebugString.c_str(), D2D1::ColorF(0.0f, 0.0f, 1.0f, 1.0f));

	m_Direction.Render(m_pImmediateContext);
	return true;
}
bool SCore::ProcDebug()
{
	if (I_InputManager.KeyBoardState(DIK_F1) == KEY_PUSH)
	{
		m_RSDebugNum = (m_RSDebugNum + 1) % DebugRSSetNum;
	}
	if (I_InputManager.KeyBoardState(DIK_F2) == KEY_PUSH)
	{
		m_BSDebugNum = (m_BSDebugNum + 1) % DebugBSSetNum;
	}
	if (I_InputManager.KeyBoardState(DIK_F3) == KEY_PUSH)
	{
		m_SSDebugNum = (m_SSDebugNum + 1) % DebugSSSetNum;
	}
	if (I_InputManager.KeyBoardState(DIK_F4) == KEY_PUSH)
	{
		m_DSSDebugNum = (m_DSSDebugNum + 1) % DebugDSSSetNum;
	}
	return true;
}
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
