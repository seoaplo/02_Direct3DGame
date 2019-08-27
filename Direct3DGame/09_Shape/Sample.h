#pragma once

#include "S3DGameLib.h"
#include "SDXPlaneObject.h"
#include "SCamera.h"

class Sample : public SCore
{
public:
	SDXPlaneObject	TestObject;
	std::vector<UINT>	m_Tex;
	T_STR				m_DebugString;
	
	SCamera				m_CameraBack;
	SCamera				m_CameraSide;
	SCamera*			m_pMainCamera;
	D3DXMATRIX			m_matObjWorld;

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

