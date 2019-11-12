#pragma once
#include "SModel.h"
#include "SMaterial.h"
#include "SCAHeader.h"

class SMesh : public SModel
{
public:
	int	iSubMtrlIndex;
	STextureList	m_TextureList;
public:
	//----------------------------------------------------------------------------------------------------------
	// Create
	//----------------------------------------------------------------------------------------------------------
	virtual bool				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, STextureList* pLoadTextureList = nullptr, const TCHAR* pLoadShaderFile = nullptr);
	virtual bool				CreateVertexData()override;
	virtual bool				CreateIndexData()override;
	virtual HRESULT				CreateVertexBuffer()override;
	virtual HRESULT				CreateIndexBuffer()override;
	virtual HRESULT				CreateResource()override;
	virtual HRESULT				SetInputLayout()override;
	//virtual HRESULT				CreateConstantBuffer();
	//----------------------------------------------------------------------------------------------------------
	// Load
	//----------------------------------------------------------------------------------------------------------

	virtual bool				Load(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* szLoadName, const TCHAR* pLoadShaderFile, bool bThread = false)override;
	virtual HRESULT				LoadTextures(ID3D11Device* pDevice);
	virtual void						SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj)override;
	//----------------------------------------------------------------------------------------------------------
	// Frame Work
	//----------------------------------------------------------------------------------------------------------
	virtual bool				Init()	override;	
	virtual bool				Frame()override;
	virtual bool				PreRender(ID3D11DeviceContext* pContext)override;
	virtual bool				Render(ID3D11DeviceContext*		pContext)override;
	virtual bool				PostRender(ID3D11DeviceContext* pContext)override;
	virtual bool				Release()override;
};


