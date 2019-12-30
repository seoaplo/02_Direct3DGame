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
#include "SLightObj.h"

struct VS_CBChangesEveryFrame
{
	D3DXMATRIX      matNormal;
	D3DXVECTOR3		vLightPos;
	float			f1;
	D3DXVECTOR3		vEye;
	float			f2;
	D3DXVECTOR3		vEyeDir;
	float			f3;
};
struct VS_CBNeverChanges
{
	D3DXVECTOR4				cb_AmbientLightColor;
	D3DXVECTOR4				cb_DiffuseLightColor;
	D3DXVECTOR4				cb_SpecularLightColor;
};



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
	SLightObj*				m_pSunLight;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_pConstantBuffer;
	VS_CBNeverChanges		m_cbNeverChanges;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_pCBNeverChanges;
	//--------------------------------------------------------------------------------------
	// ����Ʈ ���� ���
	//--------------------------------------------------------------------------------------
	D3DXMATRIX				m_mLightWorld;
	D3DXVECTOR3				m_vInitLightPos;
	bool					m_bDebugRender;
	bool					m_bNormalMapRender;
	//===================================================================
	// ���� ���� ��ü
	//===================================================================
	shared_ptr<SCamera > m_pMainCamera;

	DXGame::SDxHelperEX		dxObj;
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
	SBrush					m_Brush;

	SNode*					m_PickNode;
	T_STR					m_strBaseTexture;
	T_STR					m_strAlphaTex0;
	T_STR					m_strAlphaTex1;
	T_STR					m_strAlphaTex2;
	T_STR					m_FileName;

	int iRow;
	int iColl;
	int iNumSize;
	float fDistance;

	float m_fBrushSize;
	float m_fHeightValue;
	bool  m_bHeightSet;
	DWORD m_dwHeightMode;

	bool m_bSplatingSet;
	float m_fSplatingTime;
	float m_fSplatingMaxTime;
	float m_fSmoothSize;

	bool bCreateMap;
	bool bMap;

	bool bLoadMap;
	bool bSaveMap;

	SParser					m_Paser;
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
	void	SetHeightVertex(SNode* pNode, DWORD dwMode, float fDistance, float fHeight);
	HRESULT ScreenViewPort(UINT iWidht, UINT iHeight);

	void	SetTexture();
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
