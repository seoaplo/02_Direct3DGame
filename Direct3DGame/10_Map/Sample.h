#pragma once

#include "S3DGameLib.h"
#include "SCamera.h"

//--------------------------------------------------------------------------------------
// CameraViewStyle
//--------------------------------------------------------------------------------------
T_STR CameraViewStyle[] =
{
	_T("Top g_matView"),
	_T("Front g_matView"),
	_T("Side g_matView"),
	_T("User g_matView"),
};


class Sample : public SCore
{
public:
	//===================================================================
	// 각종 지원 객체
	//===================================================================
	SCamera			m_Camera[4];
	SBox			m_Box;
	SPlane			m_Plane;
	SDirection		m_Direction;
public:
	//===================================================================
	// Matrix
	//===================================================================
	D3DXMATRIX		m_World[2];
	D3DXMATRIX		m_View;
	D3DXMATRIX		m_Projection;

	//===================================================================
	// 오브젝트 회전 정보
	//===================================================================
	float m_fYaw;
	float m_fPitch;
	float m_fRoll;
	
	DXGame::SDxRT m_ViewPort[4];
public:
	HRESULT	ScreenViewPort(UINT iWidth, UINT iHeight);
	//====================================================================
	// 재정의 함수
	//====================================================================
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

	//--------------------------------------------------------------------------------------
	// 변경된 클라이언트 영역를 재설정을 위한 소멸 및 생성
	//--------------------------------------------------------------------------------------
	HRESULT		CreateResource();
	HRESULT		DeleteResource();	
	
	
public:
	Sample();
	virtual ~Sample();
};

