#pragma once
#include "SKeyInput.h"
#include "SStickInput.h"
#include "SMouseInput.h"
#include "STemplate.h"
#include "SDirectWrite.h"


class SInputManager : public Singleton<SInputManager>
{
private:
	friend class Singleton<SInputManager>;
public:
	//===========================================================================
	// Direct Input Pointer
	//===========================================================================
	LPDIRECTINPUT8			m_pDirectInput;
	SKeyInput*				m_pKeyInput;
	SMouseInput*			m_pMouseInput;
	SStickInput*			m_pStickInput;

	//===========================================================================
	// Other
	//===========================================================================
	HWND					m_hWnd;
	DWORD					m_dwElements;
	DWORD					m_dwImmediate;			// 직접 데이터 사용 여부
	bool					m_bDebugFlag;
public:
	//===========================================================================
	// Game FrameWork
	//===========================================================================
	bool			Init();
	bool			Frame();
	bool			Render(UINT iStartX, UINT iStartY, UINT iClientWidth, UINT iClientHeight);
	bool			Release();
	//===========================================================================
	// Reset
	//===========================================================================
	bool			ResetDevice();
	bool			ResetResource() { return true; }
	//===========================================================================
	// Init
	//===========================================================================
	bool			InitDirectInput(HINSTANCE, HWND, bool, bool, bool);
	//===========================================================================
	//	Other
	//===========================================================================
	DWORD			KeyBoardState(DWORD dwKey);
	DWORD			MouseButtonState(DWORD dwKey);
	DWORD			JoystickButton(DWORD dwKey);
	void			DeviceAcquire();
	void			DeviceUnAcquire();
	void			SetAcquire(bool bActive);
	void			SetDebugFlag(bool bFlag)
	{
		m_bDebugFlag = bFlag;
	}
private:
	SInputManager();
public:
	virtual ~SInputManager();
};
#define I_InputManager SInputManager::GetInstance()
