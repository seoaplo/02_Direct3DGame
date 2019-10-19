﻿#pragma once

#include <istdplug.h>
#include <iparamb2.h>
#include <iparamm2.h>
#include <maxtypes.h>
#include <utilapi.h>
#include <max.h>
#include <stdmat.h>
#include <decomp.h>
#include <iskin.h>
#include <bipexp.h>
#include <phyexp.h>

#include <windows.h>
#include <string>
#include <vector>

#define CTL_CHARS			31
#define SINGLE_QUOTE		39 // ( ' )
#define ALMOST_ZERO			1.0e-3f
#define SUBMATERIAL_SIZE	15
#define MAX_WEIGHT_BIPED	8

const enum MaterialTextureType
{
	TexType_AmbientColor = 0,
	TexType_DiffuseColor,
	TexType_SpecularColor,
	TexType_SpecularLevel,
	TexType_Glossiness,
	TexType_Self_IIIumination,
	TexType_Opacity,
	TexType_FilterColor,
	TexType_Bump,
	TexType_Reflection1,
	TexType_Reflection2,
	TexType_Displacement,
	TexType_Number,
};

const enum OBJECTCLASSTYPE {
	CLASS_GEOM = 0,
	CLASS_BONE,
	CLASS_DUMMY,
	CLASS_BIPED,
};

typedef struct _D3D_MATRIX
{
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float m[4][4];
	};
}D3D_MATRIX, *LPD3D_MATRIX;

struct PNCT
{
	Point3 p;
	Point3 n;
	Point4 c;
	Point2 t;
	PNCT()
	{

	}
	PNCT(Point3 ip, Point3 in, Point4 ic, Point2 it)
	{
		p = ip;
		n = in;
		c = ic;
		t = it;
	}
	~PNCT()
	{

	}
};

struct SCATextureMap
{
	bool	bUse;
	TSTR	szName;
	SCATextureMap()
	{
		bUse = false;
	}
};

struct SCASubMaterial
{
	bool	bUse;
	TSTR	szName;
	int		iTextureNumSize;
	SCATextureMap TextureMapList[TexType_Number];
	SCASubMaterial()
	{
		bUse = false;
		iTextureNumSize = 0;
	}
};

struct SCAMaterial
{
	Mtl*	pMaterial;
	int		iMaterialID;
	TSTR	szName;

	int SubMaterialNum;
	SCASubMaterial	SubMaterialList[SUBMATERIAL_SIZE];
	SCAMaterial()
	{
		pMaterial = nullptr;
		iMaterialID -1;
	}
};

struct SOATriangle
{
	int    iSubIndex;
	PNCT   v[3];
	SOATriangle()
	{
		iSubIndex = -1;
	}
};

struct SOASubMesh
{
	bool bUse;
	std::vector<PNCT>			VertexList;
	std::vector<DWORD>			IndexList;
	SOASubMesh()
	{
		bUse = false;
	}
};

struct SOAMesh
{
	int				iMaterialID;
	int				iSubNum;
	Box3			m_box;
	SOASubMesh		SubMeshList[SUBMATERIAL_SIZE];
	SOAMesh()
	{
		iMaterialID = -1;
		iSubNum = 0;
	}
};


struct SCAPositionAnim
{
	int     i;
	Point3  p;
};

struct SCARotationAnim
{
	int     i;
	Quat    q;
};

struct SCAScaleAnim
{
	int     i;
	Point3  p;
	Quat    q;
};

struct SCAAnimationTrack
{
	bool	bPosition;
	bool	bRotation;
	bool	bScale;

	std::vector<SCAPositionAnim>	PositionTrack;
	std::vector<SCARotationAnim>	RotationTrack;
	std::vector<SCAScaleAnim>		ScaleTrack;
	SCAAnimationTrack()
	{
		bPosition = false;
		bRotation = false;
		bScale = false;
	}
};

struct SOAObject
{
	INode*  pINode;
	int		m_ClassType;
	int		iIndex;
	TSTR    name;
	TSTR    ParentName;

	Matrix3    wtm;
	D3D_MATRIX matWorld;
	D3D_MATRIX matInvWorld;
	
	SOAMesh			m_Mesh;
	SCAAnimationTrack	m_AnimTrack;

	SOAObject()
	{
		name = L"none";
		ParentName = L"none";
	}
};

struct SCAScene
{
	int   iFirstFrame;  // 0
	int   iLastFrame;   // 100
	int   iFrameSpeed; // 1 Scecond = 30 Frame
	int   iTickPerFrame; // 1 Frame = 160 Tick
	int   iNumObjects;
	int   iNumMaterials;
};


struct SCABiped
{
	BYTE BipID;
	float m_fWeight;
};
// ─────────────────────────────────
// 정점당 1개 씩 할당필요.
// ─────────────────────────────────
struct SCABipedVertex
{
	int					m_dwNumWeight;
	std::vector<SCABiped>		m_BipedList;
};
// ─────────────────────────────────
//  쉐이더 사용시( 가중치 8개 제한 사용 )
// ─────────────────────────────────
struct PNCTIW_VERTEX
{
	Point3		p;
	Point3		n;
	Point4		c;
	Point2		t;
	float		w[8];	// W0, fW1, fW2, fNumWeight;
	float		i[8];	// I0, I1, I2, I3

	PNCTIW_VERTEX()
	{
		memset(this, -1.0f, sizeof(PNCTIW_VERTEX));
	}
	PNCTIW_VERTEX(Point3 Pos, Point3 Normal, Point4 Color, Point2 Coord)
	{
		p = Pos;
		n = Normal;
		c = Color;
		t = Coord;

	}
	PNCTIW_VERTEX(Point3 Pos, Point3 Normal, Point4 Color, Point2 Coord,
		float w0, float w1, float w2, float w3, float w4, float w5, float w6, float w7,
		float i0, float i1, float i2, float i3, float i4, float i5, float i6, float i7)
	{
		p = Pos;
		n = Normal;
		c = Color;
		t = Coord;

		w[0] = w0; 	w[1] = w1;	w[2] = w2;	w[3] = w3;
		w[4] = w4;	w[5] = w5;	w[6] = w6;	w[7] = w7;

		i[0] = i0; 	i[1] = w1;	i[2] = i2;	i[3] = i3;
		i[4] = i4;	i[5] = w5;	i[6] = i6;	i[7] = i7;
	}	
};

struct SCATriangle
{
	int    iSubIndex;
	PNCTIW_VERTEX   v[3];
	SCATriangle()
	{
		iSubIndex = -1;
	}
};

struct SCASubMesh
{
	bool bUse;
	std::vector<PNCTIW_VERTEX>			VertexList;
	std::vector<DWORD>					IndexList;
	SCASubMesh()
	{
		bUse = false;
	}
};

struct SCAMesh
{
	int				iMaterialID;
	int				iSubNum;
	Box3			m_box;
	SCASubMesh		SubMeshList[SUBMATERIAL_SIZE];
	SCAMesh()
	{
		iMaterialID = -1;
		iSubNum = 0;
	}
};

struct SCAObject
{
	INode*  pINode;
	int		m_ClassType;
	int		iIndex;

	TSTR    name;
	TSTR    ParentName;

	Matrix3    wtm;
	D3D_MATRIX matWorld;
	D3D_MATRIX matInvWorld;

	SCAMesh			m_Mesh;
	SCAAnimationTrack	m_AnimTrack;

	SCAObject()
	{
		name = L"none";
		ParentName = L"none";
	}
};