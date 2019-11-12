#pragma once
#include "SMesh.h"
#include "STimer.h"
#include "SCAHeader.h"


class SDrawObject
{
public:
	int m_iIndex;
	int m_iClassType;
	int m_iMaterialID;
	T_STR	m_ObjectName;
	T_STR	m_ParentName;
	SDrawObject*	m_pParentObject;

	D3DXVECTOR3 m_vCenter;

	SScene	m_Scene;
	SMaterial m_Material;
	std::vector<SMesh> m_MeshList;

	D3DXMATRIX m_matAnim;
	D3DXMATRIX m_matAnimInv;
	D3DXMATRIX  m_matCalculation;
	D3DXMATRIX  m_matWorld;
	D3DXMATRIX  m_matView;
	D3DXMATRIX  m_matProj;

	float m_fAnimationSpeed;
	float m_fAnimElapseTime;
	std::vector<SAnimTrack>   m_PosAnimList;
	std::vector<SAnimTrack>   m_RotAnimList;
	std::vector<SAnimTrack>   m_ScaleAnimList;

	// 0 frame 분해된 에니메이션 트랙의 행렬
	D3DXMATRIX m_matAnimSelf;
	D3DXMATRIX m_matPos;
	D3DXMATRIX m_matRot;
	D3DXMATRIX m_matScl;
	D3DXVECTOR3 m_vPosTrack;
	D3DXVECTOR3 m_vScaleTrack;
	D3DXQUATERNION m_qRotTrack;

public:
	SMesh* CreateMesh();
	bool Init();
	bool Frame();
	bool Render(ID3D11DeviceContext* pContext);
	bool Release();

	virtual bool Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pShaderFile = nullptr);

	void Interpolate(D3DXMATRIX &matParent,  float fElapseTime);
	bool GetAnimationTrack(float fElapseTime, std::vector<SAnimTrack>& trackList, SAnimTrack& StartTrack, SAnimTrack& EndTrack);
	
	bool SetParent(SDrawObject* pParentObject);
	bool SetParent(int	iIndex);
	bool SetParent(T_STR ParentName);

	bool SetMaterial(SMaterial* material);
	bool SetMaterial(T_STR MaterialName);
	bool SetMaterial(int iMaterialNumber);

	bool SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView = nullptr, D3DXMATRIX* pProj = nullptr);
public:
	SDrawObject();
	virtual ~SDrawObject();
};

class SDrawObjectManager : public Singleton<SDrawObjectManager>
{
	friend class Singleton<SDrawObjectManager>;
private:
	std::vector<SDrawObject> m_DrawObjectList;
public:
	SDrawObject*	Create();
	SDrawObject*	GetDrawObject(T_STR DrawObjectName);
	SDrawObject*	GetDrawObject(int	iIndex);
	int			GetSize();
private:
	void UpdateParent();
private:
	SDrawObjectManager() {}
public:
	~SDrawObjectManager() {}
};

#define I_DrawObjectManager SDrawObjectManager::GetInstance()