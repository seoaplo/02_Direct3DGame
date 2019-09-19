#pragma once

#include "S3DGameLib.h"
#include "SCamera.h"
#include "SModelViewCamera.h"
#include "SMiniMap.h"
#include "SHeightMap.h"
#include "SSampleObj.h"
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

class Sample : public SCore
{
public:
	//===================================================================
	// ���� ���� ��ü
	//===================================================================
	shared_ptr<SSampleObj>	m_pBoxObj;
	SLightObj*				m_pSunLight;
	shared_ptr<SModelViewCamera > m_pMainCamera;
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

	HRESULT					CreateConstantBuffer();
	int WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
public:
	Sample();
	virtual ~Sample();
};
