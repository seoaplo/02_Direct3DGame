#include "SCamera.h"
int SCamera::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (((uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK) && m_nRotateModelButtonMask & MOUSE_LEFT_BUTTON) ||
		((uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONDBLCLK) && m_nRotateModelButtonMask & MOUSE_MIDDLE_BUTTON) ||
		((uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONDBLCLK) && m_nRotateModelButtonMask & MOUSE_RIGHT_BUTTON))
	{
		int iMouseX = (short)LOWORD(lParam);
		int iMouseY = (short)HIWORD(lParam);
		m_WorldArcBall.OnBegin(iMouseX, iMouseY);
	}

	if (((uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK) && m_nRotateCameraButtonMask & MOUSE_LEFT_BUTTON) ||
		((uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONDBLCLK) && m_nRotateCameraButtonMask & MOUSE_MIDDLE_BUTTON) ||
		((uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONDBLCLK) && m_nRotateCameraButtonMask & MOUSE_RIGHT_BUTTON))
	{
		int iMouseX = (short)LOWORD(lParam);
		int iMouseY = (short)HIWORD(lParam);
		m_ViewArcBall.OnBegin(iMouseX, iMouseY);
	}

	if (uMsg == WM_MOUSEMOVE)
	{
		int iMouseX = (short)LOWORD(lParam);
		int iMouseY = (short)HIWORD(lParam);
		m_WorldArcBall.OnMove(iMouseX, iMouseY);
		m_ViewArcBall.OnMove(iMouseX, iMouseY);
	}

	if ((uMsg == WM_LBUTTONUP && m_nRotateModelButtonMask & MOUSE_LEFT_BUTTON) ||
		(uMsg == WM_MBUTTONUP && m_nRotateModelButtonMask & MOUSE_MIDDLE_BUTTON) ||
		(uMsg == WM_RBUTTONUP && m_nRotateModelButtonMask & MOUSE_RIGHT_BUTTON))
	{
		m_WorldArcBall.OnEnd();
	}

	if ((uMsg == WM_LBUTTONUP && m_nRotateCameraButtonMask & MOUSE_LEFT_BUTTON) ||
		(uMsg == WM_MBUTTONUP && m_nRotateCameraButtonMask & MOUSE_MIDDLE_BUTTON) ||
		(uMsg == WM_RBUTTONUP && m_nRotateCameraButtonMask & MOUSE_RIGHT_BUTTON))
	{
		m_ViewArcBall.OnEnd();
	}

	if (uMsg == WM_CAPTURECHANGED)
	{
		if ((HWND)lParam != hWnd)
		{
			if ((m_nRotateModelButtonMask & MOUSE_LEFT_BUTTON) ||
				(m_nRotateModelButtonMask & MOUSE_MIDDLE_BUTTON) ||
				(m_nRotateModelButtonMask & MOUSE_RIGHT_BUTTON))
			{
				m_WorldArcBall.OnEnd();
			}

			if ((m_nRotateCameraButtonMask & MOUSE_LEFT_BUTTON) ||
				(m_nRotateCameraButtonMask & MOUSE_MIDDLE_BUTTON) ||
				(m_nRotateCameraButtonMask & MOUSE_RIGHT_BUTTON))
			{
				m_ViewArcBall.OnEnd();
			}
		}
	}


	if (uMsg == WM_MOUSEWHEEL)
	{
		// Update member var state
		m_nMouseWheelDelta += (short)HIWORD(wParam);
	}
	return -1;
}

