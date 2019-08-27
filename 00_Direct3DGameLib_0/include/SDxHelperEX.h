#pragma once
#include "SDxBasic.h"
#include "SDXState.h"
#include "STextureManager.h"

namespace DXGame
{
	// 아래의 경고가 있을 경우 사용한다.
	// 이와 같은 경고는 이미 쉐이더 파이프라인에 할당(리소스 및 상태값들이)되어 사용중일 경우에 발생한다.
	// D3D11 WARNING : ID3D11DeviceContext::SOSetTargets : Resource being set to SO Buffer slot 0 is still bound on input![STATE_SETTING WARNING #10: DEVICE_SOSEFFARGETS_HARZARD]
	// D3D11 WARNING : ID3D11DeviceContext::SOSetTargets : Forcing Vertex Buffer slot 0 to NULL.[ STATE_SETTING WARNING #1: DEVICE_IASETVERTEXBUFFERS_HARZARD]
	void ClearD3D11DeviceContext(ID3D11DeviceContext* pd3dDeviceContext);

	//==============================================================================================================
	// Load Shader
	//==============================================================================================================
	HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlopOut);
	ID3D11VertexShader*		LoadVertexShaderFile(ID3D11Device* pd3dDevice, const void* pLoadShaderFile, ID3DBlob** ppBlobOut = nullptr, char* pFuntioncName = 0, bool bBinary = false);
	ID3D11PixelShader*		LoadPixelShaderFile(ID3D11Device* pd3dDevice, const void* pLoadShaderFile, char* pFuntionName = 0, bool bBinary = false, ID3DBlob** pRetBlob = nullptr);
	ID3D11GeometryShader*	LoadGeometryShaderFile(ID3D11Device* pd3dDevice, const void* pLoadShaderFile, ID3DBlob** ppBlobOut = nullptr, char* pFuntioncName = 0, bool bBinary = false);
	ID3D11HullShader*		LoadHullShaderFile(ID3D11Device* pd3dDevice, const void* pLoadShaderFile, ID3DBlob** ppBlobOut = nullptr, char* pFuntioncName = 0, bool bBinary = false);
	ID3D11DomainShader*		LoadDomainShaderFile(ID3D11Device* pd3dDevice, const void* pLoadShaderFile, ID3DBlob** ppBlobOut = nullptr, char* pFuntioncName = 0, bool bBinary = false);
	ID3D11ComputeShader*	LoadComputeShaderFile(ID3D11Device* pd3dDevice, const void* pLoadShaderFile, ID3DBlob** ppBlobOut = nullptr, char* pFuntioncName = 0, bool bBinary = false);


	//==============================================================================================================
	// Create Layout and Buffer
	//==============================================================================================================
	ID3D11InputLayout*	CreateInputlayout(ID3D11Device* pd3dDevice, DWORD dwSize, LPCVOID lpData, D3D11_INPUT_ELEMENT_DESC* layout, UINT numElements);
	ID3D11Buffer*		CreateVertexBuffer(ID3D11Device* pd3dDevice, void *vertices, UINT iNumVertex, UINT iVertexSize, bool bDynamic = false);
	ID3D11Buffer*		CreateIndexBuffer(ID3D11Device* pd3dDevice, void *indices, UINT iNumIndex, UINT Size, bool bDynamic = false);
	ID3D11Buffer*		CreateConstantBuffer(ID3D11Device* pd3dDevice, void *data, UINT iNumIndex, UINT Size, bool bDynamic = false);

	ID3D11ShaderResourceView*	CreateShaderResourceView(ID3D11Device* pDevice, const TCHAR* strFIlePath);
	ID3D11DepthStencilView*		CreateDepthStencilView(ID3D11Device* pDevice, DWORD dwWidth, DWORD dwHeight);

	class SDxHelperEX
	{
	public:
		//==============================================================================================================
		// Shader ComPtr
		//==============================================================================================================
		Microsoft::WRL::ComPtr<ID3D11Buffer>				g_pVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>				g_pIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>				g_pConstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>			g_pVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>			g_pPixelShader;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader>		g_pGeometryShader;
		Microsoft::WRL::ComPtr<ID3D11HullShader>			g_pHullShader;
		Microsoft::WRL::ComPtr<ID3D11DomainShader>			g_pDomainShader;
		Microsoft::WRL::ComPtr<ID3D11ComputeShader>			g_pComputeShader;
		Microsoft::WRL::ComPtr<ID3DBlob>					g_pVSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob>					g_pPSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob>					g_pGSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob>					g_pHSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob>					g_pDSBlob;
		Microsoft::WRL::ComPtr<ID3DBlob>					g_pCSBlob;

		//==============================================================================================================
		// State Comptr
		//==============================================================================================================
		Microsoft::WRL::ComPtr<ID3D11InputLayout>				g_pInputlayout;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		g_pTextureSRV;


		//==============================================================================================================
		// Layout State
		//==============================================================================================================
		UINT					m_iPrimitiveType;
		UINT					m_iCullMode;
		UINT					m_iSamplerMode;
		//==============================================================================================================
		// Others
		//==============================================================================================================
		UINT					m_iNumVertex;
		UINT					m_iNumIndex;
		UINT					m_iVertexSize;
		UINT					m_iIndexSize;
		D3D11_BOX				m_BoxVB;
		D3D11_BOX				m_BoxIB;
		UINT					m_iPreVB;
		UINT					m_iPreIB;
	public:
		void PreRender(ID3D11DeviceContext*		pContext, UINT iVertexSize = 0);
		void PostRender(ID3D11DeviceContext*		pContext, UINT iCount = 0);
		bool Render(ID3D11DeviceContext*		pContext, UINT iVertexSize, UINT iCount = 0);
	public:
		SDxHelperEX();
		~SDxHelperEX();
	};
}

#define str(x) L#x
#define xstr(x) str(x)

#ifndef HRESULT_FAILE_MESSAGE
#define HRESULT_FAILE_MESSAGE(x){ if (FAILED(x)){\
		LPWSTR output;\
		WCHAR buffer[256]={0,};\
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_IGNORE_INSERTS |FORMAT_MESSAGE_ALLOCATE_BUFFER,\
		NULL,x,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR)&output,0,NULL);\
		wsprintf(buffer,L"File=%s\nLine=%s", xstr(__FILE__),xstr( __LINE__ ));\
		MessageBox(NULL, buffer,output,MB_OK);}\
	}
#endif

