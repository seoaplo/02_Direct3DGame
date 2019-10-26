#pragma once
#include "S3DGameLib.h"
#include "SCamera.h"
#include "SModelViewCamera.h"
#include "SDXShape.h"
#include "SQuadTree.h"

class Sample : public SCore
{
public:
	//===================================================================
	// 각종 지원 객체
	//===================================================================
	shared_ptr<SCamera > m_pMainCamera;

	DXGame::SDxHelperEX		dxObj;
	bool					m_bDebugRender;
	D3DXMATRIX				m_matInitWorld;
	D3DXMATRIX				m_matWorld;

	SLine					m_Line;
	SQuadTree				m_QuadTree;
public:
	//====================================================================
	// QuadTree
	//====================================================================
	bool DrawQuadLine(SNode* pNode);

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
	HRESULT		CreateResource() { return S_OK; }
	HRESULT		DeleteResource() { return S_OK; }

	//======================================================================================
	// Load Export Data
	//======================================================================================
	HRESULT					CreateConstantBuffer();
	int WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
public:
	Sample();
	virtual ~Sample();
};
