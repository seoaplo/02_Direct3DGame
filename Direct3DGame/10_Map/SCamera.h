#pragma once

#include "STimer.h"
#include "SDxBasic.h"

class SCamera
{
private:
	//==============================================================================
	// 카메라 벡터
	//==============================================================================
	D3DXVECTOR3	m_vLook;
	D3DXVECTOR3	m_vUp;
	D3DXVECTOR3	m_vRight;

	D3DXVECTOR3	m_vPos;
	D3DXVECTOR3	m_vTarget;
private:
	//==============================================================================
	// 카메라 행렬
	//==============================================================================
	D3DXMATRIX		m_matWorld;
	D3DXMATRIX		m_matView;
	D3DXMATRIX		m_matProj;
private:
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
	float		m_Radius;
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
	// 카메라 행렬 계산
	//==============================================================================
	virtual D3DXMATRIX	SetViewMatrix(D3DXVECTOR3 p, D3DXVECTOR3 t, D3DXVECTOR3 u = D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	virtual D3DXMATRIX	SetObjectView(D3DXVECTOR3 vMax, D3DXVECTOR3 vMin);
	virtual D3DXMATRIX	SetProjMatrix(float fFov,  float fAspect,  float fNearPlane,  float fFarPlane);
public:
	//==============================================================================
	// 카메라 이동
	//==============================================================================
	virtual void	MoveLook(float fValue);
	virtual void	MoveSide(float fValue);
	virtual void	MoveUp(float fValue);

public:
	//==============================================================================
	// 카메라 벡터
	//==============================================================================
	D3DXVECTOR3*	GetLookVector() { return &m_vLook; }
	D3DXVECTOR3*	GetUpVector() { return &m_vUp; }
	D3DXVECTOR3*	GetRightVector() { return &m_vRight; }
			   
	D3DXVECTOR3*	GetPositionVector() { return &m_vPos; }
	D3DXVECTOR3*	GetTargetVector() { return &m_vTarget; }
public:
	//==============================================================================
	// 카메라 행렬
	//==============================================================================
	D3DXMATRIX* GetWorldMatrix() { return &m_matWorld; }
	D3DXMATRIX*	GetViewMatrix() { return &m_matView; }
	D3DXMATRIX*	GetProjMatrix() { return &m_matProj; }
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
	float GetRadius() { return m_Radius; }
public:
	SCamera();
	~SCamera();
};

