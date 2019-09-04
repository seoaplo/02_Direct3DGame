#include "SCamera.h"

bool	SCamera::Init()
{
	return true;
}
void	SCamera::CreateViewMatrix(DXGame::SVector3 p, DXGame::SVector3 t, DXGame::SVector3 u)
{
	m_vPos = p;
	m_vTarget = t;
	m_vUp = u;

	m_matView.ViewLookAt(p, t, u);

	m_vLook		= DXGame::SVector3(m_matView._13, m_matView._23, m_matView._33);
	m_vUp		= DXGame::SVector3(m_matView._12, m_matView._22, m_matView._32);
	m_vRight	= DXGame::SVector3(m_matView._11, m_matView._21, m_matView._31);
}
void	SCamera::CreateProjMatrix(RECT ClientRect)
{
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(
		&matProj, D3DX_PI / 2,
		(float)ClientRect.right / (float)ClientRect.bottom,
		1.0f,
		100.0f);

	m_matProj.PerspectiveFovLH(1.0f,
		100.0f,
		D3DX_PI / 2,
		(float)ClientRect.right / (float)ClientRect.bottom);

}
void	SCamera::Forward()
{
	m_vPos = m_vPos + m_vLook * I_Timer.GetSPF() * 20.0f;
}
void	SCamera::Backward()
{
	m_vPos = m_vPos - m_vLook * I_Timer.GetSPF() * 20.0f;
}
void	SCamera::RightRotation()
{

	m_vPos = m_vPos + m_vRight * I_Timer.GetSPF() * 20.0f;

}
void	SCamera::LeftRotation()
{
	m_vPos = m_vPos - m_vRight * I_Timer.GetSPF() * 20.0f;
}
bool	SCamera::Frame()
{
	CreateViewMatrix(m_vPos, m_vTarget, m_vUp);
	return true;
}

SCamera::SCamera()
{
	m_matWorld.Identity();
	m_matView.Identity();
	m_matProj.Identity();
	m_vPos = DXGame::SVector3(0.0f, 0.0f, -10.0f);
	m_vTarget = DXGame::SVector3(0.0f, 0.0f, 0.0f);
	m_vUp = DXGame::SVector3(0.0f, 1.0f, 0.0f);
}


SCamera::~SCamera()
{
}
