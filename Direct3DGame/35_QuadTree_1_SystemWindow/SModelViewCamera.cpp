#include "SModelViewCamera.h"

//======================================================================================
// 카메라 위치 정보
//======================================================================================
void SModelViewCamera::MoveLook(float fValue)
{
	m_vTarget += m_vLook * fValue;
}
void SModelViewCamera::MoveSide(float fValue)
{
	m_vTarget += m_vRight * fValue;
}
void SModelViewCamera::MoveUp(float fValue)
{
	m_vTarget += m_vUp * fValue;
}

//======================================================================================
// 오일러 각에 따른 뷰 행렬 계산 
//======================================================================================
D3DXMATRIX SModelViewCamera::Update(float fElapseTime)
{
	if (I_InputManager.KeyBoardState(DIK_W)) MoveLook(	fElapseTime * 2.0f	);
	if (I_InputManager.KeyBoardState(DIK_S)) MoveLook( -fElapseTime * 2.0f	);
	if (I_InputManager.KeyBoardState(DIK_D)) MoveSide(	fElapseTime * 2.0f	);
	if (I_InputManager.KeyBoardState(DIK_A)) MoveSide( -fElapseTime * 2.0f	);
	if (I_InputManager.KeyBoardState(DIK_Q)) MoveUp(	fElapseTime * 2.0f	);
	if (I_InputManager.KeyBoardState(DIK_E)) MoveUp(   -fElapseTime * 2.0f	);

	// Change the radius from the camera to the model based on wheel scrolling
	if (m_nMouseWheelDelta && m_nZoomButtonMask == MOUSE_WHEEL) 
		m_fRadius += m_nMouseWheelDelta * m_fRadius * 1.0f / 120.0f;
	m_fRadius = __min(m_fMaxRadius, m_fRadius);
	m_fRadius = __max(m_fMinRadius, m_fRadius);
	m_nMouseWheelDelta = 0;

	// Get the inverse of the arcball's rotation matrix
	D3DXMATRIX mCameraRot;
	D3DXMatrixInverse(&mCameraRot, nullptr, m_ViewArcBall.GetRotationMatrix());

	// Transform vectors based on camera's rotation matrix
	D3DXVECTOR3 vWorldUp, vWorldLook;
	D3DXVECTOR3 vLocalUp(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 vLocalLook(0.0f, 0.0f, 1.0f);
	D3DXVec3TransformCoord(&vWorldUp, &vLocalUp, &mCameraRot);
	D3DXVec3TransformCoord(&vWorldLook, &vLocalLook, &mCameraRot);

	// Update the eye point based on a radius away from the lookAt position
	m_vPos = m_vTarget - vWorldLook * m_fRadius;

	// Update the view matrix
	D3DXMatrixLookAtLH(&m_matView, &m_vPos, &m_vTarget, &vWorldUp);

	D3DXMATRIX matInvView;
	D3DXMatrixInverse(&matInvView, nullptr, &m_matView);
	matInvView._41 = matInvView._42 = matInvView._43 = 0;

	D3DXMATRIX matModelLastRotInv;
	D3DXMatrixInverse(&matModelLastRotInv, nullptr, &m_matModelLastRot);

	// Accumulate the delta of the arcball's rotation in view space.
	// Note that per-frame delta rotations could be problematic over long periods of time.
	D3DXMATRIX matModelRot;
	matModelRot = *m_WorldArcBall.GetRotationMatrix();
	m_matModelRot *= m_matView * matModelLastRotInv * matModelRot * matInvView;

	m_matCameraRotLast = mCameraRot;
	m_matModelLastRot = matModelRot;

	// Translate the rotation matrix to the same position as the lookAt position
	m_matModelRot._41 = m_vTarget.x;
	m_matModelRot._42 = m_vTarget.y;
	m_matModelRot._43 = m_vTarget.z;

	// Translate world matrix so its at the center of the model
	D3DXMATRIX masTRans;
	D3DXMatrixTranslation(&masTRans, -m_vModelCenter.x, -m_vModelCenter.y, -m_vModelCenter.z);
	m_matWorld = masTRans * m_matModelRot;

	return UpdateVector();
}

void SModelViewCamera::InputProc()
{
	return;
}

//======================================================================================
// 뷰 행렬 계산 
//======================================================================================
D3DXMATRIX	SModelViewCamera::SetViewMatrix(D3DXVECTOR3 vPos, D3DXVECTOR3 vTarget, D3DXVECTOR3 vUp)
{
	m_vDefaultEye = m_vPos = vPos;
	m_vDefaultLookAt = m_vTarget = vTarget;

	// Propogate changes to the member arcball
	D3DXQUATERNION quat;

	D3DXMatrixLookAtLH(&m_matView, &vPos, &vTarget, &vUp);
	D3DXQuaternionRotationMatrix(&quat, &m_matView);
	m_ViewArcBall.SetQuatNow(quat);

	// Set the radius according to the distance
	D3DXVECTOR3 vEyeToPoint = vTarget - vPos;
	//D3DXVec3Subtract(&vEyeToPoint, &vTarget, &vPos);
	SetRadius(D3DXVec3Length(&vEyeToPoint));

	return UpdateVector();
}
SModelViewCamera::SModelViewCamera()
{
	m_fRadius = 5.0f;
	m_fDefaultRadius = 5.0f;
	m_fMinRadius = 1.0f;
	m_fMaxRadius = FLT_MAX;

	m_vModelCenter = D3DXVECTOR3(0, 0, 0);

	m_nRotateModelButtonMask = MOUSE_LEFT_BUTTON;
	m_nZoomButtonMask = MOUSE_WHEEL;
	m_nRotateCameraButtonMask = MOUSE_RIGHT_BUTTON;
}


SModelViewCamera::~SModelViewCamera()
{
}
