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

struct SMesh
{
	int		iIndex;
	int     iMtrlID;
	int     iSubMeshNum;
	int		iTriangleNum;
	T_STR    name;
	T_STR    ParentName;
	D3DXMATRIX matWorld;
	std::vector<std::vector<PNCT_VERTEX>>	m_VertexList;
	std::vector<std::vector<DWORD>>			m_IndexList;
	std::vector<DXGame::SDxHelperEX>	m_dxobjList;
public:
	SMesh()
	{
		iMtrlID = -1;
		iSubMeshNum = 1;
	}
	virtual ~SMesh()
	{

	}
};

