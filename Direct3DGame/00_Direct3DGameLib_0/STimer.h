#pragma once
#include "SDxBasic.h"
#include "SDirectWrite.h"
#include "STemplate.h"


class STimer 
{
private:
	//===================================================================================
	// Basis Time
	//===================================================================================
	LARGE_INTEGER	m_Frequency;		// 초당 주파수
	LARGE_INTEGER	m_Current;			// 현재 시간
	LARGE_INTEGER	m_Frame;			// 이전 프레임 시간 체크
	float			m_fSecPerFrame;		// 프레임 경과 시간

	//===================================================================================
	// Frame Time
	//===================================================================================
	LARGE_INTEGER	m_FPS;					// FPS 체크 타이머
	int				m_iFramePerSecond;		// 초당 프레임
	int				m_iFPSElapse;			// 초당 경과된 프레임

	//===================================================================================
	//	Event Time
	//===================================================================================
	LARGE_INTEGER	m_Start;				// 이벤트 타이머 시작 시간
	LARGE_INTEGER	m_Elapse;				// 이벤트 타이머 경과 시간
	bool			m_bStarted;				// 이벤트 타이머 작동 여부
	float			m_fEventTime;
public:
	//===================================================================================
	//	Frame Work
	//===================================================================================
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release() { return true; }

	//===================================================================================
	// 경과한 시간에 대한 지원 함수
	//===================================================================================
	void	Reset();
	void	Start();
	void	Stop();
	bool	IsStarted()const { return m_bStarted; }
	float	GetElapsedTime();

	//===================================================================================
	// FPS
	//===================================================================================
	int		GetFPS() { return m_iFramePerSecond; }
	float	GetSPF() { return m_fSecPerFrame; }

public:
	STimer();
	virtual ~STimer();
};

class SGrobalTimer : public Singleton<SGrobalTimer>
{
private:
	friend class Singleton<SGrobalTimer>;
private:
	STimer m_Timer;
	//===================================================================================
	// Debug Render
	//===================================================================================
	D3DXVECTOR2 m_vStart;
	D3DXVECTOR2 m_vPost;

	bool m_bDebugFlag;
	UINT m_iClientHeight;
	UINT m_iClientWidth;
public:
	
	//===================================================================================
	//	Frame Work
	//===================================================================================
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release() { return true; }

	//===================================================================================
	// 경과한 시간에 대한 지원 함수
	//===================================================================================
	void	Reset() { m_Timer.Reset();} ;
	void	Start() { m_Timer.Start(); };
	void	Stop() { m_Timer.Stop(); };
	bool	IsStarted()const { return m_Timer.IsStarted(); }
	float	GetElapsedTime() { return m_Timer.GetElapsedTime(); }

	//===================================================================================
	// FPS
	//===================================================================================
	int		GetFPS() { return m_Timer.GetFPS(); }
	float	GetSPF() { return m_Timer.GetSPF(); }

	//===================================================================================
	// Debug Render
	//===================================================================================
	void	SetDebugFlag(bool bFlag) { m_bDebugFlag = bFlag; };
	void	SetDebugRect(D3DXVECTOR2 vStart, D3DXVECTOR2 vPost);
private:
	SGrobalTimer() {} ;
public:
	virtual ~SGrobalTimer() {} ;
};

#define I_Timer SGrobalTimer::GetInstance()

