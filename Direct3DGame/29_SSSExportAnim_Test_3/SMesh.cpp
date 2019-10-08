#include "SMesh.h"

//----------------------------------------------------------------------------------------------------------
// Create
//----------------------------------------------------------------------------------------------------------
bool Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TextureList* pLoadTextureList = nullptr, const TCHAR* pLoadShaderFile = nullptr)
{
	return true;
}
bool CreateVertexData()
{
	return true;
}
bool CreateIndexData()
{
	return true;
}
HRESULT CreateVertexBuffer()
{
	HRESULT hr;
	return hr;
}
HRESULT CreateIndexBuffer()
{
	HRESULT hr;
	return hr;
}
HRESULT CreateResource()
{
	HRESULT hr;
	return hr;
}
//----------------------------------------------------------------------------------------------------------
// Load
//----------------------------------------------------------------------------------------------------------

bool Load(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* szLoadName, const TCHAR* pLoadShaderFile, bool bThread = false)
{
	return true;
}
HRESULT LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pLoadShaderFile = nullptr)
{
	HRESULT hr;
	return hr;
}
HRESULT LoadTextures(ID3D11Device* pDevice)
{
	HRESULT hr;
	return hr;
}

//----------------------------------------------------------------------------------------------------------
// Frame Work
//----------------------------------------------------------------------------------------------------------
bool Init()
{
	return true;
}
bool Frame()
{
	return true;
}
bool PreRender(ID3D11DeviceContext* pContext)
{
	return true;
}
bool Render(ID3D11DeviceContext*		pContext)
{
	return true;

}
bool PostRender(ID3D11DeviceContext* pContext)
{
	return true;

}
bool Release()
{
	return true;
}