#include "SSound.h"


void		SSound::Play()
{
	bool PlayState = false;
	if (m_pChannel)
	{
		m_pChannel->isPlaying(&PlayState);
	}

	if (!PlayState)
	{
		m_pSystem->playSound(m_pSound, NULL, false, &m_pChannel);
		m_pChannel->setVolume(m_fVolume);
	}
}

void		SSound::Stop()
{
	if (m_pChannel == nullptr) return;
	m_pChannel->stop();
}
void		SSound::Paused()
{
	if (m_pChannel == nullptr) return;
	bool Pause;
	m_pChannel->getPaused(&Pause);
	m_pChannel->setPaused(!Pause);
}
void		SSound::Volume(int iVolume, bool bUp)
{
	// 크기를 0에서 100 사이로 설정한다.
	if (iVolume > 100 || iVolume < 0)
	{
		return;
	}
	float fVolumeState;
	float fRealVolume = static_cast<float>((iVolume / 10.0f) * I_Timer.GetSPF());

	m_pChannel->getVolume(&fVolumeState);
	if (bUp)
	{
		fVolumeState = min(1, fVolumeState + fRealVolume);
	}
	else
	{
		fVolumeState = max(0, fVolumeState - fRealVolume);
	}
	m_pChannel->setVolume(fVolumeState);
}

void	SSound::PlayEffect()
{
	FMOD::Channel* pChannel = nullptr;
	pChannel->setVolume(m_fVolume);
	pChannel->setMode(FMOD_LOOP_OFF);

	m_pSystem->playSound(m_pSound, 0, false, &pChannel);
}
void	SSound::SetMode(DWORD dwMode)
{
	if (m_pChannel)
	{
		m_pChannel->setMode(dwMode);
	}
}

bool SSound::Init()
{
	return true;
}
bool SSound::Frame()
{
	return true;
}
bool SSound::Render()
{
	return true;
}
bool SSound::Release()
{
	if (m_pSound == nullptr) return false;
	m_pSound->release();
	return true;
}
SSound::SSound()
{
	m_fVolume = 0.5f;
	m_pChannel = nullptr;
	m_pSound = nullptr;
}


SSound::~SSound()
{
}
