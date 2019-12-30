#include "SHeightMap.h"

float SHeightMap::GetHeightmap(int row, int col)
{
	return m_fHeightList[row * m_iNumRows + col] * m_MapDesc.fScaleHeight;
}
bool SHeightMap::CreateHeightMap(const TCHAR* strHeightMapTex)
{
	HRESULT hr = S_OK;
	// Image Info Set
	D3DX11_IMAGE_INFO ImageInfo;
	D3DX11_IMAGE_LOAD_INFO loadInfo;
	ZeroMemory(&loadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO));
	loadInfo.Usage = D3D11_USAGE_STAGING;
	loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	loadInfo.Format = DXGI_FORMAT_FROM_FILE;
	loadInfo.pSrcInfo = &ImageInfo;

	// Create texture form file
	ID3D11Resource *pTexture = NULL;
	hr = D3DX11CreateTextureFromFile(m_pDevice, strHeightMapTex, &loadInfo, NULL, &pTexture, NULL);
	HRESULT_FAILED_RETURN_BOOL(hr);

	// Texture Pointer Connect from Resource Pointer
	ID3D11Texture2D *pTexture2D = NULL;
	hr = pTexture->QueryInterface(__uuidof(ID3D11Texture2D), (LPVOID*)&pTexture2D);
	HRESULT_FAILED_RETURN_BOOL(hr);
	SAFE_RELEASE(pTexture);

	// Texture Get Descriptor
	D3D11_TEXTURE2D_DESC desc;
	pTexture2D->GetDesc(&desc);

	m_fHeightList.resize(desc.Height*desc.Width);

	// Get Texture Data From Gpu Memory
	if (pTexture2D)
	{
		D3D11_MAPPED_SUBRESOURCE MappedFaceDest;
		if (SUCCEEDED(m_pContext->Map((ID3D11Resource*)pTexture2D, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_READ, 0, &MappedFaceDest)))
		{
			// 텍스쳐 데이터에 픽셀 데이터 외의 데이터가 존재할 수 있기 때문에 
			// 직접 제어하는 대신 구조체를 통해 제어하는 방식이 안전하다.
			UCHAR* pTexels = (UCHAR*)MappedFaceDest.pData;
			PNCT_VERTEX	v;
			for (UINT row = 0; row < desc.Height; row++)
			{
				UINT rowStart = row * MappedFaceDest.RowPitch;
				for (UINT col = 0; col < desc.Width; col++)
				{
					UINT colStart = col * 4;	// RGBA 시작 주소
					UINT uRed = pTexels[rowStart + colStart + 0];
					m_fHeightList[row * desc.Width + col] = uRed;	// DWORD이므로 pitch/4	
				}
			}
			m_pContext->Unmap(pTexture2D, D3D11CalcSubresource(0, 0, 1));
		}
	}

	m_iNumRows = desc.Height;
	m_iNumCols = desc.Width;

	SAFE_RELEASE(pTexture2D);
	return true;
}
float SHeightMap::GetHeightOfVertex(UINT Index)
{
	return m_fHeightList[Index] * m_MapDesc.fScaleHeight;
};
D3DXVECTOR4 SHeightMap::GetColorOfVertex(UINT Index)
{
	return D3DXVECTOR4(1, 1, 1, 1.0f);
};
SHeightMap::SHeightMap()
{
}


SHeightMap::~SHeightMap()
{
}