D3DXMATRIX SCamera::SetViewMatrix(D3DXVECTOR3 vPos, D3DXVECTOR3 vTarget, D3DXVECTOR3 vUp)
{
	m_vDefaultEye = m_vPos = vPos;
	m_vDefaultLookAt = m_vTarget = vTarget;
	D3DXMatrixLookAtLH(&m_matView, &m_vPos, &m_vTarget, &vUp);

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

	return UpdateVector();
}
D3DXMATRIX SCamera::SetObjectView(D3DXVECTOR3 vMax, D3DXVECTOR3 vMin)
{
	float fRadius = D3DXVec3Length(&(vMax - vMin)) * 0.5f;
	D3DXVECTOR3 vTarget = (vMax + vMin) * 0.5f;
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
	if (I_InputManager.MouseButtonState(0) == KEY_HOLD)
	{
		m_fYawAngle		+= D3DXToRadian(I_InputManager.m_pMouseInput->MouseLocation().x * 0.1f);
		m_fPitchAngle	+= D3DXToRadian(I_InputManager.m_pMouseInput->MouseLocation().y * 0.1f);
	}
	// 카메라 이동 거리 = 속도 + (경과시간 * 휠 변위값)
	float fValue = I_InputManager.m_pMouseInput->MouseLocation().x;
	float fDistance = m_fSpeed * fValue * I_Timer.GetSPF();

	if (I_InputManager.KeyBoardState(DIK_SPACE)) m_fSpeed += I_Timer.GetSPF() * 10.0f;
	else										 m_fSpeed -= I_Timer.GetSPF() * 10.0f;

	// 최소값으로 고정
	if (m_fSpeed < 1.0f) m_fSpeed = 1.0f;

	if (I_InputManager.KeyBoardState(DIK_W)) MoveLook(I_Timer.GetSPF() * 5.0f * m_fSpeed);
	if (I_InputManager.KeyBoardState(DIK_S)) MoveLook(-I_Timer.GetSPF() * 5.0f* m_fSpeed);
	if (I_InputManager.KeyBoardState(DIK_D)) MoveSide(I_Timer.GetSPF() * 5.0f* m_fSpeed);
	if (I_InputManager.KeyBoardState(DIK_A)) MoveSide(-I_Timer.GetSPF() * 5.0f* m_fSpeed);
	if (I_InputManager.KeyBoardState(DIK_Q)) MoveUp(I_Timer.GetSPF() * 5.0f* m_fSpeed);
	if (I_InputManager.KeyBoardState(DIK_E)) MoveUp(-I_Timer.GetSPF() * 5.0f* m_fSpeed);

	Update(D3DXVECTOR4(m_fPitchAngle, m_fYawAngle, m_fRollAngle, fDistance));
	UpdateVector();

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

void SCamera::SetRadius(float fDefaultRadius, float fMinRadius, float fMaxRadius)
{
	m_fDefaultRadius = m_fRadius = fDefaultRadius;
	m_fMinRadius = fMinRadius;
	m_fMaxRadius = fMaxRadius;
}
void SCamera::SetWindow(int nWidth, int nHeight, float fArcballRadius)
{
	m_WorldArcBall.SetWindow(nWidth, nHeight, fArcballRadius);
	m_ViewArcBall.SetWindow(nWidth, nHeight, fArcballRadius);
}


SCamera::SCamera()
{
	// 추가
	m_vPosDelta = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_fDefaultRadius = 0.0f;
	m_fMinRadius = 1.0f;
	m_fMaxRadius = FLT_MAX;
	m_fRadius = 0.0f;

	m_fYawAngle = 0.0f;
	m_fPitchAngle = 0.0f;
	m_fRollAngle = 0.0f;

	m_fSpeed = 0.0f;

	// 추가
	m_nMouseWheelDelta = 0;

	m_vModelCenter = D3DXVECTOR3(0, 0, 0);

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matModelRot);
	D3DXMatrixIdentity(&m_matModelLastRot);
	D3DXMatrixIdentity(&m_matCameraRotLast);

	m_nRotateModelButtonMask = MOUSE_LEFT_BUTTON;
	m_nZoomButtonMask = MOUSE_WHEEL;
	m_nRotateCameraButtonMask = MOUSE_RIGHT_BUTTON;


	D3DXMatrixIdentity(&m_matWorld);
	SetProjMatrix(D3DX_PI / 4.0f, 1.0f, 0.1f, 1000.0f);
	SetViewMatrix(D3DXVECTOR3(0.0f, 0.0f, 10.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));
}

SCamera::~SCamera()
{
}
