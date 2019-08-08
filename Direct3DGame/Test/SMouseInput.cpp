#include "SMouseInput.h"

bool SMouseInput::Init(LPDIRECTINPUT8	DirectInput, HWND hWnd)
{
	HRESULT hr;
	hr = DirectInput->CreateDevice(GUID_SysMouse, &m_pDiMouseDevice, NULL);
	HRESULT_FAILED_RETURN_BOOL(hr);

	hr = m_pDiMouseDevice->SetDataFormat(&c_dfDIMouse);
	HRESULT_FAILED_RETURN_BOOL(hr);

	hr = m_pDiMouseDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr))
	{
		while (m_pDiMouseDevice->Acquire() == DI_OK)
		{
			hr = m_pDiMouseDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
			HRESULT_FAILED_RETURN_BOOL(hr);
		}
	}
	while (m_pDiMouseDevice->Acquire() == DI_OK);

	return true;
}
bool SMouseInput::Frame()
{
	HRESULT hr;
	memcpy(&m_MouseBeforeState, &m_MouseState, sizeof(m_MouseState));

	ZeroMemory(&m_MouseState, sizeof(m_MouseState));
	if (!m_pDiMouseDevice) return false;

	hr = m_pDiMouseDevice->GetDeviceState(sizeof(m_MouseState), &m_MouseState);
	if (FAILED(hr))
	{
		while (m_pDiMouseDevice->Acquire() == DIERR_INPUTLOST)
		{
			hr = m_pDiMouseDevice->Acquire();
		}
	}
	return true;
}
bool SMouseInput::Render()
{
	return true;
}
bool SMouseInput::Release()
{
	m_pDiMouseDevice->Unacquire();
	m_pDiMouseDevice->Release();
	m_pDiMouseDevice = nullptr;
	return true;
}


void SMouseInput::SetAcquire(bool bActive)
{
	if (bActive) m_pDiMouseDevice->Acquire();
	else		m_pDiMouseDevice->Unacquire();
}
SMouseInput::SMouseInput()
{
	m_pDiMouseDevice = nullptr;
}
DWORD SMouseInput::ButtonCheck(DWORD dwButton)
{
	DWORD KeyState;
	if (m_MouseState.rgbButtons[dwButton] & 0x80)
	{
		if (m_MouseBeforeState.rgbButtons[dwButton] & 0x80)
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
		if (m_MouseBeforeState.rgbButtons[dwButton] & 0x80 || m_MouseState.rgbButtons[dwButton] & 0x80)
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

D3DVECTOR SMouseInput::MouseLocation()
{
	D3DVECTOR Mouse;
	Mouse.x = static_cast<float>(m_MouseState.lX);
	Mouse.y = static_cast<float>(m_MouseState.lY);
	Mouse.z = static_cast<float>(m_MouseState.lZ);
	return Mouse;
}

SMouseInput::~SMouseInput()
{
	Release();
}
