#pragma once

#include "STimer.h"
#include "SDxBasic.h"

class SCamera
{
protected:
	//==============================================================================
	// �� ȸ���� ���� ���ʹϾ�
	//==============================================================================
	D3DXQUATERNION m_qRotation;

protected:
	//==============================================================================
	// ī�޶� ����
	//==============================================================================
	D3DXVECTOR3	m_vLook;
	D3DXVECTOR3	m_vUp;
	D3DXVECTOR3	m_vRight;

	D3DXVECTOR3	m_vPos;
	D3DXVECTOR3	m_vTarget;
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
public:
	//==============================================================================
	// Game Frame Work
	//==============================================================================
	virtual bool	Init();
	virtual bool	Frame();
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
	virtual D3DXMATRIX	SetViewMatrix(D3DXVECTOR3 p, D3DXVECTOR3 t, D3DXVECTOR3 u = D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	virtual D3DXMATRIX	SetObjectView(D3DXVECTOR3 vMax, D3DXVECTOR3 vMin);
	virtual D3DXMATRIX	SetProjMatrix(float fFov,  float fAspect,  float fNearPlane,  float fFarPlane);
	//==============================================================================
	// �� ȸ�� �߽� ��ġ�� �����Ѵ�.
	//==============================================================================
	void	SetTargetPos(D3DXVECTOR3 vPos) { m_vTarget = vPos; }
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
	~SCamera();
};

