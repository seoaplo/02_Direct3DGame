#pragma once
#include "SDxRT.h"
#include "SDXShape.h"

struct AT_ConstantData
{
	D3DXMATRIX matProj;
	float fDistance;
	float fOutline;
	DWORD dwWidth;
	DWORD dwHeight;

};

class SATPoint : public SPoint
{
public:
	AT_ConstantData		m_ConstantData;
	HRESULT				CreateConstantBuffer() override;
	void				UpdateConstantBuffer(ID3D11DeviceContext* pContext, SModel* pParent = nullptr)override;
public:
	SATPoint();
	virtual ~SATPoint();
};

class SAlphaTexturing
{
private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
public:
	DXGame::SDxRT			m_RenderTarget;
	SATPoint					m_Point;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_ConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Resource> m_FirstRenderTexture;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_SecondRenderTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SecondRenderTarget;

	STexture*				m_RTexture;
	STexture*				m_GTexture;
	STexture*				m_BTexture;
	STexture*				m_ATexture;
public:
	bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	bool Set(const STexture* pTargetTexture, float fWidth, float fHeight);
	bool Draw(ID3D11DeviceContext* pContext, PC_VERTEX TargetVertex, float fDistance, float fOutline);
	bool Release();
public:
	SAlphaTexturing();
	virtual ~SAlphaTexturing();
};

