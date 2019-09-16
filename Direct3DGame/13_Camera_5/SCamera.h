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
	// ��
	//==============================================================================
	D3DXVECTOR3	m_vModelCenter;
	D3DXMATRIX	m_matModelLastRot;
	D3DXMATRIX	m_matModelRot;
	D3DXMATRIX	m_matCameraRotLast;

protected:
	//==============================================================================
	// �� ȸ���� ���� ���ʹϾ�
	//==============================================================================
	D3DXQUATERNION m_qRotation;

	//==============================================================================
	// �� �࿡ ���� ������ w = ������( �������� ������ �Ÿ� )
	//==============================================================================
	D3DXVECTOR3	m_vPosDelta;
protected:
	//==============================================================================
	// ī�޶� ����
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
	// ī�޶� ���
	//==============================================================================
	D3DXMATRIX		m_matWorld;
	D3DXMATRIX		m_matView;
	D3DXMATRIX		m_matProj;
protected:
	//==============================================================================
	// ���� ����� �Ӽ� ��
	//==============================================================================
	float		m_fFov;
	float		m_fAspect;
	float		m_fNearPlane;
	float		m_fFarPlane;
	//==============================================================================
	// ���Ϸ� �� ����
	//==============================================================================
	float		m_fYawAngle;
	float		m_fPitchAngle;
	float		m_fRollAngle;
	//==============================================================================
	// �������� ���� ����
	//==============================================================================
	float		m_fRadius;
	float		m_fDefaultRadius;
	float		m_fMinRadius;
	float		m_fMaxRadius;
protected:
	//==============================================================================
	// ���콺 �� ��ư�� ��뿩�θ� �Ǵ��ϴ� ����ũ ���� ���콺 �� ������
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
	// ī�޶� �̵�
	//==============================================================================
	virtual void	MoveLook(float fValue);
	virtual void	MoveSide(float fValue);
	virtual void	MoveUp(float fValue);
	//==============================================================================
	// �߰� : �� ���⺤�� ����
	//==============================================================================
	virtual D3DXMATRIX		UpdateVector();
	//==============================================================================
	// ���Ϸ� ���� ���� �� ��� ���
	//==============================================================================
	virtual D3DXMATRIX		Update(D3DXVECTOR4 vDirValue);

public:
	//==============================================================================
	// ī�޶� ��� ���
	//==============================================================================
	virtual D3DXMATRIX	SetViewMatrix(D3DXVECTOR3 vPos, D3DXVECTOR3 vTarget, D3DXVECTOR3 vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	virtual D3DXMATRIX	SetObjectView(D3DXVECTOR3 vMax, D3DXVECTOR3 vMin);
	virtual D3DXMATRIX	SetProjMatrix(float fFov,  float fAspect,  float fNearPlane,  float fFarPlane);
	//virtual D3DXMATRIX	SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj);
	//==============================================================================
	// �� ȸ�� �߽� ��ġ�� �����Ѵ�.
	//==============================================================================
	void	SetModelCenter(D3DXVECTOR3 vModelCenter){ m_vModelCenter = vModelCenter; }
	//==============================================================================
	// ���� ������
	//==============================================================================
	void	SetPosition(D3DXVECTOR3 vPos)		{ m_vPos = vPos; }
	void	SetLook(D3DXVECTOR3 vLook)		{ m_vLook = vLook; }
	void	SetTargetPos(D3DXVECTOR3 vTargetPos)	{ m_vTarget = vTargetPos; }
public:
	//==============================================================================
	// ī�޶� ����
	//==============================================================================
	D3DXVECTOR3&	GetLookVector() { return m_vLook; }
	D3DXVECTOR3&	GetUpVector() { return m_vUp; }
	D3DXVECTOR3&	GetRightVector() { return m_vRight; }
			   
	D3DXVECTOR3&	GetPositionVector() { return m_vPos; }
	D3DXVECTOR3&	GetTargetVector() { return m_vTarget; }
public:
	//==============================================================================
	// ī�޶� ���
	//==============================================================================
	D3DXMATRIX& GetWorldMatrix() { return m_matWorld; }
	D3DXMATRIX&	GetViewMatrix() { return m_matView; }
	D3DXMATRIX&	GetProjMatrix() { return m_matProj; }
public:
	//==============================================================================
	// ���� ����� �Ӽ� ��
	//==============================================================================
	float GetFov() { return m_fFov; }
	float GetAspect() { return m_fAspect; }
	float GetNearClip() { return m_fNearPlane; }
	float GetFarClip() { return m_fFarPlane; }
	//==============================================================================
	// ���Ϸ� �� ����
	//==============================================================================
	float GetYaw() { return m_fYawAngle; }
	float GetPitch() { return m_fPitchAngle; }
	float GetRoll() { return m_fRollAngle; }
	//==============================================================================
	// �������� ���� ����
	//==============================================================================
	float GetRadius() { return m_fRadius; }
public:
	SCamera();
	virtual ~SCamera();
};

