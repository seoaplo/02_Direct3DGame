#pragma once

#include "S3DGameLib.h"
#include "SDXPlaneObject.h"

class Sample : public SCore
{
public:
	SDXPlaneObject TestObject;
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

