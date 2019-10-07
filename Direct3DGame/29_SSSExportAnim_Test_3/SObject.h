#pragma once
#include "SModel.h"
#include "SMesh.h"
#include "STimer.h"

enum OBJECTCLASSTYPE{
	CLASS_GEOM = 0,
	CLASS_BONE,
	CLASS_DUMMY,
	CLASS_BIPED,
};

class SObject : public SModel
{
public:
	SMaterial*	m_pMaterial;
	SMesh*		m_pMesh;
	float m_fElapseTime;
public:
	virtual HRESULT				SetInputLayout();
	bool Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pLoadShaderFile);
	virtual HRESULT				CreateVertexBuffer();
	virtual HRESULT				CreateIndexBuffer();
	virtual HRESULT				CreateConstantBuffer();
	virtual HRESULT				CreateResource();
	//----------------------------------------------------------------------------------------------------------
	// Load
	//----------------------------------------------------------------------------------------------------------

	virtual bool				Load(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* szLoadName, const TCHAR* pLoadShaderFile, bool bThread = false) { return false; }
	virtual HRESULT				LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pLoadShaderFile = nullptr);
	virtual HRESULT				LoadTextures(ID3D11Device* pDevice);

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
	virtual HRESULT				DeleteResource();
	virtual bool				UpdateBuffer();
	// 지연 컨텍스트의 사용시에 즉시 컨텍스트로 변경하기 위해서 로직에서 가상 함수화 하였다.
	virtual void				UpdateConstantBuffer(ID3D11DeviceContext* pContext, SObject* pParent = nullptr);
	virtual bool				Draw(ID3D11DeviceContext* pContext, D3DXVECTOR3 vStart, D3DXVECTOR3 vPost, D3DXVECTOR4 dwColor);


	bool GetAnimationTrack(
		float fElapseTime,
		std::vector<SAnimTrack>& trackList,
		SAnimTrack& StartTrack,
		SAnimTrack& EndTrack);

	void Interpolate(
		SMesh& sMesh,
		D3DXMATRIX &matParent,
		float fElapseTime);
public:
	SObject();
	~SObject();
};

