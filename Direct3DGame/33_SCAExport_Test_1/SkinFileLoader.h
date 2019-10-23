#pragma once
#include "SParser.h"
#include "STemplate.h"
#include "SSkinObject.h"

class SkinFileLoader : public Singleton<SkinFileLoader>
{
	friend class Singleton<SkinFileLoader>;
private:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;
	SParser m_Paser;
public:
	bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	bool Load(T_STR FilePath);
private:
	SkinFileLoader();
public:
	~SkinFileLoader();
};

#define I_SkinFileLoaderManeger SkinFileLoader::GetInstance()
