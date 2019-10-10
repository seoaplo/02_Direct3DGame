#pragma once

#include "S3DGameLib.h"
#include "SCamera.h"
#include "SModelViewCamera.h"
#include "SDXShape.h"
#include "SMesh.h"
#include "SSSFileLoader.h"

class Sample : public SCore
{
	friend class Singleton<SSSFileLoader>;
	friend class SSSFileLoader;
public:
	//===================================================================
	// 각종 지원 객체
	//===================================================================
	shared_ptr<SCamera > m_pMainCamera;
	
	DXGame::SDxHelperEX		dxObj;
	bool					m_bDebugRender;
	D3DXMATRIX				m_matInitWorld;
	D3DXMATRIX				m_matWorld;

	vector<vector<PNCT_VERTEX>> VertexList;
public:
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

	//======================================================================================
	// Load Export Data
	//======================================================================================
	HRESULT					CreateConstantBuffer();
	int WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
public:
	Sample();
	virtual ~Sample();
};

