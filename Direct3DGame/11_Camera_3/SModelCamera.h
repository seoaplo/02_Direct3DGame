#pragma once
#include "SCamera.h"

class SModelCamera : public SCamera
{
public:
	//======================================================================================
	// 오일러 각에 따른 뷰 행렬 계산 
	//======================================================================================
	D3DXMATRIX			Update(D3DXVECTOR4 vDirValue = D3DXVECTOR4(0, 0, 0, 0));
public:
	SModelCamera();
	~SModelCamera();
};

