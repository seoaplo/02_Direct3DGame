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
	};
}D3D_MATRIX, *LPD3D_MATRIX;

struct PNCT
{
	Point3 p;
	Point3 n;
	Point4 c;
	Point2 t;
};

struct Triangle
{
	PNCT   v[3];
};

class SSSWriter
{
	Interface*				m_pMax;				// Max Interface
	std::wstring			m_filename;			// file path
	INode*					m_pRootNode;		// Interface Node
	std::vector<INode*>		m_ObjectList;		// Node List
	std::vector<Triangle>	m_triList;			// Triangle List
public:
	void  Set(const TCHAR* name, Interface* pMax);
	bool  Export();
	void  PreProcess(INode* pNode);
	void  AddObject(INode* pNode);
	void  GetMesh(INode* pNode);
	TriObject* GetTriObjectFromNode(INode* pNode, TimeValue time, bool& deleteit);
public:
	//=============================================================================
	// Util
	//=============================================================================

	void SSSWriter::DumpMatrix3(D3D_MATRIX& mat, Matrix3* m)
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
	void SSSWriter::DumpPoint3(Point3& pDesc, Point3& pSrc)
	{
		pDesc.x = pSrc.x;
		pDesc.y = pSrc.z;
		pDesc.z = pSrc.y;
	}
	bool SSSWriter::EqualPoint4(Point4& p1, Point4& p2)
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
	bool SSSWriter::EqualPoint3(Point3& p1, Point3& p2)
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
	bool SSSWriter::EqualPoint2(Point2& p1, Point2& p2)
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

