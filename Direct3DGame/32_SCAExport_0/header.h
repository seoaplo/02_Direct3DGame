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
#include <bipexp.h>
#include <phyexp.h>

#include <windows.h>
#include <string>
#include <vector>
#include <map>

#define CTL_CHARS			31
#define SINGLE_QUOTE		39 // ( ' )
#define ALMOST_ZERO			1.0e-3f
#define SUBMATERIAL_SIZE	15
#define MAX_WEIGHT_BIPED	8
#define BOX_VERTEX			24
#define BOX_INDEX			36

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

struct SAScene
{
	int   iFirstFrame;  // 0
	int   iLastFrame;   // 100
	int   iFrameSpeed; // 1 Scecond = 30 Frame
	int   iTickPerFrame; // 1 Frame = 160 Tick
	int   iNumObjects;
	int   iNumMaterials;
};

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

struct SATextureMap
{
	bool	bUse;
	TSTR	szName;
	SATextureMap()
	{
		bUse = false;
	}
};

struct SASubMaterial
{
	TSTR	szName;
	int		iTextureNumSize;
	SATextureMap TextureMapList[TexType_Number];
	SASubMaterial()
	{
		iTextureNumSize = 0;
	}
};

struct SAMaterial
{
	Mtl*	pMaterial;
	int		iMaterialID;
	TSTR	szName;

	int SubMaterialNum;
	std::vector<SASubMaterial>	SubMaterialList;
	SAMaterial()
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

struct SOATriangleList
{
	int iSize;
	std::vector<SOATriangle> List;
	SOATriangleList()
	{
		iSize = 0;
	}
	~SOATriangleList()
	{
		List.clear();
	}
};

struct SOASubMesh
{
	int iVertexSize;
	int iIndexSize;
	std::vector<PNCT>			VertexList;
	std::vector<DWORD>			IndexList;

	SOASubMesh()
	{
		iVertexSize = 0;
		iIndexSize = 0;
	}
};

struct SOAMesh
{
	int				iMaterialID;
	int				iSubNum;
	Box3			m_box;

	std::vector<SOASubMesh>	SubMeshList;

	SOAMesh()
	{
		iMaterialID = -1;
		iSubNum = 0;
	}
};


struct SAPositionAnim
{
	int     i;
	Point3  p;
};

struct SARotationAnim
{
	int     i;
	Quat    q;
};

struct SAScaleAnim
{
	int     i;
	Point3  p;
	Quat    q;
};

struct SAAnimationTrack
{
	bool	bPosition;
	bool	bRotation;
	bool	bScale;

	std::vector<SAPositionAnim>	PositionTrack;
	std::vector<SARotationAnim>	RotationTrack;
	std::vector<SAScaleAnim>		ScaleTrack;
	SAAnimationTrack()
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

	TSTR    name;
	TSTR    ParentName;

	Matrix3    wtm;
	D3D_MATRIX matWorld;
	D3D_MATRIX InvmatWorld;

	SOAMesh			m_Mesh;
	SAAnimationTrack	m_AnimTrack;


	SOAObject()
	{
		name = L"none";
		ParentName = L"none";
	}
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
		w[0] = -1.0f;		w[1] = -1.0f;		w[2] = -1.0f;		w[3] = -1.0f;
		w[4] = -1.0f;		w[5] = -1.0f;		w[6] = -1.0f;		w[7] = -1.0f;

		i[0] = -1.0f;		i[1] = -1.0f;		i[2] = -1.0f;		i[3] = -1.0f;
		i[4] = -1.0f;		i[5] = -1.0f;		i[6] = -1.0f;		i[7] = -1.0f;
	}
	PNCTIW_VERTEX(Point3 Pos, Point3 Normal, Point4 Color, Point2 Coord)
	{
		p = Pos;
		n = Normal;
		c = Color;
		t = Coord;

		w[0] = -1.0f;		w[1] = -1.0f;		w[2] = -1.0f;		w[3] = -1.0f;
		w[4] = -1.0f;		w[5] = -1.0f;		w[6] = -1.0f;		w[7] = -1.0f;

		i[0] = -1.0f;		i[1] = -1.0f;		i[2] = -1.0f;		i[3] = -1.0f;
		i[4] = -1.0f;		i[5] = -1.0f;		i[6] = -1.0f;		i[7] = -1.0f;
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

struct SCATriangleList
{
	int iSize;
	std::vector<SCATriangle> List;
	SCATriangleList()
	{
		iSize = 0;
	}
	~SCATriangleList()
	{
		List.clear();
	}
};

struct SCASubMesh
{
	int iVertexSize;
	int iIndexSize;
	std::vector<PNCTIW_VERTEX>			VertexList;
	std::vector<DWORD>			IndexList;

	SCASubMesh()
	{
		iVertexSize = 0;
		iIndexSize = 0;
	}
};

struct SCAMesh
{
	int				iMaterialID;
	int				iSubNum;
	Box3			m_box;

	std::vector<SCASubMesh>	SubMeshList;

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
	SAAnimationTrack	m_AnimTrack;

	SCAObject()
	{
		name = L"none";
		ParentName = L"none";
	}
};

typedef std::map<INode*, DWORD> NodeList;