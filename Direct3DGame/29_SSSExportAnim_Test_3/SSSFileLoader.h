#pragma once
#include "SParser.h"
#include "STemplate.h"
#include "SDrawObject.h"

class SSSFileLoader : public Singleton<SSSFileLoader>
{
	friend class Singleton<SSSFileLoader>;
private:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;
	SParser m_Paser;

public:
	bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	bool Load(T_STR FilePath);
private:
	SSSFileLoader();
public:
	~SSSFileLoader();
};

#define I_SSSFileLoaderManeger SSSFileLoader::GetInstance()