#include "SSoundManager.h"

bool		SSoundManager::Init()
{
	FMOD::System_Create(&m_pSystem);
	m_pSystem->init(CHANNELMAXARRAY, FMOD_INIT_NORMAL, 0);
	return true;
}
bool		SSoundManager::Frame()
{
	m_pSystem->update();
	return true;
}
bool		SSoundManager::Render()
{
	return true;
}
bool		SSoundManager::Release()
{
	for (ITOR itor = m_SoundList.begin(); itor != m_SoundList.end();)
	{
		SSound* pSound = (*itor).second;
		pSound->Release();
		delete pSound;
		itor = m_SoundList.erase(itor);
	}

	m_SoundList.clear();

	m_pSystem->close();
	m_pSystem->release();
	return true;
}
int			SSoundManager::Load(const char* pFilePath)
{
	SSound*	pSound = new SSound;
	FMOD_RESULT	ret = m_pSystem->createSound(pFilePath, FMOD_DEFAULT, nullptr, &pSound->m_pSound);
	if (ret != FMOD_OK)
	{
		return -1;
	}
	pSound->m_pSystem = m_pSystem;
	pSound->m_csFileName = pFilePath;
	pSound->m_iIndex = m_iNumSound++;

	m_SoundList.insert(make_pair(pSound->m_iIndex, pSound));
	return pSound->m_iIndex;
}
SSound*		SSoundManager::GetPtr(int iKey)
{
	ITOR itor = m_SoundList.find(iKey);
	if (itor != m_SoundList.end())
	{
		SSound* pSound = (*itor).second;
		return pSound;
	}
	return nullptr;
}
bool		SSoundManager::Delete(int iKey)
{
	ITOR	itor = m_SoundList.find(iKey);
	if (itor != m_SoundList.end())
	{
		SSound* pSound = (*itor).second;
		pSound->Release();
		delete pSound;
		m_SoundList.erase(itor);
		return true;
	}
	return false;
}
void		SSoundManager::Play(int iKey)
{
	ITOR itor = m_SoundList.find(iKey);
	if (itor != m_SoundList.end())
	{
		SSound* pSound = (*itor).second;
		pSound->Play();
	}
}

SSoundManager::SSoundManager()
{
}


SSoundManager::~SSoundManager()
{
}
