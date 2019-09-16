#pragma once

#include "S3DGameLib.h"
#include "SCamera.h"
#include "SModelViewCamera.h"
#include "SMiniMap.h"
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
	SCamera			m_MainCamera;
	SDXShape*		m_pBoxShape;
	SMiniMap		m_MiniMap;
	Microsoft::WRL::ComPtr< ID3D11PixelShader > m_pPixelShader;
public:
	//===================================================================
	// Matrix
	//===================================================================
	D3DXMATRIX		m_matBoxWorld;
	D3DXVECTOR3		m_vBoxPosition[100];
	D3DXVECTOR4		m_vBoxColor[100];
	//===================================================================
	// ����Ʈ ����
	//===================================================================
	DXGame::SDxRT	m_MiniMapVP;
	//===================================================================
	// ī�޶� ȸ�� ����
	//===================================================================
	float m_fCameraYaw;
	float m_fCameraPitch;
	float m_fCameraRoll;
	float m_fRadius;
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

