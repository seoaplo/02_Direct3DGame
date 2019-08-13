#include "SVector.h"

void SVector::Nomalize()
{
	float fSize = Size();

	x /= fSize;
	y /= fSize;
	z /= fSize;
}
float	SVector::Size()
{
	D3DVECTOR Vec;
	return sqrt((x * x) + (y * y) + (z * z));
}
float	SVector::SquredSize()
{
	return (x * x) + (y * y) + (z * z);
}

//===============================================================================
// Vector Operator
//===============================================================================
SVector SVector::operator=(const SVector& vSource)
{
	SVector RetVector;
	RetVector.x = vSource.x;
	RetVector.y = vSource.y;
	RetVector.z = vSource.z;

	return RetVector;
}
SVector SVector::operator+(const SVector& vSource)
{
	SVector RetVector;
	RetVector.x = x + vSource.x;
	RetVector.y = y + vSource.y;
	RetVector.z = z + vSource.z;

	return RetVector;
}
SVector SVector::operator-(const SVector& vSource)
{
	SVector RetVector;
	RetVector.x = x - vSource.x;
	RetVector.y = y - vSource.y;
	RetVector.z = z - vSource.z;

	return RetVector;
}

float SVector::Dot(const SVector& vSource)
{
	float RetScalar;
	RetScalar = (x * vSource.x) + (y * vSource.y) + (z * vSource.z);

	return RetScalar;
}
SVector SVector::Cross(const SVector& vSource)
{
	SVector RetVector;
	return RetVector;
}

//===============================================================================
// Scala Operator
//===============================================================================

SVector SVector::operator*(const float& fSource)
{
	SVector RetVector;
	RetVector.x = x + fSource;
	RetVector.y = y + fSource;
	RetVector.z = z + fSource;

	return RetVector;
}
SVector SVector::operator/(const float& fSource)
{
	x = x / fSource;
	y = y / fSource;
	z = z / fSource;

	return *this;
}
SVector SVector::operator-(const float& fSource)
{
	x = x - fSource;
	y = y - fSource;
	z = z - fSource;

	return *this;
}
SVector SVector::operator+(const float& fSource)
{
	x = x + fSource;
	y = y + fSource;
	z = z + fSource;

	return *this;
}

SVector::SVector()
{
}


SVector::~SVector()
{
}
