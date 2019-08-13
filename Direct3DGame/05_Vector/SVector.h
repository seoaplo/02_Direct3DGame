#pragma once
#include "S3DGameLib.h"

class SVector
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
	float	Size();
	float	SquredSize();
public:
	//===============================================================================
	// Vector Operator
	//===============================================================================
	SVector operator=(const SVector& vSource);
	SVector operator+(const SVector& vSource);
	SVector operator-(const SVector& vSource);

	float Dot(const SVector& vSource);
	SVector Cross(const SVector& vSource);

public:
	//===============================================================================
	// Scalar Operator
	//===============================================================================
	SVector operator*(const float& fSource);
	SVector operator/(const float& fSource);
	SVector operator-(const float& fSource);
	SVector operator+(const float& fSource);

public:
	SVector();
	~SVector();
};

