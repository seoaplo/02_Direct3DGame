#include "SBrush.h"


HRESULT	SATPoint::CreateConstantBuffer()
{
	m_dxobj.g_pConstantBuffer.Attach(DXGame::CreateConstantBuffer(m_pDevice, &m_ConstantData, 1, sizeof(AT_ConstantData)));

	return S_OK;
}
void	SATPoint::UpdateConstantBuffer(ID3D11DeviceContext* pContext, SModel* pParent)
{
	pContext->UpdateSubresource(m_dxobj.g_pConstantBuffer.Get(), 0, NULL, &m_ConstantData, 0, 0);
	pContext->GSSetConstantBuffers(0, 1, m_dxobj.g_pConstantBuffer.GetAddressOf());
	pContext->PSSetConstantBuffers(0, 1, m_dxobj.g_pConstantBuffer.GetAddressOf());
}
SATPoint::SATPoint()
{
	D3DXMatrixIdentity(&m_ConstantData.matProj);
	m_ConstantData.fDistance = 0.0f;
}
SATPoint::~SATPoint()
{

}


bool SBrush::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	return true;
}
bool SBrush::Set(const STexture* pTargetTexture, float fWidth, float fHeight)
{
	assert(pTargetTexture);
	if (pTargetTexture == nullptr) return false;
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTextureInterface;
	Microsoft::WRL::ComPtr<ID3D11Resource> res;
	Microsoft::WRL::ComPtr<ID3D11Resource> res2;
	pTargetTexture->m_pSRV->GetResource(res.GetAddressOf());
	res->QueryInterface<ID3D11Texture2D>(pTextureInterface.GetAddressOf());
	D3D11_TEXTURE2D_DESC desc;
	pTextureInterface->GetDesc(&desc);

	D3DXMatrixOrthoLH(&m_Point.m_ConstantData.matProj, fWidth, fHeight, 0.0f, 1.0f);
	D3DXMatrixTranspose(&m_Point.m_ConstantData.matProj, &m_Point.m_ConstantData.matProj);

	m_Point.m_ConstantData.dwWidth = static_cast<DWORD>(fWidth);
	m_Point.m_ConstantData.dwHeight = static_cast<DWORD>(fHeight);

	m_Point.Create(m_pDevice, m_pContext, pTargetTexture->m_szName.c_str(), L"AlphaTexture.hlsl");

	m_RenderTarget.Create(m_pDevice, static_cast<float>(desc.Width), static_cast<float>(desc.Height));
	m_RenderTarget.Set(m_pDevice, 0, 0, static_cast<float>(desc.Width), static_cast<float>(desc.Height));
	m_RenderTarget.Clear(m_pContext, D3DXVECTOR4(0.0, 0.0, 0.0, 1.0));


	m_RenderTarget.m_pSRV->GetResource(m_FirstRenderTexture.GetAddressOf());

	hr = m_pDevice->CreateTexture2D(&m_RenderTarget.m_TexDesc, 0, m_SecondRenderTexture.GetAddressOf());
	HRESULT_FAILED_RETURN(hr);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	m_RenderTarget.m_pSRV->GetDesc(&SRVDesc);

	m_pDevice->CreateShaderResourceView(m_SecondRenderTexture.Get(), &SRVDesc, m_SecondRenderTarget.GetAddressOf());
	return true;
}

bool SBrush::Draw(ID3D11DeviceContext* pContext, PC_VERTEX TargetVertex, float fDistance, float fSmoothSize, DWORD RSNumber)
{
	if (m_RenderTarget.Begin(pContext, D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f)))
	{
		m_pContext->CopyResource(m_SecondRenderTexture.Get(), m_FirstRenderTexture.Get());
		m_Point.m_ConstantData.fDistance = fDistance;
		m_Point.m_ConstantData.fSmoothSize = fDistance * fSmoothSize;
		m_Point._dxobj.g_pTextureSRV = m_SecondRenderTarget.Get();
		m_Point.Draw(pContext, TargetVertex.p, TargetVertex.c);
		return m_RenderTarget.End(pContext);
	}
	return false;
}
bool SBrush::Release()
{
	return true;
}


SBrush::SBrush()
{
	Release();
}


SBrush::~SBrush()
{
	Release();
}
