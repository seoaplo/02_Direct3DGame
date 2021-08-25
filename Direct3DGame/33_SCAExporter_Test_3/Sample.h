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
public:
	//===================================================================
	// ���� ���� ��ü
	//===================================================================
	shared_ptr<SCamera > m_pMainCamera;
	
	std::vector<SDrawObject*> m_ObjList;
	std::vector<SSkinObject*> m_SkinObjList;
	std::vector<SMatrixObjectList*> m_MatObjLists;

	SCAObject				m_SCAObject;

	DXGame::SDxHelperEX		dxObj;
	bool					m_bDebugRender;
	D3DXMATRIX				m_matInitWorld;
	D3DXMATRIX				m_matWorld;

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

	//======================================================================================
	// Load Export Data
	//======================================================================================
	HRESULT					CreateConstantBuffer();
	int WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	T_STR SaveFileDiallog(const TCHAR* szName, const TCHAR* szTitle);
	bool LoadSKMFile();
	bool LoadSMCFile();
	void ClearSCAObject();
public:
	Sample();
	virtual ~Sample();
};

