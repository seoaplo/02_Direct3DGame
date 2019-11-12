#pragma once
#include "SDxHelperEX.h"
#include "SCamera.h"

class SMiniMap : public SCamera
{
private:
	shared_ptr<SDXShape>		m_pPlaneShape;
	shared_ptr<DXGame::SDxRT>	m_pMiniMapRT;
	D3D11_VIEWPORT				m_vp;
public:
	void	Set(ID3D11Device* pDevice, float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth = 0, float MaxDepth = 1); 
	bool	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const TCHAR* pLoadTexture = nullptr, const TCHAR* pLoadShaderFile = nullptr);
	bool	Frame();
	bool	Render(ID3D11DeviceContext* pContext);
	bool	Release();
	bool	BeginRender(ID3D11DeviceContext* pContext, D3DXVECTOR4 vColor = D3DXVECTOR4(1, 1, 1, 1));
	bool	EndRender(ID3D11DeviceContext* pContext);
public:
	HRESULT		CreateResource(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, UINT iWidth, UINT iHeight);
	HRESULT		DeleteResource();
	SMiniMap();
	~SMiniMap();
};

