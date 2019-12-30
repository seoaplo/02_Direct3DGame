#pragma once
#include "SBrush.h"


class SAlphaTexturing : public SBrush
{

public:
	bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)override;
	//bool Set(const STexture* pTargetTexture, float fWidth, float fHeight)override;
	bool Draw(ID3D11DeviceContext* pContext, PC_VERTEX TargetVertex, float fDistance, float fSmoothSize, DWORD RSNumber)override;
	bool Release()override;
public:
	SAlphaTexturing();
	virtual ~SAlphaTexturing();
};

