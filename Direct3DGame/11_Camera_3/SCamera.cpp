#include "SCamera.h"

bool	SCamera::Init()
{
	return true;
}
D3DXMATRIX SCamera::SetViewMatrix(D3DXVECTOR3 p, D3DXVECTOR3 t, D3DXVECTOR3 u)
{
	m_vPos = p;
	m_vTarget = t;
	m_vUp = u;

	D3DXMatrixLookAtLH(&m_matView, &m_vPos, &m_vTarget, &m_vUp);

	m_vLook		= D3DXVECTOR3(m_matView._13, m_matView._23, m_matView._33);
	m_vUp		= D3DXVECTOR3(m_matView._12, m_matView._22, m_matView._32);
	m_vRight	= D3DXVECTOR3(m_matView._11, m_matView._21, m_matView._31);

	return m_matView;
}
D3DXMATRIX SCamera::SetObjectView(D3DXVECTOR3 vMax, D3DXVECTOR3 vMin)
{
	D3DXVECTOR3 vTarget = (vMax + vMin) * 0.5f;
	float fRadius = D3DXVec3Length(&(vMax - vMin)) * 0.5f;

	D3DXVECTOR3 vPos = (m_vLook * (fRadius * 2)) * -1.0f;

	D3DXMatrixLookAtLH(&m_matView, &vPos, &vTarget, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	m_vPos = vPos;
	m_vTarget = vTarget;

	return m_matView;
}
D3DXMATRIX SCamera::SetProjMatrix(float fFov, float fAspect, float fNearPlane, float fFarPlane)
{
	m_fFov = fFov;
	m_fAspect = fAspect;
	m_fNearPlane = fNearPlane;
	m_fFarPlane = fFarPlane;

	D3DXMatrixPerspectiveFovLH(	&m_matProj, m_fFov, m_fAspect, m_fNearPlane, m_fFarPlane);
	
	return m_matProj;
}
void	SCamera::MoveLook(float fValue)
{
	m_vPos += m_vLook  * fValue;
}
void	SCamera::MoveSide(float fValue)
{
	m_vPos += m_vRight * fValue;
}
void	SCamera::MoveUp(float fValue)
{
	m_vPos += m_vUp * fValue;
}
bool	SCamera::Frame()
{
	SetViewMatrix(m_vPos, m_vTarget, m_vUp);
	return true;
}
D3DXMATRIX SCamera::UpdateVector()
{
	m_vLook.x = m_matView._13;
	m_vLook.y = m_matView._23;
	m_vLook.z = m_matView._33;

	m_vRight.x = m_matView._11;
	m_vRight.y = m_matView._21;
	m_vRight.z = m_matView._31;

	m_vUp.x = m_matView._12;
	m_vUp.y = m_matView._22;
	m_vUp.z = m_matView._32;

	D3DXVec3Normalize(&m_vLook, &m_vLook);
	// SFrustrum::SetMatrix(nullptr, &matView, &m_matProj);
	// SFrustrum::CreateFrustrum();
	return m_matView;
}
D3DXMATRIX SCamera::Update(D3DXVECTOR4 vDirValue)
{
	D3DXMATRIX matRotation;
	D3DXQuaternionRotationYawPitchRoll( &m_qRotation,
										vDirValue.x,
										vDirValue.y,
										vDirValue.z);
	m_vPos += m_vLook * vDirValue.w;
	m_fRadius += vDirValue.w;

	D3DXMatrixAffineTransformation(&matRotation, 1.0f, nullptr, &m_qRotation, &m_vPos);
	D3DXMatrixInverse(&m_matView, nullptr, &matRotation);

	return UpdateVector();
}

SCamera::SCamera()
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);

	m_vLook = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_vUp = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vRight = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}


SCamera::~SCamera()
{
}
