#pragma once
#include "SParser.h"
#include "STemplate.h"
#include "SMatrixObject.h"
class SMatrixFileLoader : public Singleton<SMatrixFileLoader>
{
	friend class Singleton<SMatrixFileLoader>;
private:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;
	SParser m_Paser;

public:
	bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	bool Load(T_STR FilePath);
private:
	SMatrixFileLoader();
public:
	~SMatrixFileLoader();
};

#define I_MatrixFileLoader SMatrixFileLoader::GetInstance()
