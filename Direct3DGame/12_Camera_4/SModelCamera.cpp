#include "SModelCamera.h"

D3DXMATRIX SModelCamera::Update(D3DXVECTOR4 vDirValue)
{
	D3DXMATRIX matRotation;
	D3DXMatrixRotationYawPitchRoll(&matRotation, vDirValue.y, vDirValue.x, vDirValue.z);
	// �������͸� ī�޶� ȸ����ķ� ��ȯ�Ѵ�.
	D3DXVECTOR3 vWorldUp, vWorldLook;
	D3DXVECTOR3 vLocalUp = D3DXVECTOR3(0, 1, 0);
	D3DXVECTOR3 vLocalLook = D3DXVECTOR3(0, 0, 1);
	D3DXVec3TransformCoord(&vWorldUp, &vLocalUp, &matRotation);
	D3DXVec3TransformCoord(&vWorldLook, &vLocalLook, &matRotation);
	// ī�޶��� ��ġ�� �𷺼Ǻ��� * ������ ũ�� ��ŭ �̵���Ų��.
	m_vPos = m_vTarget - (vWorldLook * vDirValue.w);
	return SetViewMatrix(m_vPos, m_vTarget, vWorldUp);
}

SModelCamera::SModelCamera()
{
}


SModelCamera::~SModelCamera()
{
}
