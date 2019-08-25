#include "SVector.h"

namespace DXGame
{
	SVector2::SVector2()
	{
	}
	SVector2::SVector2(const SVector2& vSource)
	{
		x = vSource.x;
		y = vSource.y;
	}
	SVector2::SVector2(float fx, float fy)
	{
		x = fx;
		y = fy;
	}
	SVector2::~SVector2()
	{
	}

	SVector4::SVector4()
	{
	}
	SVector4::SVector4(const SVector4& vSource)
	{
		x = vSource.x;
		y = vSource.y;
		z = vSource.z;
		w = vSource.w;
	}
	SVector4::SVector4(float fx, float fy, float fz, float fw)
	{
		x = fx;
		y = fy;
		z = fz;
		w = fw;
	}
	SVector4::~SVector4()
	{
	}

	void SVector3::Nomalize()
	{
		float InvertLength = 1.0f / Length();

		x *= InvertLength;
		y *= InvertLength;
		z *= InvertLength;
	}
	float	SVector3::Length()
	{
		return sqrt((x * x) + (y * y) + (z * z));
	}
	float	SVector3::LengthSquared()
	{
		return (x * x) + (y * y) + (z * z);
	}

	//===============================================================================
	// Vector Operator
	//===============================================================================
	SVector3 SVector3::operator+(const SVector3& vSource)
	{
		SVector3 RetVector;
		RetVector.x = x + vSource.x;
		RetVector.y = y + vSource.y;
		RetVector.z = z + vSource.z;

		return RetVector;
	}
	SVector3 SVector3::operator-(const SVector3& vSource)
	{
		SVector3 RetVector;
		RetVector.x = x - vSource.x;
		RetVector.y = y - vSource.y;
		RetVector.z = z - vSource.z;

		return RetVector;
	}

	float SVector3::operator|(const SVector3& vSource)
	{
		float RetScalar;
		RetScalar = (x * vSource.x) + (y * vSource.y) + (z * vSource.z);

		return RetScalar;
	}
	SVector3 SVector3::operator^(const SVector3& vSource)
	{
		SVector3 RetVector;

		RetVector.x = (y * vSource.z) - (z * vSource.y);
		RetVector.y = (z * vSource.x) - (x * vSource.z);
		RetVector.z = (x * vSource.y) - (y * vSource.x);
		return RetVector;
	}
	bool SVector3::operator== (const SVector3 &vSource)
	{
		if (fabs(x - vSource.x) < SBASIS_EPSILON)
		{
			if (fabs(y - vSource.y) < SBASIS_EPSILON)
			{
				if (fabs(z - vSource.z) < SBASIS_EPSILON)
				{
					return true;
				}
			}
		}
		return false;
	}


	//===============================================================================
	// Scala Operator
	//===============================================================================

	SVector3 SVector3::operator*(const float& fSource)
	{
		SVector3 RetVector;
		RetVector.x = x + fSource;
		RetVector.y = y + fSource;
		RetVector.z = z + fSource;

		return RetVector;
	}
	SVector3 SVector3::operator/(const float& fSource)
	{
		x = x / fSource;
		y = y / fSource;
		z = z / fSource;

		return *this;
	}


	SVector3::SVector3()
	{
	}

	SVector3::SVector3(const SVector3& vSource)
	{
		x = vSource.x;
		y = vSource.y;
		z = vSource.z;
	}
	SVector3::SVector3(float fx, float fy, float fz)
	{
		x = fx;
		y = fy;
		z = fz;
	}

	SVector3::~SVector3()
	{
	}

}
