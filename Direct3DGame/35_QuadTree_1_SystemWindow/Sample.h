#pragma once
#include "S3DGameLib.h"
#include "SCamera.h"
#include "SModelViewCamera.h"
#include "SDXShape.h"
#include "SQuadTree.h"
#include "SBaseObj.h"
#include "SMiniMap.h"


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
	// ���� ���� ��ü
	//===================================================================
	shared_ptr<SCamera > m_pMainCamera;

	DXGame::SDxHelperEX		dxObj;
	bool					m_bDebugRender;
	D3DXMATRIX				m_matInitWorld;
	D3DXMATRIX				m_matWorld;

	int						m_iDrawDepth;
	SLine					m_Line;
	SBox					m_Box;
	SQuadTree				m_QuadTree;
	SMiniMap				m_MiniMap;

	S_BoxObject*			m_pObj;
public:
	//====================================================================
	// QuadTree
	//====================================================================
	bool DrawQuadLine(SNode* pNode);
	void DrawObject();
	void DrawSelectTreeLevel(D3DXMATRIX* pView, D3DXMATRIX* pProj);

	void	DrawMiniMap();
	void	DrawMiniMapObject(SNode* pNode);
	HRESULT ScreenViewPort(UINT iWidht, UINT iHeight);
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
