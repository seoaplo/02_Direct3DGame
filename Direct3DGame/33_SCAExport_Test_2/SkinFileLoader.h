#pragma once
#include "SParser.h"
#include "STemplate.h"
#include "SSkinObject.h"

class SkinFileLoader : public Singleton<SkinFileLoader>
{
	friend class Singleton<SkinFileLoader>;
private:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;
	SParser m_Paser;
private:

	D3DXMATRIX					m_InvMatrix[AnimationMatrixList];

	ID3D11ComputeShader*        m_pCS;

	ID3D11Buffer*               m_pVertexBuffer;
	ID3D11Buffer*               m_pIWBuffer;
	ID3D11Buffer*               m_pInvMatBuffer;
	ID3D11Buffer*               m_pBoneVertexBuffer;

	ID3D11ShaderResourceView*    m_pVertexBufferSRV;
	ID3D11ShaderResourceView*    m_pIWBufferSRV;
	ID3D11ShaderResourceView*    m_pInvMatBufferSRV;
	ID3D11UnorderedAccessView*   m_pBoneVertexBufferUAV;
public:
	bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	bool Load(T_STR FilePath);
public:
	//--------------------------------------------------------------------------------------
	// 계산쉐이더 관련 함수
	//--------------------------------------------------------------------------------------
	HRESULT CreateComputeShader(LPCWSTR pSrcFile, LPCSTR pFunctionName,
		ID3D11Device* pDevice, ID3D11ComputeShader** ppShaderOut);
	HRESULT CreateStructuredBuffer(ID3D11Device* pDevice, UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBufOut);
	HRESULT CreateBufferSRV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut);
	HRESULT CreateBufferUAV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** pUAVOut);
	ID3D11Buffer* CreateAndCopyToBuf(ID3D11Device* pDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11Buffer* pBuffer);
	void RunComputeShader(ID3D11DeviceContext* pd3dImmediateContext,
		ID3D11ComputeShader* pComputeShader,
		UINT nNumViews, ID3D11ShaderResourceView** pShaderResourceViews,
		ID3D11Buffer* pCBCS, void* pCSData, DWORD dwNumDataBytes,
		ID3D11UnorderedAccessView* pUnorderedAccessView,
		UINT X, UINT Y, UINT Z);
private:
	SkinFileLoader();
public:
	~SkinFileLoader();
};

#define I_SkinFileLoaderManeger SkinFileLoader::GetInstance()
