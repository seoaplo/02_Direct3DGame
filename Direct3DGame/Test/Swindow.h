#pragma once
#include "SDevice.h"

class SWindow : public SDevice
{
public:
	//=================================================================================================
	// Handler
	//=================================================================================================
	HWND		m_hWnd;			// Window Handle
	HINSTANCE	m_hInstance;	// Window Instance Handle

	//=================================================================================================
	// Window Value
	//=================================================================================================
	TCHAR*		m_lpszClientName;		// Window Name String
	DWORD		m_dwWindowStyle;		// WIndow Style
	RECT		m_rcWindowBounds;		// WIndow Size;

	//=================================================================================================
	// Client Size
	//=================================================================================================
	RECT		m_rcClientRect;			// Client Size
	UINT		m_nWindowWidth;			// Window Width Size
	UINT		m_nWindowHeight;		// Window Height Size

public:
	//=================================================================================================
	// Window Fuction
	//=================================================================================================
	virtual int WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	// virtual Window Procedure
	LRESULT	WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);	
	// Window Procedure
	bool	InitWindow(HINSTANCE hInstance, int iCmdShow, TCHAR* lpszClientName, RECT ClientRect, BOOL IsFullScreen = FALSE);
	// Window Initialize
	void	CenterWindow(HWND hWnd);
	// Set Window Position to Center

public:
	
	//=================================================================================================
	// Game FrameWork
	//=================================================================================================
	virtual bool Run();							// Enter the Game FrameWork
	
	virtual bool Init()		{ return true; }	// Initialize
	virtual bool Frame()	{ return true; }		// Process
	virtual bool Render()	{ return true; }		// Draw
	virtual bool Release()	{ return true; }		// Clear
	

public:
	SWindow();
	virtual ~SWindow();
};

