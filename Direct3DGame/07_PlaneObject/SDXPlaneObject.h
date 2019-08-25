#pragma once
#include "SDXObject.h"
#include "SVector.h"
#include "SMatrix.h"

struct PNCT_VERTEX
{
	DXGame::SVector3 p;
	DXGame::SVector3 n;
	DXGame::SVector4 c;
	DXGame::SVector2 t;
};
struct VS_CB
{
	DXGame::SMatrix matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	float color[4];
	float etc[4];
};

class SDXPlaneObject : public SDXObject
{
public:
	D3DXMATRIX m_matWorld;
	D3DXMATRIX m_matView;
	D3DXMATRIX m_matProj;
	RECT		g_rtClient;
	vector<UINT>			m_iTextureList;
public:
	bool Init()												override;
	bool Load(ID3D11Device* pDevice, std::wstring filename)	override;
	bool Render(ID3D11DeviceContext*	pContext)			override;
	bool Frame()											override;
	bool Release()											override;
public:
	bool CreateVertexBuffer(void* vertices, int iNumVertex, int iSize);
	bool LoadShaderAndInputlayout(const CHAR* pVertexShader, const CHAR* pPixelShader);
	bool CreateIndexBuffer(void* pData, int iNumVertex, int iSize);
	bool CreateConstantBuffer(
		void* pData,
		int iNumVertex,
		int iSize,
		bool bDynamic = false);

	bool LoadSRV(T_STR name, int iIndex = 0);
public:
	void	Set(
		ID3D11Device*			pDevice,
		ID3D11DeviceContext*	pImmediateContext,
		D3DXVECTOR3 vPos, RECT rcClient)
	{
		m_pDevice = pDevice;
		m_pDeviceContext = pImmediateContext;

		m_Pos = vPos;
		g_rtClient = rcClient;
	}
public:
	SDXPlaneObject();
	~SDXPlaneObject();
};
