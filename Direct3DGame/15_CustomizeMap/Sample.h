#pragma once

#include "S3DGameLib.h"
#include "SCamera.h"
#include "SModelViewCamera.h"
#include "SMiniMap.h"
#include "SMap.h"


class Sample : public SCore
{
public:
	//===================================================================
	// ���� ���� ��ü
	//===================================================================
	SMap			m_CustomMap;
	SCamera			m_MainCamera;
	SLine			m_LineDraw;
	//SMiniMap		m_MiniMap;
public:
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

