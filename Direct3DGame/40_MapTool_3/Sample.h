#pragma once
#include "S3DGameLib.h"
#include "SCamera.h"
#include "SModelViewCamera.h"
#include "SDXShape.h"
#include "SQuadTree.h"
#include "SBaseObj.h"
#include "SMiniMap.h"
#include "SQuadTreeArray.h"
#include "SHeightMap.h"
#include "SSelect.h"
#include "SAlphaTexturing.h"


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
	BOX_VERTEX_LIST			m_BoxVertex;
	SQuadTreeArray			m_QuadTree;
	SMiniMap				m_MiniMap;
	SMap					m_Map;
	SHeightMap				m_HeightMap;
	SSelect					m_Select;
	S_BoxObject*			m_pObj;

	PC_VERTEX				m_AlphaVertex;
	SAlphaTexturing			m_AlphaTexture;

	std::vector<SNode*>		m_PickingNodeList;
	SNode*					m_PickNode;
	D3DXVECTOR3				m_vPickVector;
	T_STR					m_strBaseTexture;
	T_STR					m_strAlphaTex0;
	T_STR					m_strAlphaTex1;
	T_STR					m_strAlphaTex2;
	int iRow;
	int iColl;
	int iNumSize;
	float fDistance;

	float m_fBrushSize;
	float m_fHeightValue;
	bool  m_bHeightSet;

	bool m_bSplatingSet;
	float m_fSplatingTime;
	float m_fSplatingMaxTime;
	float m_fSmoothSize;

	bool bCreateMap;
	bool bMap;

	bool bLoadMap;
	bool bSaveMap;
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
	bool	FindePickingNode(SNode* pNode);
	bool	FindePickingFace(SNode* pNode);
	SNode*	CheckPicking();
	HRESULT ScreenViewPort(UINT iWidht, UINT iHeight);

	void	SetTexture();
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
	// Load Data
	//======================================================================================
	bool		LoadFile();
	bool		SaveFile();
	TCHAR*		SaveFileDiallog(const TCHAR* szName, const TCHAR* szTitle);

	HRESULT					CreateConstantBuffer();
	int WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
public:
	Sample();
	virtual ~Sample();
};
