#pragma once

#include <istdplug.h>
#include <iparamb2.h>
#include <iparamm2.h>
#include <maxtypes.h>
#include <utilapi.h>
#include <max.h>
#include <stdmat.h>
#include <decomp.h>
#include <iskin.h>

//#include <bipexp.h>
//#include <phyexp.h>

#include <windows.h>
#include <string>
#include <vector>

#define CTL_CHARS			31
#define SINGLE_QUOTE		39 // ( ' )
#define ALMOST_ZERO			1.0e-3f
#define SUBMATERIAL_SIZE	15

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

struct STextureMap
{
	bool	bUse;
	TSTR	szName;
	STextureMap()
	{
		bUse = false;
	}
};

struct SubMaterial
{
	bool	bUse;
	TSTR	szName;
	int		iTextureNumSize;
	STextureMap TextureMapList[TexType_Number];
	SubMaterial()
	{
		bUse = false;
		iTextureNumSize = 0;
	}
};

struct SMaterial
{
	Mtl*	pMaterial;
	int		iMaterialID;
	TSTR	szName;

	int SubMaterialNum;
	SubMaterial	SubMaterialList[SUBMATERIAL_SIZE];
	SMaterial()
	{
		pMaterial = nullptr;
		iMaterialID -1;
	}
};

struct STriangle
{
	int    iSubIndex;
	PNCT   v[3];
	STriangle()
	{
		iSubIndex = -1;
	}
};

struct SSubMesh
{
	bool bUse;
	std::vector<PNCT>			VertexList;
	std::vector<DWORD>			IndexList;
	SSubMesh()
	{
		bUse = false;
	}
};

struct SMesh
{
	int				iMaterialID;
	int				iSubNum;
	SSubMesh		SubMeshList[SUBMATERIAL_SIZE];
	SMesh()
	{
		iMaterialID = -1;
		iSubNum = 0;
	}
};


struct SPositionAnim
{
	int     i;
	Point3  p;
};

struct SRotationAnim
{
	int     i;
	Quat    q;
};

struct SScaleAnim
{
	int     i;
	Point3  p;
	Quat    q;
};

struct SAnimationTrack
{
	bool	bPosition;
	bool	bRotation;
	bool	bScale;

	std::vector<SPositionAnim>	PositionTrack;
	std::vector<SRotationAnim>	RotationTrack;
	std::vector<SScaleAnim>		ScaleTrack;
	SAnimationTrack()
	{
		bPosition = false;
		bRotation = false;
		bScale = false;
	}
};

struct SObject
{
	INode*  pINode;
	int		m_ClassType;

	TSTR    name;
	TSTR    ParentName;

	Matrix3    wtm;
	D3D_MATRIX matWorld;
	
	SMesh			m_Mesh;
	SAnimationTrack	m_AnimTrack;

	SObject()
	{
		name = L"none";
		ParentName = L"none";
	}
};

struct SScene
{
	int   iFirstFrame;  // 0
	int   iLastFrame;   // 100
	int   iFrameSpeed; // 1 Scecond = 30 Frame
	int   iTickPerFrame; // 1 Frame = 160 Tick
	int   iNumObjects;
	int   iNumMaterials;
};