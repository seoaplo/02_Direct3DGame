#pragma once
#include "STexture.h"
#include "STemplate.h"

class STextureManager : public Singleton<STextureManager>
{
friend class Singleton<STextureManager>;
typedef std::map<int, STexture*>::iterator ltor;
public:
	std::map<int, STexture*> m_TextureList;
public:
	bool		Init();
	bool		Frame();
	bool		Render();
	bool		Release();
	int         Load(ID3D11Device* pDevice, T_STR pFilePath);
	STexture*	GetPtr(int iKey);
	bool		Delete(int iKey);
	bool		Apply(ID3D11DeviceContext* pContext,
		UINT iKey,
		UINT iSlot);
public:
	STextureManager();
	~STextureManager();
};

#define I_TextureManager STextureManager::GetInstance()