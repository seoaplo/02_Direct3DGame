#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "S3DGameLib.h"

//====================================================================
// Structures
//====================================================================
struct PCT_Vertex
{
	D3DXVECTOR4 Position;
	D3DXVECTOR4 Color;
	D3DXVECTOR2 TexCoord;
};

struct VSCB
{
	D3DXMATRIX mat;

	float fTime;
	float fC5_Y;
	float fC5_Z;
	float fC5_W;
};

class SDXObject
{
public:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	PCT_Vertex m_VertexList[4];

public:
	//====================================================================
	// 버텍스 버퍼 및 레이아웃
	//====================================================================
	ID3D11InputLayout*	m_pVertexLayout;
	ID3D11Buffer*		m_pVertexBuffer;
	ID3D11Buffer*		m_pIndexBurffer;
	ID3D11Buffer*		m_pConstantBuffer;
	//====================================================================
	// 버텍스 및 픽셀 쉐이더
	//====================================================================
	ID3D11VertexShader*		m_pVertexShader;
	ID3D11PixelShader*		m_pPixelShader;
	//====================================================================
	// 텍스쳐
	//====================================================================
	ID3D11ShaderResourceView*	m_pTextureSRV;
	//====================================================================
	// Rasterizer Set
	//====================================================================
	D3D_PRIMITIVE_TOPOLOGY	m_PrimType;
	D3D11_FILL_MODE			m_FillMode;
	//====================================================================
	// ALPHA Blend
	//====================================================================
	ID3D11BlendState* m_pAlphaBlend;
	D3D11_BLEND_DESC	m_BlendDesc;
public:
	//====================================================================
	// 정점 및 픽셀 쉐이더 로딩 및 생성
	//====================================================================
	HRESULT LoadShaderAndInputLayout(	const T_STR& VSRoute, const CHAR_STRING& VSFunc,
										const T_STR& PSRoute, const CHAR_STRING& PSFunc);

	//====================================================================
	// 버퍼 생성
	//====================================================================
	HRESULT CreateVertexBuffer(vector<PCT_Vertex> SourceVertexList);
	HRESULT CreateIndexBuffer();
	HRESULT CreateConstantBuffer();
	//====================================================================
	// 텍스쳐 생성
	//====================================================================
	HRESULT CreateTexture(const TCHAR* wRouteString);
	//====================================================================
	// 알파블렌딩 생성
	//====================================================================
	HRESULT AlphaBlendSet();
	void SetDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	{
		m_pDevice = pDevice;
		m_pDeviceContext = pDeviceContext;
	}

	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

public:
	SDXObject();
	~SDXObject();
};

