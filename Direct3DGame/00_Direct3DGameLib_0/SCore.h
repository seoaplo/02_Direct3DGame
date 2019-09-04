#pragma once
#include "STimer.h"
#include "SDXState.h"
#include "SDirectWrite.h"
#include "SInputManager.h"
#include "Swindow.h"
#include "SSoundManager.h"

class SCore	: public SWindow
{
public:
	UINT	m_RSDebugNum;
	UINT	m_BSDebugNum;
	UINT	m_SSDebugNum;
	UINT	m_DSSDebugNum;
	
	bool	m_bDebugState;
	virtual bool		ProcDebug();
	virtual bool		DrawDebug();
public:
	// CoreFrameWork
	bool	CoreInit();
	bool	CoreFrame();
	bool	CoreRender();
	bool	CoreRelease();

public:
	// GameFrameWork

	virtual bool PreInit();
	virtual bool PreFrame();
	virtual	bool PreRender();
	virtual bool PreRelease();

	virtual bool Init();
	virtual bool Frame();
	virtual	bool Render();
	virtual bool Release();

	virtual bool PostInit();
	virtual bool PostFrame();
	virtual	bool PostRender();
	virtual bool PostRelease();

	virtual int WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	bool Run();
public:
	HRESULT		CreateDxResource();
	HRESULT		DeleteDxResource();
	virtual		HRESULT		CreateResource();
	virtual		HRESULT		DeleteResource();

public:
	SCore();
	~SCore();
};

//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
//{
//	SCore g_Core;
//	if (!g_Core.InitWindow(hInstance, nCmdShow, const_cast<TCHAR*>(L"SoundTest"), g_Core.m_IsFullScreenMode))
//	{
//		MessageBox(0, _T("Sample  초기화 실패"), _T("Fatal error"), MB_OK);
//		return 0;
//	}
//	ShowCursor(TRUE);
//	g_Core.Run();
//	return 1;
//}