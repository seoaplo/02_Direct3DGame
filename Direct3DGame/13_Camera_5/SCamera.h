#pragma once

#include "STimer.h"
#include "SArcBall.h"
#include "SInputManager.h"

#define MOUSE_LEFT_BUTTON   0x01
#define MOUSE_MIDDLE_BUTTON 0x02
#define MOUSE_RIGHT_BUTTON  0x04
#define MOUSE_WHEEL         0x08

class SCamera
{
protected:
	SArcBall	m_WorldArcBall;
	SArcBall	m_ViewArcBall;

protected:
	//==============================================================================
	// 모델
	//==============================================================================
	D3DXVECTOR3	m_vModelCenter;
	D3DXMATRIX	m_matModelLastRot;
	D3DXMATRIX	m_matModelRot;
	D3DXMATRIX	m_matCameraRotLast;

protected:
	//==============================================================================
	// 뷰 회전에 대한 쿼터니언
	//==============================================================================
	D3DXQUATERNION m_qRotation;

	//==============================================================================
	// 각 축에 대한 변위값 w = 반지름( 원점으로 부터의 거리 )
	//==============================================================================
	D3DXVECTOR3	m_vPosDelta;
protected:
	//==============================================================================
	// 카메라 벡터
	//==============================================================================
	D3DXVECTOR3	m_vLook;
	D3DXVECTOR3	m_vUp;
	D3DXVECTOR3	m_vRight;
	D3DXVECTOR3	m_vPos;
	D3DXVECTOR3	m_vTarget;
	D3DXVECTOR3 m_vDefaultEye;		// Default camera eye position
	D3DXVECTOR3 m_vDefaultLookAt;	// Default LookAt position
protected:
	//==============================================================================
	// 카메라 행렬
	//==============================================================================
	D3DXMATRIX		m_matWorld;
	D3DXMATRIX		m_matView;
	D3DXMATRIX		m_matProj;
protected:
	//==============================================================================
	// 투영 행렬의 속성 값
	//==============================================================================
	float		m_fFov;
	float		m_fAspect;
	float		m_fNearPlane;
	float		m_fFarPlane;
	//==============================================================================
	// 오일러 각 성분
	//==============================================================================
	float		m_fYawAngle;
	float		m_fPitchAngle;
	float		m_fRollAngle;
	//==============================================================================
	// 반지름에 대한 변수
	//==============================================================================
	float		m_fRadius;
	float		m_fDefaultRadius;
	float		m_fMinRadius;
	float		m_fMaxRadius;
protected:
	//==============================================================================
	// 마우스 및 버튼의 사용여부를 판단하는 마스크 값과 마우스 휠 변위값
	//==============================================================================
	int			m_nMouseWheelDelta;			// Amount of middle wheel scroll (+/-)
	int			m_nRotateModelButtonMask;
	int			m_nZoomButtonMask;
	int			m_nRotateCameraButtonMask;
protected:
	float		m_fSpeed;
public:
	//==============================================================================
	// Game Frame Work
	//==============================================================================
	virtual bool	Frame();
	virtual int WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	void SetRadius(float fDefaultRadius = 5.0f, float fMinRadius = 1.0f, float fMaxRadius = FLT_MAX);
	virtual void SetButtonMasks(int nRotateModelButtonMask, int nZoomButtonMask, int nRotateCameraButtonMask) {	m_nRotateModelButtonMask = nRotateModelButtonMask,	m_nZoomButtonMask = nZoomButtonMask; m_nRotateCameraButtonMask = nRotateCameraButtonMask;	}
	virtual void SetWindow(int nWidth, int nHeight, float fArcballRadius = 0.9f);
public:
	//==============================================================================
	// 카메라 이동
	//==============================================================================
	virtual void	MoveLook(float fValue);
	virtual void	MoveSide(float fValue);
	virtual void	MoveUp(float fValue);
	//==============================================================================
	// 추가 : 뷰 방향벡터 저장
	//==============================================================================
	virtual D3DXMATRIX		UpdateVector();
	//==============================================================================
	// 오일러 각에 따른 뷰 행렬 계산
	//==============================================================================
	virtual D3DXMATRIX		Update(D3DXVECTOR4 vDirValue);

public:
	//==============================================================================
	// 카메라 행렬 계산
	//==============================================================================
	virtual D3DXMATRIX	SetViewMatrix(D3DXVECTOR3 vPos, D3DXVECTOR3 vTarget, D3DXVECTOR3 vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	virtual D3DXMATRIX	SetObjectView(D3DXVECTOR3 vMax, D3DXVECTOR3 vMin);
	virtual D3DXMATRIX	SetProjMatrix(float fFov,  float fAspect,  float fNearPlane,  float fFarPlane);
	//virtual D3DXMATRIX	SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj);
	//==============================================================================
	// 모델 회전 중심 위치를 지정한다.
	//==============================================================================
	void	SetModelCenter(D3DXVECTOR3 vModelCenter){ m_vModelCenter = vModelCenter; }
	//==============================================================================
	// 벡터 포지션
	//==============================================================================
	void	SetPosition(D3DXVECTOR3 vPos)		{ m_vPos = vPos; }
	void	SetLook(D3DXVECTOR3 vLook)		{ m_vLook = vLook; }
	void	SetTargetPos(D3DXVECTOR3 vTargetPos)	{ m_vTarget = vTargetPos; }
public:
	//==============================================================================
	// 카메라 벡터
	//==============================================================================
	D3DXVECTOR3&	GetLookVector() { return m_vLook; }
	D3DXVECTOR3&	GetUpVector() { return m_vUp; }
	D3DXVECTOR3&	GetRightVector() { return m_vRight; }
			   
	D3DXVECTOR3&	GetPositionVector() { return m_vPos; }
	D3DXVECTOR3&	GetTargetVector() { return m_vTarget; }
public:
	//==============================================================================
	// 카메라 행렬
	//==============================================================================
	D3DXMATRIX& GetWorldMatrix() { return m_matWorld; }
	D3DXMATRIX&	GetViewMatrix() { return m_matView; }
	D3DXMATRIX&	GetProjMatrix() { return m_matProj; }
public:
	//==============================================================================
	// 투영 행렬의 속성 값
	//==============================================================================
	float GetFov() { return m_fFov; }
	float GetAspect() { return m_fAspect; }
	float GetNearClip() { return m_fNearPlane; }
	float GetFarClip() { return m_fFarPlane; }
	//==============================================================================
	// 오일러 각 성분
	//==============================================================================
	float GetYaw() { return m_fYawAngle; }
	float GetPitch() { return m_fPitchAngle; }
	float GetRoll() { return m_fRollAngle; }
	//==============================================================================
	// 반지름에 대한 변수
	//==============================================================================
	float GetRadius() { return m_fRadius; }
public:
	SCamera();
	virtual ~SCamera();
};

