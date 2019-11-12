#pragma once
#include "SDrawObject.h"
#include "SSkinObject.h"
#include "SMatrixObject.h"

class SCAObject : public SDrawObject
{
public:
	int m_iSkinObjectListSize;
	std::vector<SSkinObject*>	m_SkinObjectList;
	SMatrixObjectList*			m_pMatrixObjectList;

public:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>        m_pCS;

public:
	bool Init();
	bool Frame();
	bool Render(ID3D11DeviceContext* pContext);
	bool Release();
public:
	virtual bool Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pShaderFile = nullptr);
	//--------------------------------------------------------------------------------------
	// 계산쉐이더 관련 함수
	//--------------------------------------------------------------------------------------
	HRESULT CreateComputeShader(LPCWSTR pSrcFile, LPCSTR pFunctionName,
		ID3D11Device* pDevice, ID3D11ComputeShader** ppShaderOut);
	void RunComputeShader(ID3D11DeviceContext* pd3dImmediateContext,
		ID3D11ComputeShader* pComputeShader,
		UINT nNumViews, ID3D11ShaderResourceView** pShaderResourceViews,
		ID3D11Buffer* pCBCS, void* pCSData, DWORD dwNumDataBytes,
		ID3D11UnorderedAccessView* pUnorderedAccessView,
		UINT X, UINT Y, UINT Z);
	void CreateComputeBuffer();
	void CreateComputeData();

	bool SetMatrixObjectList(int iMatrixObejctList);
	bool SetSkinObject(int iSkinObejct);
	void ClearSkinObjectList();
public:
	SCAObject();
	virtual ~SCAObject();
};

