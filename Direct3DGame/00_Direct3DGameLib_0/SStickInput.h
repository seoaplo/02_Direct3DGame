#pragma once
#include "SUtils.h"
#include "SDxBasic.h"

class SStickInput
{
	friend class SInputManager;
private:
	//===========================================================================
	// Direct Input Pointer
	//===========================================================================
	LPDIRECTINPUTDEVICE8	m_pDiJoyStickDevice;					// 조이스틱 디바이스
	//===========================================================================
	// State Value
	//===========================================================================
	DIJOYSTATE				m_JoyState;
	DIJOYSTATE				m_JoyBeforeState;

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
	D3DVECTOR		LeftStickLocation();
	D3DVECTOR		RightStickLocation();
	DWORD			ButtonCheck(DWORD dwButton);
	void			SetAcquire(bool bActive);
private:
	SStickInput();
	~SStickInput();
};

#define StickLocationNomalize(n)	n = ((n / 65535) * 100) - 100
