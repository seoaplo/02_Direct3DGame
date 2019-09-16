#pragma once
#include "STemplate.h"
#include "SDxHelperEX.h"

struct PLANE_VERTEX_LIST
{
	union
	{
		struct
		{
			PNCT_VERTEX LeftTop;
			PNCT_VERTEX RightTop;
			PNCT_VERTEX RightBottom;
			PNCT_VERTEX LeftBottom;;
		};
		PNCT_VERTEX VERTEX[4];
	};
	PNCT_VERTEX& operator[] (unsigned iNum)
	{
		return VERTEX[iNum];
	}
	PLANE_VERTEX_LIST()
	{
		VERTEX[0].p = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
		VERTEX[0].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		VERTEX[0].c = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		VERTEX[0].t = D3DXVECTOR2(0.0f, 0.0f);

		VERTEX[1].p = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
		VERTEX[1].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		VERTEX[1].c = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		VERTEX[1].t = D3DXVECTOR2(1.0f, 0.0f);

		VERTEX[2].p = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
		VERTEX[2].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		VERTEX[2].c = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		VERTEX[2].t = D3DXVECTOR2(1.0f, 1.0f);

		VERTEX[3].p = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
		VERTEX[3].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		VERTEX[3].c = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		VERTEX[3].t = D3DXVECTOR2(0.0f, 1.0f);
	}
	~PLANE_VERTEX_LIST()
	{

	}
};


struct BOX_VERTEX_LIST
{
	union
	{
		struct
		{
			PLANE_VERTEX_LIST Front;
			PLANE_VERTEX_LIST Back;
			PLANE_VERTEX_LIST Low;
			PLANE_VERTEX_LIST Up;
			PLANE_VERTEX_LIST Left;
			PLANE_VERTEX_LIST Right;
		};
		PLANE_VERTEX_LIST Plane[6];
		PNCT_VERTEX Vertex[24];
	};
	PNCT_VERTEX& operator[] (unsigned iNum)
	{
		return Vertex[iNum];
	}
	BOX_VERTEX_LIST()
	{
		ZeroMemory(this, sizeof(BOX_VERTEX_LIST));
		// Á¤¸é
		Vertex[0].p = D3DXVECTOR3(-1.0f, 1.0f, -1.0f);
		Vertex[0].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[0].c = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		Vertex[0].t = D3DXVECTOR2(0.0f, 0.0f);

		Vertex[1].p = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
		Vertex[1].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[1].c = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		Vertex[1].t = D3DXVECTOR2(1.0f, 0.0f);

		Vertex[2].p = D3DXVECTOR3(1.0f, -1.0f, -1.0f);
		Vertex[2].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[2].c = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		Vertex[2].t = D3DXVECTOR2(1.0f, 1.0f);

		Vertex[3].p = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
		Vertex[3].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[3].c = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		Vertex[3].t = D3DXVECTOR2(0.0f, 1.0f);

		// µÞ¸é
		Vertex[4].p = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		Vertex[4].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[4].c = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		Vertex[4].t = D3DXVECTOR2(0.0f, 0.0f);

		Vertex[5].p = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
		Vertex[5].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[5].c = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		Vertex[5].t = D3DXVECTOR2(1.0f, 0.0f);

		Vertex[6].p = D3DXVECTOR3(-1.0f, -1.0f, 1.0f);
		Vertex[6].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[6].c = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		Vertex[6].t = D3DXVECTOR2(1.0f, 1.0f);

		Vertex[7].p = D3DXVECTOR3(1.0f, -1.0f, 1.0f);
		Vertex[7].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[7].c = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		Vertex[7].t = D3DXVECTOR2(0.0f, 1.0f);

		// ¾Æ·§¸é
		Vertex[8].p = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
		Vertex[8].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[8].c = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		Vertex[8].t = D3DXVECTOR2(0.0f, 0.0f);

		Vertex[9].p = D3DXVECTOR3(1.0f, -1.0f, -1.0f);
		Vertex[9].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[9].c = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		Vertex[9].t = D3DXVECTOR2(1.0f, 0.0f);

		Vertex[10].p = D3DXVECTOR3(1.0f, -1.0f, 1.0f);
		Vertex[10].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[10].c = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		Vertex[10].t = D3DXVECTOR2(1.0f, 1.0f);

		Vertex[11].p = D3DXVECTOR3(-1.0f, -1.0f, 1.0f);
		Vertex[11].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[11].c = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		Vertex[11].t = D3DXVECTOR2(0.0f, 1.0f);

		// À­¸é
		Vertex[12].p = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
		Vertex[12].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[12].c = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		Vertex[12].t = D3DXVECTOR2(0.0f, 0.0f);

		Vertex[13].p = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		Vertex[13].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[13].c = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		Vertex[13].t = D3DXVECTOR2(1.0f, 0.0f);

		Vertex[14].p = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
		Vertex[14].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[14].c = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		Vertex[14].t = D3DXVECTOR2(1.0f, 1.0f);

		Vertex[15].p = D3DXVECTOR3(-1.0f, 1.0f, -1.0f);
		Vertex[15].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[15].c = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		Vertex[15].t = D3DXVECTOR2(0.0f, 1.0f);

		// ¿Þ¸é
		Vertex[16].p = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
		Vertex[16].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[16].c = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		Vertex[16].t = D3DXVECTOR2(0.0f, 0.0f);

		Vertex[17].p = D3DXVECTOR3(-1.0f, 1.0f, -1.0f);
		Vertex[17].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[17].c = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		Vertex[17].t = D3DXVECTOR2(1.0f, 0.0f);

		Vertex[18].p = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
		Vertex[18].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[18].c = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		Vertex[18].t = D3DXVECTOR2(1.0f, 1.0f);

		Vertex[19].p = D3DXVECTOR3(-1.0f, -1.0f, 1.0f);
		Vertex[19].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[19].c = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		Vertex[19].t = D3DXVECTOR2(0.0f, 1.0f);


		// ¿À¸¥¸é
		Vertex[20].p = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
		Vertex[20].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[20].c = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		Vertex[20].t = D3DXVECTOR2(0.0f, 0.0f);

		Vertex[21].p = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		Vertex[21].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[21].c = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		Vertex[21].t = D3DXVECTOR2(1.0f, 0.0f);

		Vertex[22].p = D3DXVECTOR3(1.0f, -1.0f, 1.0f);
		Vertex[22].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[22].c = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		Vertex[22].t = D3DXVECTOR2(1.0f, 1.0f);

		Vertex[23].p = D3DXVECTOR3(1.0f, -1.0f, -1.0f);
		Vertex[23].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Vertex[23].c = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		Vertex[23].t = D3DXVECTOR2(0.0f, 1.0f);

	}
	~BOX_VERTEX_LIST()
	{

	}
};