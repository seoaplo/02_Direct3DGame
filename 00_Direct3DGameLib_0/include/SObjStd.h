#pragma once
#include "SDxHelperEX.h"
#include "SDxState.h"
#include "SUtils.h"
#include <memory>

//--------------------------------------------------------------------------------------
// Collision Structure
//--------------------------------------------------------------------------------------

struct S_PLANE
{
	float fA, fB, fC, fD;
	bool	CreatePlane(D3DXVECTOR3 v0, D3DXVECTOR3 v1, D3DXVECTOR3 v2);
	bool	CreatePlane(D3DXVECTOR3 vNormal, D3DXVECTOR3 v0);
	void	Normalize()
	{
		float fMag = sqrt(fA * fA + fB * fB + fC * fC);
		fA /= fMag;
		fB /= fMag;
		fC /= fMag;
		fD /= fMag;
	}
};

struct S_SPHERE
{
	D3DXVECTOR3 vCenter;
	float		fRadius;
};

struct S_BOX
{
	// Common
	D3DXVECTOR3 vCenter;
	D3DXVECTOR3 vPos[8];
	// AABB
	D3DXVECTOR3	vMax;
	D3DXVECTOR3	vMin;
	// OBB
	D3DXVECTOR3	vAxis[3];
	float		fExtent[3];
};

//--------------------------------------------------------------------------------------
// Shader Structure
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

struct PNCT2_VERTEX
{
	D3DXVECTOR3		p;
	D3DXVECTOR3		n;
	D3DXVECTOR4		c;
	D3DXVECTOR2     t;
	D3DXVECTOR3		vTangent;
	bool operator == (const PNCT2_VERTEX & Vertex)
	{
		if (p == Vertex.p  && n == Vertex.n && 	c == Vertex.c   &&	vTangent == Vertex.vTangent)
		{
			return true;
		}
		return  false;
	}
	PNCT2_VERTEX() {}
	PNCT2_VERTEX(D3DXVECTOR3		vp,
		D3DXVECTOR3		vn,
		D3DXVECTOR4		vc,
		D3DXVECTOR2     vt,
		D3DXVECTOR3		tangent)
	{
		p = vp, n = vn, c = vc, t = vt, vTangent = tangent;
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
		Rect.Plane_LeftTop.x			= static_cast<float>(TargetRect.left);
		Rect.Plane_LeftBottom.x		= static_cast<float>(TargetRect.left);
		Rect.Plane_RightTop.x		= static_cast<float>(TargetRect.right);
		Rect.Plane_RightBottom.x		= static_cast<float>(TargetRect.right);

		Rect.Plane_LeftTop.y		= static_cast<float>(TargetRect.top);
		Rect.Plane_RightTop.y	= static_cast<float>(TargetRect.top);
		Rect.Plane_LeftBottom.y	= static_cast<float>(TargetRect.bottom);
		Rect.Plane_RightBottom.y = static_cast<float>(TargetRect.bottom);

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
		Rect.Plane_LeftTop.t.x = static_cast<float>(TargetTexCoordRect.left);
		Rect.Plane_LeftBottom.t.x = static_cast<float>(TargetTexCoordRect.left);
		Rect.Plane_RightTop.t.x = static_cast<float>(TargetTexCoordRect.right);
		Rect.Plane_RightBottom.t.x = static_cast<float>(TargetTexCoordRect.right);

		Rect.Plane_LeftTop.t.y = static_cast<float>(TargetTexCoordRect.top);
		Rect.Plane_RightTop.t.y = static_cast<float>(TargetTexCoordRect.top);
		Rect.Plane_LeftBottom.t.y = static_cast<float>(TargetTexCoordRect.bottom);
		Rect.Plane_RightBottom.t.y = static_cast<float>(TargetTexCoordRect.bottom);


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

//--------------------------------------------------------------------------------------
// Shape Structure
//--------------------------------------------------------------------------------------

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
		// ¾Õ¸é
		Vertex[0] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
		Vertex[1] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
		Vertex[2] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
		Vertex[3] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));
		// µÞ¸é
		Vertex[4] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
		Vertex[5] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
		Vertex[6] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
		Vertex[7] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

		// ¿À¸¥ÂÊ
		Vertex[8] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
		Vertex[9] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
		Vertex[10] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
		Vertex[11] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

		// ¿ÞÂÊ
		Vertex[12] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
		Vertex[13] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
		Vertex[14] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
		Vertex[15] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

		// À­¸é
		Vertex[16] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
		Vertex[17] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
		Vertex[18] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
		Vertex[19] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

		// ¾Æ·§¸é
		Vertex[20] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
		Vertex[21] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
		Vertex[22] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
		Vertex[23] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

	}
	~BOX_VERTEX_LIST()
	{

	}
};