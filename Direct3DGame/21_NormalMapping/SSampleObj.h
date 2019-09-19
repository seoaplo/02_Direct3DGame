#pragma once
#include "SNormalMap.h"

class SSampleObj : public SDXShape
{
public:
	vector<PNCT2_VERTEX>	m_VertexList;
	SNormalMap			m_NormalMap;
	T_STR				m_strNormalMapName;
	INT					m_itxNormalMap;
	const STexture*			m_pNormalTexture;
	D3DXMATRIX			m_matNormal;
public:
	bool				Render(ID3D11DeviceContext*  pContext);
	bool				UpdateBuffer();
	HRESULT				SetInputLayout();
	HRESULT				CreateVertexBuffer();
	HRESULT				CreateIndexBuffer();
	HRESULT				CreateResource();
	HRESULT				LoadTextures(ID3D11Device* pDevice, const TCHAR* pLoadTextureString);
	void				SetMatrix(const D3DXMATRIX* pWorld, const D3DXMATRIX* pView, const D3DXMATRIX* pProj);
public:
	SSampleObj();
	~SSampleObj();
};

