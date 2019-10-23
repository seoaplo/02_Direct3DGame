#pragma once
#include "S3DGameLib.h"

#define AnimationMatrixList 255
struct SScene
{
	int   iFirstFrame;  // 0
	int   iLastFrame;   // 100
	int   iFrameSpeed; // 1 Scecond = 30 Frame
	int   iTickPerFrame; // 1 Frame = 160 Tick
	int   iNumObjects;
	int   iNumMaterials;
};

struct BoundingBox
{

};

struct SAnimTrack
{
	int					iTick;
	D3DXVECTOR3			Position;
	D3DXQUATERNION		Quaternion;
};

struct IW_VERTEX
{
	D3DXVECTOR4		I1;
	D3DXVECTOR4		W1;
	D3DXVECTOR4		I2;
	D3DXVECTOR4		W2;
	IW_VERTEX()
	{
		ZeroMemory(this, sizeof(IW_VERTEX));
	}
};
