#include "STextureManager.h"


bool		STextureManager::Init()
{
	return true;
}
bool    STextureManager::Apply(ID3D11DeviceContext* pContext, UINT iKey, UINT iSlot)
{
	GetPtr(iKey)->Apply(pContext, iSlot);
	return true;
}
STexture* STextureManager::GetPtr(int iKey)
{
	ltor itor = m_TextureList.find(iKey);
	if (itor != m_TextureList.end())
	{
		STexture* pData = (*itor).second;
		return pData;
	}
	return nullptr;
}
bool STextureManager::Delete(int iKey)
{
	ltor ltor = m_TextureList.find(iKey);
	if (ltor != m_TextureList.end())
	{
		STexture* pData = (*ltor).second;
		pData->Release();
		delete pData;
		m_TextureList.erase(ltor);
		return true;
	}
	return false;
}
bool STextureManager::Render()
{
	for (ltor ltor = m_TextureList.begin(); ltor != m_TextureList.end(); ltor++)
	{
		STexture* pData = (*ltor).second;
		pData->Render();
	}
	return true;
};

int STextureManager::Load(
	ID3D11Device* pDevice, T_STR FilePath)
{
	// fullpath split
	if (FilePath.empty()) return -1;

	TCHAR Drive[MAX_PATH] = { 0, };
	TCHAR Dir[MAX_PATH] = { 0, };
	TCHAR Name[MAX_PATH] = { 0, };
	TCHAR Ext[MAX_PATH] = { 0, };
	_tsplitpath_s(FilePath.c_str(),
		Drive, Dir, Name, Ext);

	T_STR filename = Name;
	filename += Ext;

	T_STR path = Drive;
	path += Dir;

	// 중복제거
	for (ltor ltor = m_TextureList.begin();
		ltor != m_TextureList.end();
		ltor++)
	{
		STexture* pData = (*ltor).second;
		if (pData->m_szName == filename)
		{
			return pData->m_iIndex;
		}
	}
	STexture* pData = new STexture;
	if (FAILED(pData->Load(pDevice, FilePath.c_str())))
	{
		delete pData;
		return -1;
	}
	pData->m_szName = filename;
	pData->m_szPath = path;
	pData->m_iIndex = m_TextureList.size();
	m_TextureList.insert(make_pair(pData->m_iIndex, pData));
	return pData->m_iIndex;
}


bool STextureManager::Frame()
{
	return true;
};

bool STextureManager::Release()
{
	
	for (ltor itor = m_TextureList.begin(); itor != m_TextureList.end();)
	{
		STexture* pData = (*itor).second;
		pData->Release();
		delete pData;
		itor = m_TextureList.erase(itor);
	}
	m_TextureList.clear();
	return true;
};

STextureManager::STextureManager()
{
}


STextureManager::~STextureManager()
{
}
