#pragma once

#include "SDXObject.h"

class Sample : public SCore
{
public:
	SDXObject TestObject;
public:
	//====================================================================
	// ������ �Լ�
	//====================================================================
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();
public:
	Sample();
	~Sample();
};

