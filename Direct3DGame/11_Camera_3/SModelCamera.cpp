#include "SModelCamera.h"

D3DXMATRIX SModelCamera::Update(D3DXVECTOR4 vDirValue)
{
	D3DXMATRIX matRotation;
	D3DXMatrixRotationYawPitchRoll(&matRotation, vDirValue.y, vDirValue.x, vDirValue.z);
	// 기저벡터를 카메라 회전행렬로 변환한다.
	D3DXVECTOR3 vWorldUp, vWorldLook;
	D3DXVECTOR3 vLocalUp = D3DXVECTOR3(0, 1, 0);
	D3DXVECTOR3 vLocalLook = D3DXVECTOR3(0, 0, 1);
	D3DXVec3TransformCoord(&vWorldUp, &vLocalUp, &matRotation);
	D3DXVec3TransformCoord(&vWorldLook, &vLocalLook, &matRotation);
	// 카메라의 위치를 디렉션벡터 * 반지름 크기 만큼 이동시킨다.
	m_vPos = m_vTarget - (vWorldLook * vDirValue.w);
	return SetViewMatrix(m_vPos, m_vTarget, vWorldUp);
}

SModelCamera::SModelCamera()
{
}


SModelCamera::~SModelCamera()
{
}
