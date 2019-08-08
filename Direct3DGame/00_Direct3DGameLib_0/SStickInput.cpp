#include "SStickInput.h"

bool SStickInput::Init(LPDIRECTINPUT8	DirectInput, HWND hWnd)
{
	HRESULT hr;
	hr = DirectInput->CreateDevice(GUID_Joystick, &m_pDiJoyStickDevice, NULL);
	HRESULT_FAILED_RETURN_BOOL(hr);

	hr = m_pDiJoyStickDevice->SetDataFormat(&c_dfDIJoystick);
	HRESULT_FAILED_RETURN_BOOL(hr);

	hr = m_pDiJoyStickDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr))
	{
		while (m_pDiJoyStickDevice->Acquire() == DI_OK)
		{
			hr = m_pDiJoyStickDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
			HRESULT_FAILED_RETURN_BOOL(hr);
		}
	}
	while (m_pDiJoyStickDevice->Acquire() == DI_OK);

	ZeroMemory(&m_JoyBeforeState, sizeof(m_JoyBeforeState));
	ZeroMemory(&m_JoyState, sizeof(m_JoyState));
	return true;
}
bool SStickInput::Frame()
{
	HRESULT hr;
	memcpy(&m_JoyBeforeState, &m_JoyState, sizeof(m_JoyState));

	ZeroMemory(&m_JoyState, sizeof(m_JoyState));
	if (!m_pDiJoyStickDevice) return false;

	hr = m_pDiJoyStickDevice->GetDeviceState(sizeof(m_JoyState), &m_JoyState);
	if (FAILED(hr))
	{
		while (m_pDiJoyStickDevice->Acquire() == DIERR_INPUTLOST)
		{
			hr = m_pDiJoyStickDevice->Acquire();
		}
	}
	return true;
}
bool SStickInput::Render()
{
	return true;
}
bool SStickInput::Release()
{
	m_pDiJoyStickDevice->Unacquire();
	m_pDiJoyStickDevice->Release();
	m_pDiJoyStickDevice = nullptr;
	return true;
}


void SStickInput::SetAcquire(bool bActive)
{
	if (bActive) m_pDiJoyStickDevice->Acquire();
	else		m_pDiJoyStickDevice->Unacquire();
}

DWORD SStickInput::ButtonCheck(DWORD dwButton)
{
	DWORD KeyState;
	if (m_JoyState.rgbButtons[dwButton] & 0x80)
	{
		if (m_JoyBeforeState.rgbButtons[dwButton] & 0x80)
		{
			KeyState = KEY_HOLD;
		}
		else
		{
			KeyState = KEY_PUSH;
		}
	}
	else
	{
		if (m_JoyBeforeState.rgbButtons[dwButton] & 0x80 || m_JoyState.rgbButtons[dwButton] & 0x80)
		{
			KeyState = KEY_UP;
		}
		else
		{
			KeyState = KEY_FREE;
		}
	}
	return KeyState;
}
D3DVECTOR SStickInput::LeftStickLocation()
{
	D3DVECTOR LStick;
	LStick.x = static_cast<float>(m_JoyState.lX);
	LStick.y = static_cast<float>(m_JoyState.lY);
	LStick.z = static_cast<float>(m_JoyState.lZ);

	StickLocationNomalize(LStick.x);
	StickLocationNomalize(LStick.y);
	StickLocationNomalize(LStick.z);
	return LStick;
}
D3DVECTOR SStickInput::RightStickLocation()
{
	D3DVECTOR RStick;
	
	RStick.x = static_cast<float>(m_JoyState.lRx);
	RStick.y = static_cast<float>(m_JoyState.lRy);
	RStick.z = static_cast<float>(m_JoyState.lRz);

	StickLocationNomalize(RStick.x);
	StickLocationNomalize(RStick.y);
	StickLocationNomalize(RStick.z);
	return RStick;
}
SStickInput::~SStickInput()
{
	Release();
}

SStickInput::SStickInput()
{
	m_pDiJoyStickDevice = nullptr;
}


