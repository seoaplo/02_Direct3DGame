#pragma once
#include "SDxBasic.h"
#include "SDXShape.h"

enum S_POSITION
{
	P_BACK = 0,		// Point is positioned behind plane;
	P_FRONT,		// Point is positioned in front of plane
	P_ONPLANE,		// Point is positioned on plane
	P_SPANNING,		// Point is spanning on plane
};

class SFrustum
{
protected:
	//==============================================================================
	// 카메라 행렬
	//==============================================================================
	D3DXMATRIX		m_matWorld;
	D3DXMATRIX		m_matView;
	D3DXMATRIX		m_matProj;
	D3DXMATRIX		m_matViewProj;

	D3DXVECTOR3		m_vFrustum[8];
	vector<PNCT_VERTEX>	m_VertexList;

	D3DXPLANE		m_DxPlane[6];
	S_PLANE			m_Plane[6];
	D3DXVECTOR3		m_vNearCenter;
	SBox*			m_pBoxShape;
	ID3D11Device*	m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
public:
	bool		Init();
	bool		Frame();
	bool		PreRender(ID3D11DeviceContext*	pContext);
	bool		PostRender(ID3D11DeviceContext*	pContext);
	bool		Render(ID3D11DeviceContext*		pContext);
	bool		Release();
	bool		ResetDevice();
	bool		ResetResource() { return true; }
public:
	void	SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj);
	bool	CreateRenderBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	bool	CreateFrustum();
	void	ExtractPlanesD3D(S_PLANE* pPlanes, const D3DXMATRIX& comboMatrix);
	//==============================================================================
	// 충돌 체크
	//==============================================================================
	bool	ClassifyPoint(D3DXVECTOR3* pv);
	bool	ClassifySphere(S_SPHERE* pSphere);
	bool	CheckOBBInPlane(S_BOX*  pBox);
	S_POSITION CheckPoitionOBBInPlane(S_BOX*  pBox);

public:
	//==============================================================================
	// 카메라 행렬
	//==============================================================================
	D3DXMATRIX& GetWorldMatrix()	{ return m_matWorld; }
	D3DXMATRIX&	GetViewMatrix()		{ return m_matView; }
	D3DXMATRIX&	GetProjMatrix()		{ return m_matProj; }
	D3DXMATRIX&	GetViewProjMatrix() { return m_matViewProj; }

	SBox*		GetBoxPtr()	{ return m_pBoxShape; }
public:
	//==============================================================================
	// Property
	//==============================================================================

	// get
	_declspec(property(get = GetWorldMatrix))			D3DXMATRIX& _matWorld;
	_declspec(property(get = GetViewMatrix))			D3DXMATRIX& _matView;
	_declspec(property(get = GetProjMatrix))			D3DXMATRIX& _matProj;
	_declspec(property(get = GetViewProjMatrix))		D3DXMATRIX& _matViewProj;

	_declspec(property(get = GetBoxPtr))				SBox* _pBoxShape;
public:
	SFrustum();
	~SFrustum();
};

