#pragma once

#include "SDXObject.h"

class Sample : public SCore
{
public:
	SDXObject TestObject;
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

