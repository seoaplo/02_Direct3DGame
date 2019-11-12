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
	int Load(T_STR FilePath);
public:
private:
	SkinFileLoader();
public:
	~SkinFileLoader();
};

#define I_SkinFileLoaderManeger SkinFileLoader::GetInstance()
