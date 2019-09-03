#pragma once
#include "SModel.h"


struct PLANE_VERTEX_LIST
{
	union
	{
		struct
		{
			PNCT_VERTEX LeftTop;
			PNCT_VERTEX RightTop;
			PNCT_VERTEX RightBottom;
			PNCT_VERTEX LeftBottom;;
		};
		PNCT_VERTEX VERTEX[24];
	};
	PNCT_VERTEX& operator[] (unsigned iNum)
	{
		return VERTEX[iNum];
	}
	PLANE_VERTEX_LIST()
	{
		VERTEX[0].p = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
		VERTEX[0].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		VERTEX[0].c = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		VERTEX[0].t = D3DXVECTOR2(0.0f, 0.0f);

		VERTEX[1].p = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
		VERTEX[1].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		VERTEX[1].c = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		VERTEX[1].t = D3DXVECTOR2(1.0f, 0.0f);

		VERTEX[2].p = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
		VERTEX[2].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		VERTEX[2].c = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		VERTEX[2].t = D3DXVECTOR2(1.0f, 1.0f);

		VERTEX[3].p = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
		VERTEX[3].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		VERTEX[3].c = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		VERTEX[3].t = D3DXVECTOR2(0.0f, 1.0f);
	}
	~PLANE_VERTEX_LIST()
	{

	}
};


struct BOX_VERTEX_LIST
{
	union
	{
		struct
		{
			PLANE_VERTEX_LIST Front;
			PLANE_VERTEX_LIST Back;
			PLANE_VERTEX_LIST Low;
			PLANE_VERTEX_LIST Up;
			PLANE_VERTEX_LIST Left;
			PLANE_VERTEX_LIST Right;
		};
		PNCT_VERTEX Plane[24];
	};
	PNCT_VERTEX& operator[] (unsigned iNum)
	{
		return Plane[iNum];
	}
	BOX_VERTEX_LIST()
	{
		ZeroMemory(this, sizeof(BOX_VERTEX_LIST));
		// Á¤¸é
		Plane[0].p = D3DXVECTOR3(-1.0f, 1.0f, -1.0f);
		Plane[0].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[0].c = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		Plane[0].t = D3DXVECTOR2(0.0f, 0.0f);

		Plane[1].p = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
		Plane[1].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[1].c = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		Plane[1].t = D3DXVECTOR2(1.0f, 0.0f);

		Plane[2].p = D3DXVECTOR3(1.0f, -1.0f, -1.0f);
		Plane[2].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[2].c = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		Plane[2].t = D3DXVECTOR2(1.0f, 1.0f);

		Plane[3].p = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
		Plane[3].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[3].c = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		Plane[3].t = D3DXVECTOR2(0.0f, 1.0f);

		// µÞ¸é
		Plane[4].p = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		Plane[4].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[4].c = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		Plane[4].t = D3DXVECTOR2(0.0f, 0.0f);

		Plane[5].p = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
		Plane[5].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[5].c = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		Plane[5].t = D3DXVECTOR2(1.0f, 0.0f);

		Plane[6].p = D3DXVECTOR3(-1.0f, -1.0f, 1.0f);
		Plane[6].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[6].c = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		Plane[6].t = D3DXVECTOR2(1.0f, 1.0f);

		Plane[7].p = D3DXVECTOR3(1.0f, -1.0f, 1.0f);
		Plane[7].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[7].c = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		Plane[7].t = D3DXVECTOR2(0.0f, 1.0f);

		// ¾Æ·§¸é
		Plane[8].p = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
		Plane[8].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[8].c = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		Plane[8].t = D3DXVECTOR2(0.0f, 0.0f);

		Plane[9].p = D3DXVECTOR3(1.0f, -1.0f, -1.0f);
		Plane[9].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[9].c = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		Plane[9].t = D3DXVECTOR2(1.0f, 0.0f);

		Plane[10].p = D3DXVECTOR3(1.0f, -1.0f, 1.0f);
		Plane[10].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[10].c = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		Plane[10].t = D3DXVECTOR2(1.0f, 1.0f);

		Plane[11].p = D3DXVECTOR3(-1.0f, -1.0f, 1.0f);
		Plane[11].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[11].c = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		Plane[11].t = D3DXVECTOR2(0.0f, 1.0f);

		// À­¸é
		Plane[12].p = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
		Plane[12].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[12].c = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		Plane[12].t = D3DXVECTOR2(0.0f, 0.0f);

		Plane[13].p = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		Plane[13].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[13].c = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		Plane[13].t = D3DXVECTOR2(1.0f, 0.0f);

		Plane[14].p = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
		Plane[14].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[14].c = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		Plane[14].t = D3DXVECTOR2(1.0f, 1.0f);

		Plane[15].p = D3DXVECTOR3(-1.0f, 1.0f, -1.0f);
		Plane[15].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[15].c = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		Plane[15].t = D3DXVECTOR2(0.0f, 1.0f);

		// ¿Þ¸é
		Plane[16].p = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
		Plane[16].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[16].c = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		Plane[16].t = D3DXVECTOR2(0.0f, 0.0f);

		Plane[17].p = D3DXVECTOR3(-1.0f, 1.0f, -1.0f);
		Plane[17].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[17].c = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		Plane[17].t = D3DXVECTOR2(1.0f, 0.0f);

		Plane[18].p = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
		Plane[18].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[18].c = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		Plane[18].t = D3DXVECTOR2(1.0f, 1.0f);

		Plane[19].p = D3DXVECTOR3(-1.0f, -1.0f, 1.0f);
		Plane[19].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[19].c = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		Plane[19].t = D3DXVECTOR2(0.0f, 1.0f);


		// ¿À¸¥¸é
		Plane[20].p = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
		Plane[20].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[20].c = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		Plane[20].t = D3DXVECTOR2(0.0f, 0.0f);

		Plane[21].p = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		Plane[21].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[21].c = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		Plane[21].t = D3DXVECTOR2(1.0f, 0.0f);

		Plane[22].p = D3DXVECTOR3(1.0f, -1.0f, 1.0f);
		Plane[22].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[22].c = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		Plane[22].t = D3DXVECTOR2(1.0f, 1.0f);

		Plane[23].p = D3DXVECTOR3(1.0f, -1.0f, -1.0f);
		Plane[23].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		Plane[23].c = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		Plane[23].t = D3DXVECTOR2(0.0f, 1.0f);

	}
	~BOX_VERTEX_LIST()
	{

	}
};


