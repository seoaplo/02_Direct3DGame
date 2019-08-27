#pragma once
#include "SUtils.h"
#include "SDxBasic.h"

class STexture
{
public:
	T_STR	m_szPath;
	T_STR	m_szName;
	int		m_iIndex;
	float	fScale;
public:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pSRV;
	D3D11_SHADER_RESOURCE_VIEW_DESC						m_TextureDesc;
public:
	HRESULT Load(ID3D11Device* pDevice, const TCHAR* szFileName);
	bool	Apply(ID3D11DeviceContext* pContext, UINT iSlot = 0);

public:
	bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();
public:
	STexture();
	~STexture();
};

