#pragma once
#pragma once
#include "SBoxObj.h"

class SSkyObj : public SBoxObj
{
public:
	void SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj);
	SSkyObj(void);
	virtual ~SSkyObj(void);
};
