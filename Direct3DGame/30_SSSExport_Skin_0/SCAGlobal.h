#pragma once
#include "header.h"

class SCAGlobal
{
public:
	static TCHAR*  FixupName(MSTR name)
	{
		static TCHAR m_tmpBuffer[MAX_PATH] = { 0, };
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
	static bool TMNegParity(Matrix3 tm)
	{
		Point3 v0 = tm.GetRow(0);
		Point3 v1 = tm.GetRow(1);
		Point3 v2 = tm.GetRow(2);
		Point3 vCross = CrossProd(v0, v1);
		return (DotProd(vCross, v2) < 0.0f) ? true : false;
	}
	static void DumpMatrix3(D3D_MATRIX& mat, Matrix3* m)
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
	static void DumpPoint3(Point3& pDesc, Point3& pSrc)
	{
		pDesc.x = pSrc.x;
		pDesc.y = pSrc.z;
		pDesc.z = pSrc.y;
	}
	static bool EqualPoint4(Point4& p1, Point4& p2)
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
	static bool EqualPoint3(Point3& p1, Point3& p2)
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
	static bool EqualPoint2(Point2& p1, Point2& p2)
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
	SCAGlobal();
	~SCAGlobal();
};

