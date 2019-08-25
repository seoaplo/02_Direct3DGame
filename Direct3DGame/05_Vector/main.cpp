#include "SVector.h"
#include <time.h>
#include <random>

void main()
{
	srand(time(NULL));

	float x = rand() % 100;
	float y = rand() % 100;
	float z = rand() % 100;

	D3DXVECTOR3	DirectVector;
	D3DXVECTOR3	DirectVector2;
	SVector		MyVector;
	SVector		MyVector2;

	ZeroMemory(&DirectVector, sizeof(D3DXVECTOR3));
	ZeroMemory(&MyVector, sizeof(SVector));

	ZeroMemory(&DirectVector2, sizeof(D3DXVECTOR3));
	ZeroMemory(&MyVector2, sizeof(SVector));

	DirectVector.x = x;
	DirectVector.y = y;
	DirectVector.z = z;

	MyVector.x = x;
	MyVector.y = y;
	MyVector.z = z;

	x = rand() % 100;
	y = rand() % 100;
	z = rand() % 100;

	DirectVector2.x = x;
	DirectVector2.y = y;
	DirectVector2.z = z;

	MyVector2.x = x;
	MyVector2.y = y;
	MyVector2.z = z;

	float fTest;

	fTest = D3DXVec3Dot(&DirectVector, &DirectVector2);

	std::cout << "DirectMath Dot = " << fTest << std::endl;

	fTest = MyVector | MyVector2;

	std::cout << "MyMath Dot = " << fTest << std::endl;

	D3DXVECTOR3 D3DRetVector;
	SVector MyRetVector;

	D3DXVec3Cross(&D3DRetVector, &DirectVector, &DirectVector2);
	MyRetVector = MyVector ^ MyVector2;

	std::cout << "DirectMath Cross = (" << D3DRetVector.x << ", " << D3DRetVector.y << ", " << D3DRetVector.z  << ")"<< std::endl;
	std::cout << "MyMath Cross = (" << MyRetVector.x << ", " << MyRetVector.y << ", " << MyRetVector.z << ")" << std::endl;

	return;
}