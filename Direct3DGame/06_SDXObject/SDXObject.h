#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "SUtils.h"
#include "SDxHelperEX.h"
#include "xTexture.h"
#include "STimer.h"

//====================================================================
// Structures
//====================================================================
struct PCT_Vertex
{
	float x, y, z;
	float u, v;
};

struct VSCB
{
	float color[4];
	float etc[4];
};

class SDXObject
{
public:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;
	ID3D11BlendState*		m_pAlphaBlend;

	DXGame::SDxHelperEX m_dxObj;
	std::vector<UINT>		m_TextureList;

public:
	bool	Init();
	bool	Frame();
	bool	Render(ID3D11DeviceContext*	pContext, UINT  iSize, UINT iCount);
	bool	Release();
public:
	void SetDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	{
		m_pDevice = pDevice;
		m_pDeviceContext = pDeviceContext;
	}
	bool SetBlendState();
	//==============================================================================================
	// Create Buffer (Vertex, index, Constant)
	//==============================================================================================
	bool CreateVertexBuffer(void* vertices, int iNumCount, int iSize);
	bool CreateIndexBuffer(void* pData, int iNumCount, int iSize);
	bool CreateConstantBuffer(void* pData, int iNumCount, int iSize, bool bDynamic = false);
	//==============================================================================================
	// Load  (Shader, Input layout, Shader Resource View)
	//==============================================================================================
	bool LoadShaderAndInputlayout(const char* pVertexShader, const char* pPixelShader);
	bool LoadSRV(T_STR name, int iIndex = 0);
public:
	SDXObject();
	~SDXObject();
};

