#pragma once
#include "SDxHelperEX.h"
#include "SDxState.h"
#include "SUtils.h"
#include <memory>


//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct PCT_VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR4 c;
	D3DXVECTOR2 t;
	bool operator == (const PCT_VERTEX & Vertex)
	{
		if (p == Vertex.p  && c == Vertex.c    && t == Vertex.t)
		{
			return true;
		}
		return  false;
	}
	PCT_VERTEX() {}
	PCT_VERTEX(D3DXVECTOR3		vp,
		D3DXVECTOR4		vc,
		D3DXVECTOR2     vt)
	{
		p = vp, c = vc, t = vt;
	}
};
struct PCT2_VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR4 c;
	D3DXVECTOR2 t0;
	D3DXVECTOR2 t1;
	bool operator == (const PCT2_VERTEX & Vertex)
	{
		if (p == Vertex.p  && c == Vertex.c    && t0 == Vertex.t0 && t1 == Vertex.t1)
		{
			return true;
		}
		return  false;
	}
	PCT2_VERTEX() {}
	PCT2_VERTEX(D3DXVECTOR3		vp,
		D3DXVECTOR4		vc,
		D3DXVECTOR2     vt0,
		D3DXVECTOR2     vt1)
	{
		p = vp, c = vc, t0 = vt0, t1 = vt1;
	}
};
struct PC_VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR4 c;
	bool operator == (const PC_VERTEX & Vertex)
	{
		if (p == Vertex.p  && c == Vertex.c)
		{
			return true;
		}
		return  false;
	}
	PC_VERTEX() {}
	PC_VERTEX(D3DXVECTOR3		vp,
		D3DXVECTOR4		vc)
	{
		p = vp, c = vc;
	}
};
struct PNC_VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	D3DXVECTOR4 c;
	bool operator == (const PNC_VERTEX & Vertex)
	{
		if (p == Vertex.p  && n == Vertex.n && 	c == Vertex.c)
		{
			return true;
		}
		return  false;
	}
	PNC_VERTEX() {}
	PNC_VERTEX(D3DXVECTOR3		vp,
		D3DXVECTOR3		vn,
		D3DXVECTOR4		vc)
	{
		p = vp, n = vn, c = vc;
	}
};
struct PNCT_VERTEX
{
	D3DXVECTOR3		p;
	D3DXVECTOR3		n;
	D3DXVECTOR4		c;
	D3DXVECTOR2     t;
	bool operator == (const PNCT_VERTEX & Vertex)
	{
		if (p == Vertex.p  && n == Vertex.n && 	c == Vertex.c  &&	t == Vertex.t)
		{
			return true;
		}
		return  false;
	}
	PNCT_VERTEX() {}
	PNCT_VERTEX(D3DXVECTOR3		vp,
		D3DXVECTOR3		vn,
		D3DXVECTOR4		vc,
		D3DXVECTOR2     vt)
	{
		p = vp, n = vn, c = vc, t = vt;
	}
};

struct VS_CONSTANT_BUFFER
{
	D3DXMATRIX matWorld;// c0						
	D3DXMATRIX matView;	// c4						
	D3DXMATRIX matProj;	// c8						
	D3DXVECTOR4 Color; // 12
};

struct PLANE_TEXTURECOORD
{
	struct PLANE_RECT
	{
		D3DXVECTOR2 Plane_LeftTop;
		D3DXVECTOR2 Plane_RightTop;
		D3DXVECTOR2 Plane_RightBottom;
		D3DXVECTOR2 Plane_LeftBottom;
	};
	union
	{
		PLANE_RECT	Rect;
		D3DXVECTOR2 Texture_Coord[4];
	};

	D3DXVECTOR2& operator[](unsigned int iNum)
	{
		return Texture_Coord[iNum];
	}

