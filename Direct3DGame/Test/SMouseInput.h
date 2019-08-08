#pragma once
#include "SUtils.h"
#include "SDxBasic.h"

enum MouseButton
{
	MOUSE_LBUTTON,
	MOUSE_RBUTTON,
	MOUSE_WHELL,
	MOUSE_OTHER
};

class SMouseInput
{
	friend class SInputManager;
private:
	//===========================================================================
	// Direct Input Pointer
	//===========================================================================
	LPDIRECTINPUTDEVICE8	m_pDiMouseDevice;					// 마우스 디바이스
	//===========================================================================
	// State Value
	//===========================================================================
	DIMOUSESTATE			m_MouseState;
	DIMOUSESTATE			m_MouseBeforeState;

public:
	//===========================================================================
	// Game FrameWork
	//===========================================================================
	bool			Init(LPDIRECTINPUT8	DirectInput, HWND hWnd);
	bool			Frame();
	bool			Render();
	bool			Release();
	//===========================================================================
	//	Other
	//===========================================================================
	D3DVECTOR		MouseLocation();
	DWORD			ButtonCheck(DWORD dwButton);
	void			SetAcquire(bool bActive);

private:
	SMouseInput();
	~SMouseInput();
};

