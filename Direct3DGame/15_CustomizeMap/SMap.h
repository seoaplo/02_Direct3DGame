#pragma once
#include "SDXShape.h"

#define RANDOM(n)			(rand() % (n))
#define RANDOMSCOPE(s,e)	(rand() %(e-s)) + s;
#define RANDOMIZE()			srand((unsigned)time(NULL))

struct SMapDesc
{
	int			iNumCols;
	int			iNumRows;
	float		fSellDistance;
	float		fScaleHeight;
	T_STR		strTextureFile;
	T_STR		strShaderFile;
};

class SMap : public SModel
{
public:
	SMap();
	~SMap();
};