	PLANE_TEXTURECOORD& operator= (PLANE_TEXTURECOORD CoordList)
	{
		Texture_Coord[0] = CoordList[0];
		Texture_Coord[1] = CoordList[1];
		Texture_Coord[2] = CoordList[2];
		Texture_Coord[3] = CoordList[3];

		return *this;
	}
	PLANE_TEXTURECOORD operator=(RECT TargetRect)
	{
		Rect.Plane_LeftTop.x			= TargetRect.left;
		Rect.Plane_LeftBottom.x		= TargetRect.left;
		Rect.Plane_RightTop.x		= TargetRect.right;
		Rect.Plane_RightBottom.x		= TargetRect.right;

		Rect.Plane_LeftTop.y		= TargetRect.top;
		Rect.Plane_RightTop.y	= TargetRect.top;
		Rect.Plane_LeftBottom.y	= TargetRect.bottom;
		Rect.Plane_RightBottom.y = TargetRect.bottom;

		return *this;
	}
	PLANE_TEXTURECOORD() {};
	~PLANE_TEXTURECOORD() {};
};

struct PLANE_PNCTLIST
{
	struct PLANE_RECT
	{
		PNCT_VERTEX Plane_LeftTop;
		PNCT_VERTEX Plane_RightTop;
		PNCT_VERTEX Plane_RightBottom;
		PNCT_VERTEX Plane_LeftBottom;
	};
	union
	{
		PLANE_RECT	Rect;
		PNCT_VERTEX pnct_Vertex[4];
	};

	PNCT_VERTEX& operator[](unsigned int iNum)
	{
		return pnct_Vertex[iNum];
	}

	PLANE_PNCTLIST& operator= (PLANE_PNCTLIST Plane_List)
	{
		pnct_Vertex[0] = Plane_List[0];
		pnct_Vertex[1] = Plane_List[1];
		pnct_Vertex[2] = Plane_List[2];
		pnct_Vertex[3] = Plane_List[3];

		return *this;
	}
	PLANE_PNCTLIST operator=(RECT TargetTexCoordRect)
	{
		Rect.Plane_LeftTop.t.x = TargetTexCoordRect.left;
		Rect.Plane_LeftBottom.t.x = TargetTexCoordRect.left;
		Rect.Plane_RightTop.t.x = TargetTexCoordRect.right;
		Rect.Plane_RightBottom.t.x = TargetTexCoordRect.right;

		Rect.Plane_LeftTop.t.y = TargetTexCoordRect.top;
		Rect.Plane_RightTop.t.y = TargetTexCoordRect.top;
		Rect.Plane_LeftBottom.t.y = TargetTexCoordRect.bottom;
		Rect.Plane_RightBottom.t.y = TargetTexCoordRect.bottom;

		/*D3DXVECTOR2 vCenter;
		vCenter.x = (Rect.Plane_RightBottom.t.x - Rect.Plane_LeftTop.t.x) / 2;
		vCenter.y = (Rect.Plane_RightBottom.t.y - Rect.Plane_LeftTop.t.y) / 2;

		D3DXVec2Normalize(&vCenter, &vCenter);

		Rect.Plane_LeftTop.p.x = -vCenter.x;
		Rect.Plane_LeftTop.p.y = vCenter.y;

		Rect.Plane_RightTop.p.x = vCenter.x;
		Rect.Plane_RightTop.p.y = vCenter.y;

		Rect.Plane_RightBottom.p.x = vCenter.x;
		Rect.Plane_RightBottom.p.y = -vCenter.y;

		Rect.Plane_LeftBottom.p.x = -vCenter.x;
		Rect.Plane_LeftBottom.p.y = -vCenter.y;*/

		return *this;
	}
	PLANE_PNCTLIST()
	{
		pnct_Vertex[0] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
		pnct_Vertex[1] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
		pnct_Vertex[2] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
		pnct_Vertex[3] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));
	};
	~PLANE_PNCTLIST() {};
};

typedef struct
{
	double x, y, z;
} Point3;


