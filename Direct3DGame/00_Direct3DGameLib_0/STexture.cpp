#include "STexture.h"

HRESULT STexture::Load(ID3D11Device* pDevice, const TCHAR* szFileName)
{
	HRESULT hr;
	
	ID3D11ShaderResourceView* CreateTextureSRV = nullptr;

	hr = D3DX11CreateShaderResourceViewFromFile(pDevice, szFileName, nullptr, nullptr, &CreateTextureSRV, nullptr);
	if (SUCCEEDED(hr))
	{
		m_pSRV.Attach(CreateTextureSRV);
		m_pSRV->GetDesc(&m_TextureDesc);
	}
	
	return hr;
}
bool STexture::Apply(ID3D11DeviceContext* pContext, UINT iSlot)
{
	pContext->PSSetShaderResources(iSlot, 1, m_pSRV.GetAddressOf());
	return true;
}

bool STexture::Init()
{
	return true;
}
bool STexture::Frame()
{
	return true;
}
bool STexture::Render()
{
	return true;
}
bool STexture::Release()
{
	return true;
}


STexture::STexture()
{
}


STexture::~STexture()
{
}
