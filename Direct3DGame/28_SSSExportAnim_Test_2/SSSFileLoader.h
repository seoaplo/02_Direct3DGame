#pragma once
#include "SParser.h"
#include "STemplate.h"
#include "SObject.h"

class SSSFileLoader : public Singleton<SSSFileLoader>
{
	friend class Singleton<SSSFileLoader>;
private:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;
	SParser m_Paser;

	int m_iMaterialSize;
	int m_iMeshSize;
	std::vector<SMaterial>	m_MaterialList;
	std::vector<SMesh>		m_MeshList;
	std::vector<SObject>	m_ObjectList;

public:
	bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	int Load(T_STR FilePath);
	SObject* GetPtr(int iIndex);
	int	GetSize()
	{
		return m_ObjectList.size();
	}
private:
	SSSFileLoader();
public:
	~SSSFileLoader();
};

#define I_SSSFileLoaderManeger SSSFileLoader::GetInstance()