#include "xTexture.h"

HRESULT xTexture::Load(ID3D11Device* pDevice, const TCHAR* szFileName)
{
	HRESULT hr;
	
	hr = D3DX11CreateShaderResourceViewFromFile(pDevice, szFileName, nullptr, nullptr, &m_pSRV, nullptr);
	
	return hr;
}
bool xTexture::Apply(ID3D11DeviceContext* pContext, UINT iSlot)
{
	pContext->PSSetShaderResources(iSlot, 1, &m_pSRV);
	return true;
}

xTexture::xTexture()
{
	m_pSRV = 0;
}


xTexture::~xTexture()
{
	if (m_pSRV)m_pSRV->Release();
}
