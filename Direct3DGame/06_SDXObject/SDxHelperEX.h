#pragma once
#include "SDxBasic.h"

namespace DXGame
{
	HRESULT CompilerShaderFromFile(	const TCHAR* sFileName,
									const char* szEntryPoint, const char* szShaderModel,
									ID3DBlob** ppBlobOut);

	ID3D11VertexShader* LoadVertexShaderFile(ID3D11Device* pDevice, const void* pShaderFileData,
								ID3DBlob** ppBlobOut = nullptr, const char* pFunctionName = nullptr,
								bool bBinary = false);

	ID3D11PixelShader* LoadPixelShaderFile(ID3D11Device* pDevice, const void* pShaderFileData,
		ID3DBlob** ppBlobOut = nullptr, const char* pFunctionName = nullptr,
		bool bBinary = false);

	ID3D11InputLayout* CreateInputLayout(	ID3D11Device* pDevice, DWORD dwSize,
											const void* lpData, D3D11_INPUT_ELEMENT_DESC* layout,
											UINT numElements);

	ID3D11Buffer* CreateVertexBuffer(	ID3D11Device* pDevice, void* pData,
										UINT iNumCount, UINT iSize,
										bool bDynamic = false);

	ID3D11Buffer* CreateIndexBuffer(ID3D11Device* pDevice, void* pData,
		UINT iNumCount, UINT iSize,
		bool bDynamic = false);

	ID3D11Buffer* CreateConstantBuffer(ID3D11Device* pDevice, void* pData,
		UINT iNumCount, UINT iSize,
		bool bDynamic = false);

	class SDxHelperEX
	{
	public:
		UINT		m_iNumVertex;
		UINT		m_iNumIndex;
	public:
		void PreRender(ID3D11DeviceContext* pContext, UINT iSize);
		void PostRender(ID3D11DeviceContext* pContext, UINT iCount);
		bool Render(ID3D11DeviceContext* pContext, UINT iSize, UINT iCount);
	public:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBuffer;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;

		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pSRV;
		Microsoft::WRL::ComPtr<ID3DBlob> m_pVSBlob;

	public:
		SDxHelperEX();
		~SDxHelperEX();
	};
}