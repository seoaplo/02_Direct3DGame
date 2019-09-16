#pragma once

#include "S3DGameLib.h"
#include "SCamera.h"
#include "SModelViewCamera.h"

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
	// ���� ���� ��ü
	//===================================================================
	SBox			m_Box;
	SPlane			m_Plane;
	SDirection		m_Direction;
	//===================================================================
	// ī�޶� ��ü
	//===================================================================
	SCamera			m_Camera[4];
	SModelViewCamera			m_MainCamera;

	UINT						m_iCameraType;
public:
	//===================================================================
	// Matrix
	//===================================================================
	D3DXMATRIX		m_World[2];
	D3DXMATRIX		m_matWorldPlaneBase;
	//===================================================================
	// ī�޶� ȸ�� ����
	//===================================================================
	float m_fCameraYaw;
	float m_fCameraPitch;
	float m_fCameraRoll;
	float m_fRadius;
	//===================================================================
	// ������Ʈ ȸ�� ����
	//===================================================================
	float m_fYaw;
	float m_fPitch;
	float m_fRoll;
	D3DXVECTOR3 m_vObjectPosition;

	// ����Ʈ ����
	DXGame::SDxRT m_ViewPort[4];
public:
	HRESULT	ScreenViewPort(UINT iWidth, UINT iHeight);
	//====================================================================
	// ������ �Լ�
	//====================================================================
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

	//--------------------------------------------------------------------------------------
	// ����� Ŭ���̾�Ʈ ������ �缳���� ���� �Ҹ� �� ����
	//--------------------------------------------------------------------------------------
	HRESULT		CreateResource();
	HRESULT		DeleteResource();	
	
	int WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
public:
	Sample();
	virtual ~Sample();
};

