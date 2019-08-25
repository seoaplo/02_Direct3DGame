#pragma once

#include "S3DGameLib.h"
#include "SDXPlaneObject.h"

class Sample : public SCore
{
public:
	SDXPlaneObject TestObject;
public:
	//====================================================================
	// 재정의 함수
	//====================================================================
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();
public:
	Sample();
	~Sample();
};

