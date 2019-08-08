#include "SInputManager.h"


bool SInputManager::ResetDevice()
{
	Release();
	Init();
	return true;
}

#pragma region FrameWork
bool SInputManager::Init()
{
	m_dwElements = 0;
	m_bDebugFlag = false;
	return true;
}

bool SInputManager::Frame()
{
	if (m_pKeyInput != nullptr)
	{
		if (!m_pKeyInput->Frame())	return false;
	}
	if (m_pMouseInput != nullptr)
	{
		if (!m_pMouseInput->Frame())	return false;
	}
	if (m_pStickInput != nullptr)
	{
		if (!m_pStickInput->Frame()) return false;
	}
	
	return true;
}

bool SInputManager::Render(UINT iStartX, UINT iStartY, UINT iClientWidth, UINT iClientHeight)
{
	if(!m_bDebugFlag) return true;


	if (m_pKeyInput != nullptr)
	{
		if (!m_pKeyInput->Render())		return false;
	}
	if (m_pMouseInput != nullptr)
	{
		if (!m_pMouseInput->Render())	return false;
	}
	if (m_pStickInput != nullptr)
	{
		if (!m_pStickInput->Render())	 return false;
	}

	return true;
}

bool SInputManager::Release()
{
	if (m_pDirectInput)
	{
		if (m_pKeyInput)
		{
			m_pKeyInput->Release();
			m_pKeyInput = nullptr;
		}
		if (m_pMouseInput)
		{
			m_pMouseInput->Release();
			m_pMouseInput = nullptr;
		}
		if (m_pStickInput)
		{
			m_pStickInput->Release();
			m_pStickInput = nullptr;
		}
		m_pDirectInput->Release();
		m_pDirectInput = nullptr;
	}
	return true;
}
#pragma endregion

bool SInputManager::InitDirectInput(HINSTANCE hInstance, HWND hWnd, bool keyboard, bool mouse, bool joystick)
{
	HRESULT hr;
	m_hWnd = hWnd;
	if(m_pKeyInput || m_pMouseInput || m_pStickInput) return true;

	// Direct Input Factory 생성
	hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL);

	HRESULT_FAILED_RETURN_BOOL(hr);
	//====================================================================================
	// 1. 장치 디바이스 생성
	// 2. 장치 데이터 형식의 설정
	// 3. 장치 동작의 설정
	//====================================================================================
	if (keyboard)
	{
		m_pKeyInput = new SKeyInput;
		m_pKeyInput->Init(m_pDirectInput, hWnd);
	}
	if (mouse)
	{
		m_pMouseInput = new SMouseInput;
		m_pMouseInput->Init(m_pDirectInput, hWnd);
	}
	if (joystick)
	{
		m_pStickInput = new SStickInput;
		m_pStickInput->Init(m_pDirectInput, hWnd);
	}
	return true;
}

DWORD SInputManager::KeyBoardState(DWORD dwKey)
{
	if (m_pKeyInput)
		return m_pKeyInput->KeyCheck(dwKey);
	
	return KEY_ERROR;
}
DWORD SInputManager::MouseButtonState(DWORD dwKey)
{
	if (m_pMouseInput)
		return m_pMouseInput->ButtonCheck(dwKey);

	return KEY_ERROR;
}
DWORD SInputManager::JoystickButton(DWORD dwKey)
{
	if (m_pStickInput)
		return m_pStickInput->ButtonCheck(dwKey);

	return KEY_ERROR;
}

void SInputManager::DeviceAcquire()
{
	if (m_pKeyInput)
	{
		m_pKeyInput->SetAcquire(true);
	}
	if (m_pMouseInput)
	{
		m_pMouseInput->SetAcquire(true);
	}
	if (m_pStickInput)
	{
		m_pStickInput->SetAcquire(true);
	}
}

void SInputManager::DeviceUnAcquire()
{
	if (m_pKeyInput)
	{
		m_pKeyInput->SetAcquire(false);
	}
	if (m_pMouseInput)
	{
		m_pMouseInput->SetAcquire(false);
	}
	if (m_pStickInput)
	{
		m_pStickInput->SetAcquire(false);
	}
}

void SInputManager::SetAcquire(bool bActive)
{
	if(bActive) DeviceAcquire();
	else		DeviceUnAcquire();
}

SInputManager::SInputManager()
{
	m_pDirectInput = nullptr;
	m_pKeyInput = nullptr;
	m_pMouseInput = nullptr;
	m_pStickInput = nullptr;
	m_hWnd = NULL;
	m_dwElements = 0;
}


SInputManager::~SInputManager()
{
	Release();
}
