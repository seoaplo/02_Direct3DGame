#pragma once
#include "SMatrix.h"
#include "STimer.h"

class SCamera
{
public:
	DXGame::SMatrix		m_matView;
	DXGame::SVector3	m_vLook;
	DXGame::SVector3	m_vUp;
	DXGame::SVector3	m_vRight;
	DXGame::SVector3	m_vPos;
	DXGame::SVector3	m_vTarget;
public:
	DXGame::SMatrix		m_matWorld;
	DXGame::SMatrix		m_matProj;
public:
	bool	Init();
	void	CreateViewMatrix(DXGame::SVector3 p, DXGame::SVector3 t, DXGame::SVector3 u = DXGame::SVector3(0.0f, 1.0f, 0.0f));
	void	CreateProjMatrix(RECT ClientRect);
	void	Forward();
	void	Backward();
	void	RightRotation();
	void	LeftRotation();
	bool	Frame();
public:
	SCamera();
	~SCamera();
};

