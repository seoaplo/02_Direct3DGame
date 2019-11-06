#pragma once

#include "S3DGameLib.h"
#include "SCamera.h"
#include "SModelViewCamera.h"
#include "SDXShape.h"
#include "SMesh.h"
#include "SSSFileLoader.h"
#include "SkinFileLoader.h"
#include "SMatrixFileLoader.h"
#include "SCAObject.h"

class Sample : public SCore
{
	friend class Singleton<SSSFileLoader>;
	friend class SSSFileLoader;
public:
	//===================================================================
	// ���� ���� ��ü
	//===================================================================
	shared_ptr<SCamera > m_pMainCamera;

	std::vector<SDrawObject*> m_ObjList;

	DXGame::SDxHelperEX		dxObj;
	bool					m_bDebugRender;
	D3DXMATRIX				m_matInitWorld;
	D3DXMATRIX				m_matWorld;

	vector<vector<PNCT_VERTEX>> VertexList;

	TCHAR FilePath[MAX_PATH];
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

	TCHAR* SaveFileDiallog(const TCHAR* szName, const TCHAR* szTitle);
	bool LoadFile();
	//======================================================================================
	// Load Export Data
	//======================================================================================
	HRESULT					CreateConstantBuffer();
	int WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
public:
	Sample();
	virtual ~Sample();
};