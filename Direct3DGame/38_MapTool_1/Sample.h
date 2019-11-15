#pragma once
#include "S3DGameLib.h"
#include "SCamera.h"
#include "SModelViewCamera.h"
#include "SDXShape.h"
#include "SQuadTree.h"
#include "SBaseObj.h"
#include "SMiniMap.h"
#include "SQuadTreeMap.h"
#include "SHeightMap.h"
#include "SSelect.h"


#define NUM_OBJECTS 100

class S_BoxObject : public SBaseObj
{
public:
	D3DXMATRIX	m_matWorld;
	D3DXVECTOR3	m_vPosition;
	D3DXVECTOR4	m_vColor;
public:
	S_BoxObject();
	~S_BoxObject()
	{

	}
};


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

	int						m_iDrawDepth;
	SLine					m_Line;
	SBox					m_Box;
	SQuadTreeMap			m_QuadTree;
	SMiniMap				m_MiniMap;
	SMap					m_Map;
	SHeightMap				m_HeightMap;
	SSelect					m_Select;
	S_BoxObject*			m_pObj;

	int iRow;
	int iColl;
	int iNumSize;
	float fDistance;
	bool bCreateMap;
	bool bMap;
public:
	void   SetTool(HWND hWnd, HINSTANCE hInst);
	//====================================================================
	// QuadTree
	//====================================================================
	bool DrawQuadLine(SNode* pNode);
	void DrawObject();
	void DrawSelectTreeLevel(D3DXMATRIX* pView, D3DXMATRIX* pProj);

	void	DrawMiniMap();
	void	DrawMiniMapObject(SNode* pNode);
	void	DrawMiniMapNode(SNode* pNode);
	void	DrawPickingTile(SNode* pNode);
	void	SetHeightVertex(SNode* pNode, float fDistance, float fHeight);
	SNode*	FindePickingNode(SNode* pNode);
	HRESULT ScreenViewPort(UINT iWidht, UINT iHeight);
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
