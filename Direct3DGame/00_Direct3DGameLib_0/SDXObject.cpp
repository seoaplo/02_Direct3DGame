#include "SDXObject.h"

bool SDXObject::Init()
{
	return true;
}
bool SDXObject::Frame()
{
	return true;
}
bool SDXObject::Render(ID3D11DeviceContext*	pContext)
{
	return true;
}	
bool SDXObject::Release()
{
	return true;
}

void	SDXObject::Set(
	ID3D11Device*			pDevice,
	ID3D11DeviceContext*	pImmediateContext,
	SObjectInfo info)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pImmediateContext;

	m_Pos = info.InitPos;
	m_pTexture = I_TextureManager.GetPtr(info.iTextureID);
}
void SDXObject::Set(D3DXVECTOR3 pos)
{
	m_Pos = pos;
}
bool SDXObject::Load(ID3D11Device* pDevice, std::wstring filename)
{
	int iKey = I_TextureManager.Load(pDevice, filename);
	m_pTexture = I_TextureManager.GetPtr(iKey);
	if (m_pTexture == nullptr) return false;

	return true;
}
void     SDXObject::SetPos(D3DXVECTOR3 pos)
{
	m_Pos = pos;
}
void     SDXObject::SetPos(float x, float y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}
void	 SDXObject::Move(D3DXVECTOR3 pos)
{
	SetPos(m_Pos);
}

SDXObject::SDXObject()
{

}


SDXObject::~SDXObject()
{

}
