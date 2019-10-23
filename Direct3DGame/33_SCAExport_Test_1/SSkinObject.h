#pragma once
#include "SCAHeader.h"
#include "SSkinMesh.h"

class SSkinObject
{
public:
	int m_iIndex;
	int m_iMaterialID;
	T_STR	m_ObjectName;
	T_STR	m_CharacterName;

	D3DXVECTOR3 m_vCenter;

	SScene	m_Scene;
	SMaterial m_Material;
	std::vector<SSkinMesh> m_MeshList;
	ID3D11Buffer*	m_pConstMatrixBuffer;

	int			m_iInverseMatMaxNum;
	D3DXMATRIX	m_InvMatrixList[255];

	D3DXMATRIX  m_matObject;
	D3DXMATRIX  m_matWorld;
	D3DXMATRIX  m_matView;
	D3DXMATRIX  m_matProj;
public:
	SSkinMesh* CreateMesh();
	bool Init();
	bool Frame();
	bool Render(ID3D11DeviceContext* pContext);
	bool Release();

	bool Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pShaderFile = nullptr);

	bool SetMaterial(SMaterial* material);
	bool SetMaterial(T_STR MaterialName);
	bool SetMaterial(int iMaterialNumber);

	bool SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView = nullptr, D3DXMATRIX* pProj = nullptr);
public:
	SSkinObject();
	~SSkinObject();
};

class SSkinObjectManager : public Singleton<SSkinObjectManager>
{
	friend class Singleton<SSkinObjectManager>;
private:
	std::vector<SSkinObject> m_SkinObjectList;
public:
	SSkinObject*	Create();
	SSkinObject*	GetSkinObject(T_STR DrawObjectName);
	SSkinObject*	GetSkinObject(int	iIndex);
	int			GetSize();
private:
	SSkinObjectManager() {}
public:
	~SSkinObjectManager() {}
};

#define I_SkinObjectManager SSkinObjectManager::GetInstance()