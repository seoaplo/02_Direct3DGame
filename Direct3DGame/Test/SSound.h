#pragma once
#include "SUtils.h"
#include "STemplate.h"
#include "STimer.h"
#include "fmod.hpp"

class SSound
{
private:
	friend class SSoundManager;
private:
	//===============================================================================================
	// SSoundManager Value
	//===============================================================================================
	FMOD::System*			m_pSystem;					// SSoundManager가 가진 System 주소
	int						m_iIndex;					// SSoundManager 안에서 해당되는 번호
private:
	//===============================================================================================
	// Sound Value
	//===============================================================================================
	FMOD::Sound*			m_pSound;					// 사운드
	float					m_fVolume;					// 사운드 크기
	FMOD::Channel*			m_pChannel;					// 사운드 채널
	std::string				m_csFileName;				// 파일 이름
public:
	void		Play();
	void		Stop();
	void		Paused();
	void		Volume(int iVolume = 10, bool bUp = true);
	void		PlayEffect();
	void		SetMode(DWORD dwMode);

private:
	bool Init();
	bool Frame();
	bool Render();
	bool Release();

private:
	SSound();
	virtual ~SSound();
};