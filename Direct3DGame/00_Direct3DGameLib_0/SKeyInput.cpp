#include "SKeyInput.h"

bool			SKeyInput::Init(LPDIRECTINPUT8	DirectInput, HWND hWnd)
{
	HRESULT hr;
	hr = DirectInput->CreateDevice(GUID_SysKeyboard, &m_pDiKeyDevice, NULL);
	HRESULT_FAILED_RETURN_BOOL(hr);

	hr = m_pDiKeyDevice->SetDataFormat(&c_dfDIKeyboard);
	HRESULT_FAILED_RETURN_BOOL(hr);

	hr = m_pDiKeyDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY);
	if (FAILED(hr))
	{
		while (m_pDiKeyDevice->Acquire() == DI_OK)
		{
			hr = m_pDiKeyDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY);
			HRESULT_FAILED_RETURN_BOOL(hr);
		}
	}
	while (m_pDiKeyDevice->Acquire() == DI_OK);

	return true;
}
bool			SKeyInput::Frame()
{
	HRESULT hr;
	memcpy(m_KeyBeforeState, m_KeyState, KEYSTATECOUNT);

	ZeroMemory(m_KeyState, ARRAYSIZE(m_KeyState));
	if (!m_pDiKeyDevice) return false;
	hr = m_pDiKeyDevice->GetDeviceState(KEYSTATECOUNT, m_KeyState);
	if (FAILED(hr))
	{
		while (m_pDiKeyDevice->Acquire() == DIERR_INPUTLOST)
		{
			m_pDiKeyDevice->Acquire();
		}
		return true;
	}
	while (m_pDiKeyDevice->Acquire() == DI_OK);
	return true;
}
bool SKeyInput::Render()
{
	return true;
}
bool SKeyInput::Release()
{
	m_pDiKeyDevice->Unacquire();
	m_pDiKeyDevice->Release();
	m_pDiKeyDevice = nullptr;
	return true;
}

DWORD SKeyInput::KeyCheck(DWORD dwKey)
{
	DWORD KeyState;
	if (m_KeyState[dwKey] & 0x80)
	{
		if (m_KeyBeforeState[dwKey] & 0x80)
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
		if (m_KeyBeforeState[dwKey] & 0x80 || m_KeyState[dwKey] & 0x80)
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
void SKeyInput::SetAcquire(bool bActive)
{
	if (bActive) m_pDiKeyDevice->Acquire();
	else		m_pDiKeyDevice->Unacquire();
}
SKeyInput::SKeyInput()
{
	m_pDiKeyDevice = nullptr;
}


SKeyInput::~SKeyInput()
{
	Release();
}
