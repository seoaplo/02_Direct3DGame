#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "SUtils.h"
#include "SDxHelperEX.h"
#include "STextureManager.h"
#include "STimer.h"

//====================================================================
// Structures
//====================================================================
struct SObjectInfo
{
	D3DXVECTOR3 InitPos;
	int			iTextureID;
	SObjectInfo() {
		iTextureID = -1;
	}
	SObjectInfo(D3DXVECTOR3 p)
	{
		InitPos = p;
		iTextureID = -1;
	}
};
class SDXObject
{
public:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;

	DXGame::SDxHelperEX		m_dxObj;
	T_STR					m_ClassName;
protected:
	STexture*				m_pTexture;
	D3DXVECTOR3				m_Pos;
	
	float    m_fAlpha;
public:
	void SetDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	{
		m_pDevice = pDevice;
		m_pDeviceContext = pDeviceContext;
	}

public:
	D3DXVECTOR3   GetPos() { return m_Pos; }
	STexture* GetTexture() { return m_pTexture; }

	void     SetPos(D3DXVECTOR3 pos);
	void     SetPos(float x, float y);
	void	 Move(D3DXVECTOR3 pos);
public:
	virtual bool	Init();
	virtual void	Set(D3DXVECTOR3 pos);
	virtual void	Set(ID3D11Device*			pDevice, ID3D11DeviceContext*	pImmediateContext, SObjectInfo info);
	virtual bool    Load(ID3D11Device* pDevice, std::wstring filename);
	virtual bool	Frame();
	virtual bool	Render(ID3D11DeviceContext*	pContext);
	virtual bool	Release();

public:
	SDXObject();
	~SDXObject();
};

