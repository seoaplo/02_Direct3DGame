#pragma once
#include "SMap.h"

class SHeightMap : public SMap
{
public:
	vector<float>		m_fHeightList;
public:
	bool		CreateHeightMap(const TCHAR* strHeightMap);
	float		GetHeightOfVertex(UINT Index);
	D3DXVECTOR4	GetColorOfVertex(UINT Index);
	float		GetHeightmap(int row, int col);
public:
	SHeightMap();
	virtual ~SHeightMap();
};