class SDirection : public SModel
{
public:
	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vDir;
	D3DXVECTOR4 m_vColor;

	RECT		g_rtClient;
	vector<UINT>			m_iTextureList;
public:
	bool Init()												override;
	bool Render(ID3D11DeviceContext*	pContext)			override;
	bool Frame()											override;
	bool Release()											override;
public:
	bool CreateVertexBuffer(void* pData, int iNumCount, int iSize);
	bool LoadShaderAndInputlayout(const CHAR* pVertexShader, const CHAR* pPixelShader);
	bool CreateIndexBuffer(void* pData, int iNumCount, int iSize);
	bool CreateConstantBuffer(
		void* pData,
		int iNumVertex,
		int iSize,
		bool bDynamic = false);

	bool LoadSRV(T_STR name, int iIndex = 0);
	bool Load(ID3D11Device* pDevice);
public:
	void	Set(ID3D11Device*			pDevice,
		ID3D11DeviceContext*	pImmediateContext, D3DXVECTOR3 vPos, D3DXVECTOR3 vDir, RECT rcClient)
	{
		m_pDevice = pDevice;
		m_pContext = pImmediateContext;
		g_rtClient = rcClient;
		m_vPos = vPos;
		m_vDir = vDir;
	}
	void	SetVector(D3DXVECTOR3 vPos, D3DXVECTOR3 vDir, D3DXVECTOR4 vColor)
	{
		m_vPos = vPos;
		m_vDir = vDir;
		m_vColor = vColor;
	}
public:
	SDirection();
	~SDirection();
};

class SPlaneObject : public SModel
{
public:
	RECT		g_rtClient;
	vector<UINT>			m_iTextureList;
public:
	bool Init()												override;
	bool Render(ID3D11DeviceContext*	pContext)			override;
	bool Frame()											override;
	bool Release()											override;
public:
	bool CreateVertexBuffer(void* VERTEX, int iNumVertex, int iSize);
	bool LoadShaderAndInputlayout(const CHAR* pVertexShader, const CHAR* pPixelShader);
	bool CreateIndexBuffer(void* pData, int iNumVertex, int iSize);
	bool CreateConstantBuffer(
		void* pData,
		int iNumCount,
		int iSize,
		bool bDynamic = false);

	bool LoadSRV(T_STR name, int iIndex = 0);
	bool Load(ID3D11Device* pDevice, std::wstring filename, PLANE_VERTEX_LIST& VertexList);
public:
	void	Set(
		ID3D11Device*			pDevice,
		ID3D11DeviceContext*	pImmediateContext,
		D3DXVECTOR3 vPos, RECT rcClient)
	{
		m_pDevice = pDevice;
		m_pContext = pImmediateContext;

		g_rtClient = rcClient;
	}
public:
	SPlaneObject();
	~SPlaneObject();
};

class SBox : public SModel
{
public:
	RECT		g_rtClient;
	D3DXVECTOR3 vPos;
	vector<UINT>			m_iTextureList;
public:
	bool Init()												override;
	bool Render(ID3D11DeviceContext*	pContext)			override;
	bool Frame()											override;
	bool Release()											override;
public:
	bool CreateVertexBuffer(void* pData, int iNumCount, int iSize);
	bool LoadShaderAndInputlayout(const CHAR* pVertexShader, const CHAR* pPixelShader);
	bool CreateIndexBuffer(void* pData, int iNumCount, int iSize);
	bool CreateConstantBuffer(
		void* pData,
		int iNumVertex,
		int iSize,
		bool bDynamic = false);

	bool LoadSRV(T_STR name, int iIndex = 0);
	bool Load(ID3D11Device* pDevice, std::wstring filename, BOX_VERTEX_LIST& VertexList);
public:
	void	Set(ID3D11Device*			pDevice,
		ID3D11DeviceContext*	pImmediateContext, D3DXVECTOR3 vPos, RECT rcClient)
	{
		m_pDevice = pDevice;
		m_pContext = pImmediateContext;
		g_rtClient = rcClient;
	}
public:
	SBox();
	~SBox();
};