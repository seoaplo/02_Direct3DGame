#include "STimer.h"

bool STimer::Init()
{
	// �ʴ� ���ļ��� ������ ��´�
	QueryPerformanceFrequency(&m_Frame);
	// ���� 0�� ��� ������ ���� �ʴ´ٴ� �ǹ��̴�.
	if (&m_Frame.QuadPart == 0)
	{
		return false;
	}

	m_FPS				= m_Frame;
	m_iFramePerSecond	= 0;
	m_iFPSElapse		= 0;
	m_fSecPerFrame		= 0.0f;
	Start();
	
	return true;
}

bool STimer::Frame()
{
	// 1�������� �ð� üũ
	// ���� �ð��� ���´�.

	QueryPerformanceCounter(&m_Current);
	m_fSecPerFrame = static_cast<float>(m_Current.QuadPart - m_Frame.QuadPart) / static_cast<float>(m_Frequency.QuadPart);

	// 1�ʴ� ������ üũ
	if (((m_Current.LowPart - m_FPS.LowPart) / m_Frequency.LowPart) >= 1)
	{
		m_iFramePerSecond = m_iFPSElapse;
		m_iFPSElapse = 0;
		m_FPS = m_Current;
	}
	m_iFPSElapse++;

	m_Frame = m_Current;

	return true;
}

bool STimer::Render()
{
	return true;
}

void STimer::Start()
{
	m_bStarted = true;
	m_fEventTime = 0.0f;
	QueryPerformanceCounter((LARGE_INTEGER*)&m_Start);
}

void STimer::Stop()
{
	GetElapsedTime();
	m_bStarted = false;
}

void STimer::Reset()
{
	m_bStarted = false;
	m_fEventTime = 0.0f;
	ZeroMemory(&m_Start, sizeof(m_Start));
	ZeroMemory(&m_Elapse, sizeof(m_Elapse));
}

STimer::STimer()
{
	m_bStarted = false;
	m_iFramePerSecond = 0;
	m_iFPSElapse = 0;
	m_fSecPerFrame = 0.0f;
	m_fEventTime = 0.0f;
	// �ý����� ���ļ� �������� ��� �´�. �̴� �ð��� ǥ���� ��� ���ؼ� ����Ѵ�.
	QueryPerformanceFrequency((LARGE_INTEGER *)&m_Frequency);
}


STimer::~STimer()
{
}

//===================================================================================
//	Frame Work
//===================================================================================
bool	SGrobalTimer::Init()
{
	
	SetDebugRect(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(300.0f, 35.0f));
	m_bDebugFlag = false;
	return m_Timer.Init();
}
bool	SGrobalTimer::Frame()
{
	return m_Timer.Frame();
}
bool	SGrobalTimer::Render()
{
	if (!m_bDebugFlag) return true;

	static TCHAR DebugString[256];
	ZeroMemory(DebugString, ARRAYSIZE(DebugString));

	_stprintf_s(DebugString, _T("FPS:%d SecPerFrame:%10.5f Elapsed : %10.2f"),
		m_Timer.GetFPS(),
		m_Timer.GetSPF(),
		m_Timer.GetElapsedTime());

	RECT rc = { (LONG)m_vStart.x , (LONG)m_vStart.y , (LONG)m_vEnd.x, (LONG)m_vEnd.y };

	return I_DirectWrite.DrawTextW(rc, DebugString, D2D1::ColorF(1, 1, 1, 0.5));
}

void SGrobalTimer::SetDebugRect(D3DXVECTOR2 vStart, D3DXVECTOR2 vEnd)
{
	m_vStart = vStart;
	m_vEnd = vEnd;
}