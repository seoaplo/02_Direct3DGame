#pragma once
#include "SCamera.h"

class SModelCamera : public SCamera
{
public:
	//======================================================================================
	// ���Ϸ� ���� ���� �� ��� ��� 
	//======================================================================================
	D3DXMATRIX			Update(D3DXVECTOR4 vDirValue = D3DXVECTOR4(0, 0, 0, 0));
public:
	SModelCamera();
	~SModelCamera();
};

