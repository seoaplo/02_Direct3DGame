#pragma once
#include "SModel.h"

struct STextureMap
{
public:
	int iIndexMap;
	int iMapID;
	T_STR	TextureName;
	STexture*	STexture;
	STextureMap()
	{
		iIndexMap = -1;
		iMapID = -1;
	}
};

struct SMaterial
{
public:
	int iIndex;
	T_STR MaterialName;
	int iSubMaterialNum;
	int iTextureMapNum;
	std::vector<SMaterial>			SubMaterial;
	std::vector<STextureMap>		TextrueMapList;
	SMaterial()
	{
		iIndex = -1;
		iSubMaterialNum = -1;
		iTextureMapNum = -1;
	}
};

struct SAnimTrack
{
	int					i;
	D3DXVECTOR3			p;
	D3DXQUATERNION		q;
};

struct SScene
{
	int   iFirstFrame;  // 0
	int   iLastFrame;   // 100
	int   iFrameSpeed; // 1 Scecond = 30 Frame
	int   iTickPerFrame; // 1 Frame = 160 Tick
	int   iNumObjects;
	int   iNumMaterials;
};

struct SMesh
{
	int		iIndex;
	int     iMtrlID;
	int		iClassID;
	int     iSubMeshNum;
	bool	bChild;
	T_STR   name;
	T_STR   ParentName;
	SMesh*	m_pParent;
	D3DXMATRIX m_matWorld;
	D3DXMATRIX m_matInvWorld;
	D3DXMATRIX  m_matCalculation;

	// 0 frame 분해된 에니메이션 트랙의 행렬
	D3DXMATRIX m_matAnimSelf;
	D3DXMATRIX m_matPos;
	D3DXMATRIX m_matRot;
	D3DXMATRIX m_matScl;
	D3DXVECTOR3 m_vPosTrack;
	D3DXVECTOR3 m_vScaleTrack;
	D3DXQUATERNION m_qRotTrack;

	SScene	m_Scene;
	std::vector<std::vector<PNCT_VERTEX>>	m_VertexList;
	std::vector<std::vector<DWORD>>			m_IndexList;
	std::vector<DWORD>			m_SubMaterialNum;
	
	std::vector<DXGame::SDxHelperEX>	m_dxobjList;

	std::vector<SAnimTrack>   m_PosAnimList;
	std::vector<SAnimTrack>   m_RotAnimList;
	std::vector<SAnimTrack>   m_ScaleAnimList;
public:
	SMesh()
	{
		iMtrlID = -1;
		iSubMeshNum = 1;
		D3DXMatrixIdentity(&m_matWorld);
		D3DXMatrixIdentity(&m_matPos);
		D3DXMatrixIdentity(&m_matRot);
		D3DXMatrixIdentity(&m_matScl);
		D3DXMatrixIdentity(&m_matCalculation);
		bChild = false;
	}
	virtual ~SMesh()
	{

	}
};

