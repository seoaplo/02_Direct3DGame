#pragma once
#include "SDrawObject.h"
#include "SSkinObject.h"
#include "SMatrixObject.h"

class SCAObject : public SDrawObject
{
public:
	int m_iSkinObjectListSize;
	std::vector<SSkinObject*>	m_SkinObjectList;
	SMatrixObjectList*				m_pMatrixObjectList;
public:
	bool Init();
	bool Frame();
	bool Render(ID3D11DeviceContext* pContext);
	bool Release();
public:
	bool SetMatrixObjectList(int iMatrixObejctList);
	bool SetSkinObject(int iSkinObejct);
	void ClearSkinObjectList();
public:
	SCAObject();
	virtual ~SCAObject();
};

