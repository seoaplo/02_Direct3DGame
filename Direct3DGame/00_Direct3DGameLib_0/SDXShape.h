#pragma once
#include "SModel.h"



class SDXShape : public SModel
{
protected:
	//==========================================================================
	// Collision Data
	//==========================================================================
	S_BOX		m_Box;
	S_SPHERE	m_Sphere;
	S_PLANE		m_Plane;
public:
	void CreateOBBBox(	float fExtX = 1.0f, float ExtY = 1.0f, float fExtZ = 1.0f,
						D3DXVECTOR3 vCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
						D3DXVECTOR3 vDirX = D3DXVECTOR3(1.0f, 0.0f, 0.0f),
						D3DXVECTOR3 vDirY = D3DXVECTOR3(0.0f, 1.0f, 0.0f),
						D3DXVECTOR3 vDirZ = D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	void CreateAABBBox(	D3DXVECTOR3 vMaxX = D3DXVECTOR3(1.0f, 1.0f, 1.0f),
						D3DXVECTOR3 vMinX = D3DXVECTOR3(-1.0f, -1.0f, -1.0f));
public:
	SDXShape() {}
	virtual ~SDXShape() {};
};

class SPoint : public SDXShape
{
private:
	PC_VERTEX m_vPosition;
public:
	HRESULT		SetInputLayout()		override;
	bool		CreateVertexData()		override;
	bool		CreateIndexData()		override;
	HRESULT		CreateVertexBuffer()	override;
	HRESULT		CreateResource()		override;
	HRESULT		LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pLoadShaderFile = nullptr) override;
	bool		Draw(ID3D11DeviceContext* pContext, D3DXVECTOR3 vPosition, D3DXVECTOR4 vColor);

public:
	SPoint();
	virtual ~SPoint();
};

class SLine : public SDXShape
{
private:
	vector<PC_VERTEX> m_LineVertexList;
public:
	HRESULT		SetInputLayout()		override;
	bool		CreateVertexData()		override;
	bool		CreateIndexData()		override;
	HRESULT		CreateVertexBuffer()	override;
	HRESULT		CreateResource()		override;
	HRESULT		LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pLoadShaderFile = nullptr) override;
	bool		Draw(ID3D11DeviceContext* pContext, D3DXVECTOR3 vStart, D3DXVECTOR3 vEnd, D3DXVECTOR4 dwColor) override;
public:
	SLine();
	virtual ~SLine();
};

class SDirection : public SDXShape
{
private:
	vector<PC_VERTEX>	m_LineVertexList;
public:
	HRESULT			SetInputLayout()		override;
	bool			CreateVertexData()		override;
	bool			CreateIndexData()		override;
	HRESULT			CreateVertexBuffer()	override;
	HRESULT			CreateResource()		override;
	HRESULT		LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pLoadShaderFile = nullptr) override;
public:
	SDirection();
	virtual ~SDirection();
};

class SPlane : public SDXShape
{
public:
	bool			CreateVertexData()	override;
	bool			CreateIndexData()	override;
	HRESULT			CreateResource()	override;
	bool SetScreenVertex(float x,
		float y,
		float w,
		float h,
		D3DXVECTOR2 vScreen);
	void		UpdateVertexData(PLANE_PNCTLIST& VertexDataList);
	HRESULT		LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pLoadShaderFile = nullptr) override;
public:
	SPlane();
	virtual ~SPlane();
};

class SBox : public SDXShape
{
public:
	bool			CreateVertexData()	override;
	bool			CreateIndexData()	override;
	HRESULT			CreateResource()	override;
	void			UpdateVertexData(BOX_VERTEX_LIST& VertexDataList);
	HRESULT			LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pLoadShaderFile = nullptr) override;
public:
	SBox();
	virtual ~SBox();
};

