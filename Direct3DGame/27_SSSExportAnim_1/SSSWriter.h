#pragma once
#include <string>
#include <vector>

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
};
struct TriList
{
	int    iSubIndex;
	PNCT   v[3];
};
struct STexMap
{
	int   iMapID;
	TSTR  szName;
};
struct SMtrl
{
	INode*  pINode;
	int   iMtrlID;
	TSTR  szName;
	std::vector<STexMap>  texList;
	std::vector<SMtrl>    subMtrl;
};
struct SMtrlHeader
{
	int    iMtrlID;
	TCHAR  szName[32];
};
typedef std::vector<TriList>	vectorTriList;
typedef std::vector<PNCT>		vertexList;
typedef std::vector<int>		IndexList;

struct SAnimTrack
{
	int     i;
	Point3  p;
	Quat    q;
};

struct SMesh
{
	INode*  pINode;
	int     iSubMesh;
	TSTR    name;
	TSTR    ParentName;
	Matrix3    wtm;
	D3D_MATRIX matWorld;
	std::vector<TriList>			triList;
	std::vector<vectorTriList>		bufferList;

	std::vector<vertexList>   vbList;
	std::vector<IndexList>    ibList;

	bool  bAnimatin;
	std::vector<SAnimTrack>   animPos;
	std::vector<SAnimTrack>   animRot;
	std::vector<SAnimTrack>   animScl;

	int     iMtrlID;
	SMesh()
	{
		name = L"none";
		ParentName = L"none";
		iMtrlID = -1;
		iSubMesh = 1;
	}
};
struct SMeshHeader
{
	int     iSubMesh;
	int     iMtrlID;
	TCHAR    name[32];
	TCHAR    ParentName[32];
	D3D_MATRIX matWorld;
};

struct SScene
{
	int   iFirstFrame;  // 0
	int   iLastFrame;   // 100
	int   iFrameSpeed; // 1 Scecond = 30 Frame
	int   iTickPerFrame; // 1 Frame = 160 Tick
};

class SSSWriter
{
	Interface*				m_pMax;				// Max Interface
	std::wstring			m_filename;			// file path
	FILE*					m_pStream;			// file Pointer
	INode*					m_pRootNode;		// Interface Node
	SScene					m_Scene;			
	Interval				m_Interval;

	std::vector<INode*>		m_ObjectList;		// Node List
	std::vector<SMesh>		m_sMeshList;		// Triangle List

	std::vector<Mtl*>		m_MaterialList;		// Node List
	std::vector<SMtrl>		m_MtrlList;			// Triangle List
public:
	void	Set(const TCHAR* name, Interface* pMax);
	bool	Export();
	void	PreProcess(INode* pNode);
	void	AddObject(INode* pNode);
	void	AddMaterial(INode* pNode);

	bool	 Convert();
	int		FindMaterial(INode* pNode);
	void	SetUniqueBuffer(SMesh& tMesh);
	int		IsEqulVerteList(PNCT& vertex, vertexList& vList);

	void	GetMaterial(INode* pNode);
	void	GetMesh(INode* pNode, SMesh& sMesh);
	void	GetTexture(Mtl* pMtl, SMtrl& sMtrl);
	Point3	GetVertexNormal(Mesh* mesh, int iFace, RVertex* rVertex);
	TriObject* GetTriObjectFromNode(INode* pNode, TimeValue time, bool& deleteit);

	void GetAnimation(
		INode* pNode,
		SMesh& tMesh);
	void ExportAnimation(SMesh& tMesh);
	bool CheckForAnimation(INode* pNode,
		bool&  bPos, bool& bRot, bool& bScl);
	void DumpPosSample(INode* pNode, SMesh& pMesh);
	void DumpRotSample(INode* pNode, SMesh& pMesh);
	void DumpSclSample(INode* pNode, SMesh& pMesh);
public:
	//=============================================================================
	// Util
	//=============================================================================
	TCHAR*  FixupName(MSTR name)
	{
		TCHAR m_tmpBuffer[MAX_PATH] = { 0, };
		memset(m_tmpBuffer, 0, sizeof(TCHAR)*MAX_PATH);

		TCHAR* cPtr;
		_tcscpy_s(m_tmpBuffer, name);
		cPtr = m_tmpBuffer;

		while (*cPtr)
		{
			if (*cPtr == '"')		*cPtr = SINGLE_QUOTE;
			else if (*cPtr == ' ' || *cPtr <= CTL_CHARS)
				*cPtr = _T('_');
			cPtr++;
		}
		return m_tmpBuffer;
	}
	bool TMNegParity(Matrix3 tm)
	{
		Point3 v0 = tm.GetRow(0);
		Point3 v1 = tm.GetRow(1);
		Point3 v2 = tm.GetRow(2);
		Point3 vCross = CrossProd(v0, v1);
		return (DotProd(vCross, v2) < 0.0f) ? true : false;
	}
	void DumpMatrix3(D3D_MATRIX& mat, Matrix3* m)
	{
		Point3 row;
		row = m->GetRow(0);
		mat._11 = row.x; mat._13 = row.y; mat._12 = row.z;

		row = m->GetRow(2);
		mat._21 = row.x; mat._23 = row.y; mat._22 = row.z;

		row = m->GetRow(1);
		mat._31 = row.x; mat._33 = row.y; mat._32 = row.z;

		row = m->GetRow(3);
		mat._41 = row.x; mat._43 = row.y; mat._42 = row.z;

		mat._14 = mat._24 = mat._34 = 0.0f;

		mat._44 = 1.0f;
	}
	void DumpPoint3(Point3& pDesc, Point3& pSrc)
	{
		pDesc.x = pSrc.x;
		pDesc.y = pSrc.z;
		pDesc.z = pSrc.y;
	}
	bool EqualPoint4(Point4& p1, Point4& p2)
	{
		if (fabs(p1.x - p2.x) > ALMOST_ZERO)
		{
			return false;
		}
		if (fabs(p1.y - p2.y) > ALMOST_ZERO)
		{
			return false;
		}
		if (fabs(p1.z - p2.z) > ALMOST_ZERO)
		{
			return false;
		}
		if (fabs(p1.w - p2.w) > ALMOST_ZERO)
		{
			return false;
		}
		return true;
	}
	bool EqualPoint3(Point3& p1, Point3& p2)
	{
		if (fabs(p1.x - p2.x) > ALMOST_ZERO)
		{
			return false;
		}
		if (fabs(p1.y - p2.y) > ALMOST_ZERO)
		{
			return false;
		}
		if (fabs(p1.z - p2.z) > ALMOST_ZERO)
		{
			return false;
		}
		return true;
	}
	bool EqualPoint2(Point2& p1, Point2& p2)
	{
		if (fabs(p1.x - p2.x) > ALMOST_ZERO)
		{
			return false;
		}
		if (fabs(p1.y - p2.y) > ALMOST_ZERO)
		{
			return false;
		}
		return true;
	}
public:
	SSSWriter();
	~SSSWriter();
};

