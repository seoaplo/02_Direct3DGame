#pragma once

#include "S3DGameLib.h"
#include "SCamera.h"

class Sample : public SCore
{
public:
	SBox				TestObject;
	SDirection			m_TestDir;
	SPlaneObject		m_Plane;

	D3DXVECTOR3 XDir;
	D3DXVECTOR3 YDir;
	D3DXVECTOR3 ZDir;
	std::vector<UINT>	m_Tex;
	T_STR				m_DebugString;
	
	SCamera				m_CameraBack;
	SCamera				m_CameraSide;
	SCamera				m_CameraUp;
	SCamera*			m_pMainCamera;

	D3DXMATRIX			m_matObjWorld;

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

