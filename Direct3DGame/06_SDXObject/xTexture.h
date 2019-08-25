#pragma once
#include "SDxBasic.h"

class xTexture
{
public:
	ID3D11ShaderResourceView* m_pSRV;
public:
	HRESULT Load(ID3D11Device* pDevice, const TCHAR* szFileName);
	bool	Apply(ID3D11DeviceContext* pContext, UINT iSlot = 0);
public:
	xTexture();
	~xTexture();
};

