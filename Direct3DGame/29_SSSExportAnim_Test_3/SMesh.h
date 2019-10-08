#pragma once
#include "SModel.h"
#include "SMaterial.h"
const enum MAXSDK_MaterialTextureType
{
	MaxTexType_AmbientColor = 0,
	MaxTexType_DiffuseColor,
	MaxTexType_SpecularColor,
	MaxTexType_SpecularLevel,
	MaxTexType_Glossiness,
	MaxTexType_Self_IIIumination,
	MaxTexType_Opacity,
	MaxTexType_FilterColor,
	MaxTexType_Bump,
	MaxTexType_Reflection1,
	MaxTexType_Reflection2,
	MaxTexType_Displacement,
	MaxTexType_Number,
};


struct SScene
{
	int   iFirstFrame;  // 0
	int   iLastFrame;   // 100
	int   iFrameSpeed; // 1 Scecond = 30 Frame
	int   iTickPerFrame; // 1 Frame = 160 Tick
	int   iNumObjects;
	int   iNumMaterials;
};

class SMesh : public SModel
{
public:
	int	iSubMtrlIndex;
	STextureList*	m_pTextureList;
public:
	//----------------------------------------------------------------------------------------------------------
	// Create
	//----------------------------------------------------------------------------------------------------------
	bool						Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const STextureList* pLoadTextureList = nullptr, const TCHAR* pLoadShaderFile = nullptr);
	virtual bool				CreateVertexData()override;
	virtual bool				CreateIndexData()override;
	virtual HRESULT				CreateVertexBuffer()override;
	virtual HRESULT				CreateIndexBuffer()override;
	//virtual HRESULT				CreateConstantBuffer();
	virtual HRESULT				CreateResource()override;
	//----------------------------------------------------------------------------------------------------------
	// Load
	//----------------------------------------------------------------------------------------------------------

	virtual bool				Load(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* szLoadName, const TCHAR* pLoadShaderFile, bool bThread = false)override;
	virtual HRESULT				LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pLoadShaderFile = nullptr)override;
	HRESULT						LoadTextures(ID3D11Device* pDevice);

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


