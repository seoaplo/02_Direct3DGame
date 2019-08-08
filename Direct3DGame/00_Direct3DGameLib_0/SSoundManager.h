#pragma once
#include	"SSound.h"

class SSoundManager	: public Singleton<SSoundManager>
{
private:
	friend Singleton<SSoundManager>;
	typedef std::map<int, SSound*>::iterator ITOR;		// iterator
private:
	FMOD::System*			m_pSystem;					// 시스템
	int						m_iNumSound;				// 사운드 수
	std::map<int, SSound*>  m_SoundList;				// 사운드 리스트
public:
	bool		Init();
	bool		Frame();
	bool		Render();
	bool		Release();
	int			Load(const char* pFilePath);
	SSound*		GetPtr(int iKey);
	bool		Delete(int iKey);
	void		Play(int iKey);
private:
	SSoundManager();
public:
	~SSoundManager();
};

#define	I_SoundManager SSoundManager::GetInstance()
#define	CHANNELMAXARRAY	32