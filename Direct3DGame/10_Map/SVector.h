#pragma once
#include "S3DGameLib.h"


#define SBASIS_EPSILON ((float)0.001f)
#define SBASIS_PI ((float)3.141592654f)
#define DegreeToRandian(degree) (degree*(TBASIS_PI/180.0f ))
#define RadianToDegree(radian) (radian*(180.0f/SBASIS_PI ))

namespace DXGame
{
	class SVector2
	{
	public:
		union
		{
			struct
			{
				float x;
				float y;
			};
			float v[2];
		};
	public:
		SVector2();
		SVector2(const SVector2& vSource);
		SVector2(float fx, float fy);
		~SVector2();
	};
	class SVector4
	{
	public:
		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};
			float v[4];
		};
	public:
		SVector4();
		SVector4(const SVector4& vSource);
		SVector4(float fx, float fy, float fz, float fw);
		~SVector4();
	};
	class SVector3
	{
	public:
		union
		{
			struct
			{
				float x;
				float y;
				float z;
			};
			float v[3];
		};
	public:
		//===============================================================================
		// This Func
		//===============================================================================
		void	Nomalize();
		float   Length();
		float   LengthSquared();
		float   Angle(SVector3& v);
	public:
		//===============================================================================
		// Vector Operator
		//===============================================================================
		SVector3	operator+(const SVector3& vSource);
		SVector3	operator-(const SVector3& vSource);
		SVector3	operator^(const SVector3& vSource);
		float		operator|(const SVector3& vSource);
		bool		operator == (SVector3 const &v);
	public:
		//===============================================================================
		// Scalar Operator
		//===============================================================================
		SVector3 operator*(const float& fSource);
		SVector3 operator/(const float& fSource);

	public:
		SVector3();
		SVector3(const SVector3& vSource);
		SVector3(float fx, float fy, float fz);
		~SVector3();
	};
	
}