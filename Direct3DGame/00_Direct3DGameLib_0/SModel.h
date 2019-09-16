#pragma once
#include "SObjStd.h"
#include "STextureManager.h"

class SModel
{
protected:
	ID3D11Device*							m_pDevice;
	ID3D11DeviceContext*					m_pContext;
	DXGame::SDxHelperEX						m_dxobj;

	STexture*							m_pTexture;

	VS_CONSTANT_BUFFER			m_cbData;
	D3DXVECTOR3					m_vCenter;
	D3DXMATRIX					m_matWorld;
	D3DXMATRIX					m_matView;
	D3DXMATRIX					m_matProj;
	vector<DWORD>				m_IndexList;
	vector<PNCT_VERTEX>			m_VertexList;

public:
	//----------------------------------------------------------------------------------------------------------
	// Getter
	//----------------------------------------------------------------------------------------------------------
	ID3D11Buffer*				GetVB();
	ID3D11Buffer*				GetIB();
	virtual	int					GetMeshCounter() { return 0; }

	DXGame::SDxHelperEX& GetDxObjHelper();
	const STexture* GetSTexture() { return m_pTexture; }
	VS_CONSTANT_BUFFER& GetConstantBuffer() { return m_cbData; }
	D3DXVECTOR3& GetCenter() { return m_vCenter; }
	D3DXMATRIX&	GetWorldMatrix() { return m_matWorld; }
	D3DXMATRIX&	GetViewMatrix() { return m_matView; }
	D3DXMATRIX&	GetProjMatrix() { return m_matProj; }
	vector<DWORD>& GetIndexList() { return m_IndexList; }
	vector<PNCT_VERTEX>& GetVertexList() { return m_VertexList; }

	//----------------------------------------------------------------------------------------------------------
	// Setter
	//----------------------------------------------------------------------------------------------------------
	virtual void				SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj);
	virtual void				SetAmbientColor(float fR, float fG, float fB, float fA);
	virtual HRESULT				SetInputLayout();
	void						SetConstantBuffer(VS_CONSTANT_BUFFER& cbData) { m_cbData = cbData; }
	//----------------------------------------------------------------------------------------------------------
	// Create
	//----------------------------------------------------------------------------------------------------------
	virtual bool				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pLoadTextureString = nullptr, const TCHAR* pLoadShaderFile = nullptr);
	virtual bool				CreateVertexData();
	virtual bool				CreateIndexData();
	virtual HRESULT				CreateVertexBuffer();
	virtual HRESULT				CreateIndexBuffer();
	virtual HRESULT				CreateConstantBuffer();
	virtual HRESULT				CreateResource();
	//----------------------------------------------------------------------------------------------------------
	// Load
	//----------------------------------------------------------------------------------------------------------

	virtual bool				Load(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* szLoadName, const TCHAR* pLoadShaderFile, bool bThread = false);
	virtual HRESULT				LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pLoadShaderFile = nullptr);
	virtual HRESULT				LoadTextures(ID3D11Device* pDevice, const TCHAR* pLoadTextureString);

	//----------------------------------------------------------------------------------------------------------
	// Frame Work
	//----------------------------------------------------------------------------------------------------------
	virtual bool				Init();
	virtual bool				Frame();
	virtual bool				PreRender(ID3D11DeviceContext* pContext);
	virtual bool				Render(ID3D11DeviceContext*		pContext);
	virtual bool				PostRender(ID3D11DeviceContext* pContext);
	virtual bool				Release();

	//----------------------------------------------------------------------------------------------------------
	// Draw
	//----------------------------------------------------------------------------------------------------------
	virtual bool				Convert(ID3D11Device* pDevice);
	virtual HRESULT				DeleteResource();
	virtual bool				UpdateBuffer();
	// 지연 컨텍스트의 사용시에 즉시 컨텍스트로 변경하기 위해서 로직에서 가상 함수화 하였다.
	virtual void				UpdateConstantBuffer(ID3D11DeviceContext* pContext, SModel* pParent = nullptr);
	virtual bool				Draw(ID3D11DeviceContext* pContext, D3DXVECTOR3 vStart, D3DXVECTOR3 vPost, D3DXVECTOR4 dwColor);
	
	//----------------------------------------------------------------------------------------------------------
	// Property
	//----------------------------------------------------------------------------------------------------------

	_declspec(property(get = GetDxObjHelper, put = SetDxObjHelper))		DXGame::SDxHelperEX&	_dxobj;
	_declspec(property(get = GetSTexture))			STexture*				_pTexture;
	_declspec(property(get = GetConstantBuffer))	VS_CONSTANT_BUFFER&		_cbData;
	_declspec(property(get = GetCenter))			D3DXVECTOR3&			_vCenter;
	_declspec(property(get = GetWorldMatrix))		D3DXMATRIX&				_matWorld;
	_declspec(property(get = GetViewMatrix))		D3DXMATRIX&				_matView;
	_declspec(property(get = GetProjMatrix))		D3DXMATRIX&				_matProj;
	_declspec(property(get = GetIndexList))			vector<DWORD>&			_IndexList;
	_declspec(property(get = GetVertexList))		vector<PNCT_VERTEX>&	_VertexList;


	SModel();
	virtual ~SModel();
};
