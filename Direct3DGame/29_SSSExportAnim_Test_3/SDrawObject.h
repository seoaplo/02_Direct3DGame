#pragma once
#include "SMesh.h"
#include "STimer.h"

struct SScene
{
	int   iFirstFrame;  // 0
	int   iLastFrame;   // 100
	int   iFrameSpeed; // 1 Scecond = 30 Frame
	int   iTickPerFrame; // 1 Frame = 160 Tick
	int   iNumObjects;
	int   iNumMaterials;
};

struct SAnimTrack
{
	int					iTick;
	D3DXVECTOR3			Position;
	D3DXQUATERNION		Quaternion;
};

class SDrawObject
{
public:
	int m_iIndex;
	int m_iClassType;
	int m_iMaterialID;
	T_STR	m_ObjectName;
	T_STR	m_ParentName;
	SDrawObject*	m_pParentObject;

	SScene	m_Scene;
	SMaterial* m_pMaterial;
	std::vector<SMesh> m_MeshList;

	D3DXMATRIX m_matWorld;
	D3DXMATRIX m_matInvWorld;
	D3DXMATRIX  m_matCalculation;

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

	bool Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TCHAR* pShaderFile);

	void Interpolate(D3DXMATRIX &matParent,  float fElapseTime);
	bool GetAnimationTrack(float fElapseTime, std::vector<SAnimTrack>& trackList, SAnimTrack& StartTrack, SAnimTrack& EndTrack);
	
	bool SetParent(SDrawObject* pParentObject);
	bool SetParent(int	iIndex);
	bool SetParent(T_STR ParentName);

	bool SetMaterial(SMaterial* material);
	bool SetMaterial(T_STR MaterialName);
	bool SetMaterial(int iMaterialNumber);

public:
	SDrawObject();
	~SDrawObject();
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
	SDrawObjectManager() {}
public:
	~SDrawObjectManager() {}
};

#define I_DrawObjectManager SDrawObjectManager::GetInstance()