#pragma once
#include "SMesh.h"

class SSkinMesh : public SMesh
{
public:
	vector<IW_VERTEX>								m_IW_VertexList;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			m_pIW_VertexBuffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer>               m_pBoneVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>  m_pBoneVertexBufferUAV;
public:
	//----------------------------------------------------------------------------------------------------------
	// Create
	//----------------------------------------------------------------------------------------------------------
	bool				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, STextureList* pLoadTextureList = nullptr, const TCHAR* pLoadShaderFile = nullptr);
	bool				CreateVertexData()override;
	bool				CreateIndexData()override;
	HRESULT				CreateVertexBuffer()override;
	HRESULT				CreateIndexBuffer()override;
	HRESULT				CreateResource()override;
	HRESULT				SetInputLayout()override;
	//virtual HRESULT				CreateConstantBuffer();
	//----------------------------------------------------------------------------------------------------------
	// Load
	//----------------------------------------------------------------------------------------------------------

	virtual bool				Load(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* szLoadName, const TCHAR* pLoadShaderFile, bool bThread = false)override;
	HRESULT						LoadTextures(ID3D11Device* pDevice);
	void						SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj)override;
	//----------------------------------------------------------------------------------------------------------
	// Frame Work
	//----------------------------------------------------------------------------------------------------------
	virtual bool				Init()	override;
	virtual bool				Frame()override;
	virtual bool				PreRender(ID3D11DeviceContext* pContext)override;
	virtual bool				Render(ID3D11DeviceContext*		pContext)override;
	virtual bool				PostRender(ID3D11DeviceContext* pContext)override;
	virtual bool				Release()override;
public:
	SSkinMesh();
	~SSkinMesh();
};