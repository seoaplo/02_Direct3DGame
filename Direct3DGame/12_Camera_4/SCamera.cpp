#include "SCamera.h"

bool	SCamera::Init()
{
	return true;
}
D3DXMATRIX SCamera::SetViewMatrix(D3DXVECTOR3 p, D3DXVECTOR3 t, D3DXVECTOR3 u)
{
	m_vPos = p;
	m_vTarget = t;
	D3DXMatrixLookAtLH(&m_matView, &m_vPos, &m_vTarget, &u);

	D3DXQUATERNION qRotation;
	D3DXVECTOR3 vTrans, vScale, vAxis;
	D3DXMATRIX mInvView;
	D3DXQUATERNION q;
	float fAngle;

	if (SUCCEEDED(D3DXMatrixDecompose(&vScale, &qRotation, &vTrans, &m_matView)))
	{
		D3DXQuaternionNormalize(&qRotation, &qRotation);
		D3DXQuaternionToAxisAngle(&qRotation, &vAxis, &fAngle);
		// 반드시 정규화 해야 한다.
		D3DXVec3Normalize(&vAxis, &vAxis);

		q.x = sin(fAngle / 2) * vAxis.x;
		q.y = sin(fAngle / 2) * vAxis.y;
		q.z = sin(fAngle / 2) * vAxis.z;
		q.w = cos(fAngle / 2);
	}
	D3DXMatrixInverse(&mInvView, NULL, &m_matView);
	// The axis basis vectors and camera position are stored inside the 
	// position matrix in the 4 rows of the camera's world matrix.
	// To figure out the yaw/pitch of the camera, we just need the Z basis vector
	D3DXVECTOR3* pZBasis = (D3DXVECTOR3*)&mInvView._31;

	m_fYawAngle = atan2f(pZBasis->x, pZBasis->z);
	float fLen = sqrtf(pZBasis->z * pZBasis->z + pZBasis->x * pZBasis->x);
	m_fPitchAngle = -atan2f(pZBasis->y, fLen);

	//m_vPos = p;
	//m_vTarget = t;
	//m_vUp = u;

	//D3DXMatrixLookAtLH(&m_matView, &m_vPos, &m_vTarget, &m_vUp);

	//m_vLook		= D3DXVECTOR3(m_matView._13, m_matView._23, m_matView._33);
	//m_vUp		= D3DXVECTOR3(m_matView._12, m_matView._22, m_matView._32);
	//m_vRight	= D3DXVECTOR3(m_matView._11, m_matView._21, m_matView._31);

	return UpdateVector();
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
										vDirValue.y,
										vDirValue.x,
										vDirValue.z);
	m_vPos += m_vLook * vDirValue.w;
	m_fRadius += vDirValue.w;

	D3DXMatrixAffineTransformation(&matRotation, 1.0f, nullptr, &m_qRotation, &m_vPos);
	D3DXMatrixInverse(&m_matView, nullptr, &matRotation);

	return UpdateVector();
}

SCamera::SCamera()
{

	m_fDefaultRadius = 0.0f;
	m_fMinRadius = 1.0f;
	m_fMaxRadius = FLT_MAX;
	m_fRadius = 0.0f;

	m_fYawAngle = 0.0f;
	m_fPitchAngle = 0.0f;
	m_fRollAngle = 0.0f;


	D3DXMatrixIdentity(&m_matWorld);


	D3DXMatrixIdentity(&m_matWorld);
	SetProjMatrix(D3DX_PI / 4.0f, 1.0f, 0.1f, 1000.0f);
	SetViewMatrix(D3DXVECTOR3(0.0f, 0.0f, 10.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));
}


SCamera::~SCamera()
{
}
