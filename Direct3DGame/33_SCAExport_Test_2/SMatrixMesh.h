#pragma once
#include "SMesh.h"

class SMatrixMesh : public SMesh
{
public:
	//----------------------------------------------------------------------------------------------------------
	// Create
	//----------------------------------------------------------------------------------------------------------
	virtual bool				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, STextureList* pLoadTextureList = nullptr, const TCHAR* pLoadShaderFile = nullptr);
	virtual HRESULT				LoadTextures(ID3D11Device* pDevice);
	//----------------------------------------------------------------------------------------------------------
	// Frame Work
	//----------------------------------------------------------------------------------------------------------
	virtual bool				PreRender(ID3D11DeviceContext* pContext)override;
public:
	SMatrixMesh();
	~SMatrixMesh();
};

