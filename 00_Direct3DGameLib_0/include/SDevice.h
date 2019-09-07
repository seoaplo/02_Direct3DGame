#pragma once
#include "SDxBasic.h"
#include "SUtils.h"
#include "SDXState.h"
#include "SDxRT.h"
class SDevice
{
public:
	DXGame::SDxRT				m_DefaultRT;
	//==============================================================================================
	// DirectX Interface Pointer
	//==============================================================================================
	ID3D11Device*				m_pDevice;					// DirectX Interface Device ver.11	( Resource )
	ID3D11DeviceContext*		m_pImmediateContext;		// DirectX Interface DeviceContext ver.11 ( Rendering )
	IDXGIFactory*				m_pDxgiFactory;				// DirectX Interface Factory ( Create Interface )
	IDXGISwapChain*				m_pDxgiSwapChain;			// DirectX interface SwapChain ( Buffers )
	
	//==============================================================================================
	// DirectX Enum or Structure
	//==============================================================================================
	D3D_DRIVER_TYPE							m_DriverType;				// Device Type
	D3D_FEATURE_LEVEL						m_FeatureLevel;				// Device Feature Level
	DXGI_SWAP_CHAIN_DESC					m_SwapChainDesc;
	BOOL									m_IsFullSceenMode;			// FullScreen Flag
public:
	//==============================================================================================
	// Getter and Setter Functions
	//==============================================================================================
	BOOL					GetFullScreenModeFlag() { assert(m_IsFullSceenMode); return m_IsFullSceenMode; }
	void					SetFullScreenModeFlag(BOOL bFlag){  m_IsFullSceenMode = bFlag; }
	ID3D11Device*			GetDevice() { assert(m_pDevice); return m_pDevice;}
	ID3D11DeviceContext*	GetContext(){ assert(m_pImmediateContext); return m_pImmediateContext; }
	ID3D11RenderTargetView*	    GetRenderTargetView() { return  m_DefaultRT.m_pRenderTargetView.Get(); }
	ID3D11RenderTargetView**	GetRenderTargetViewAddress() { return  m_DefaultRT.m_pRenderTargetView.GetAddressOf(); }
	ID3D11DepthStencilView*		GetDepthStencilView() { return m_DefaultRT.m_pDepthStencilView.Get(); }
	ID3D11DepthStencilView**	GetDepthStencilViewAddress() { return  m_DefaultRT.m_pDepthStencilView.GetAddressOf(); }
	IDXGIFactory*			GetFactory() { assert(m_pDxgiFactory); return m_pDxgiFactory; }
	IDXGISwapChain*			GetSwapChain() { assert(m_pDxgiSwapChain); return m_pDxgiSwapChain ; }

public:
	//==============================================================================================
	// Initialize Function
	//==============================================================================================
	HRESULT InitDevice(HWND hWnd, UINT iWidth, UINT iHeight, BOOL isFullScreen = FALSE);	// Initialize Enter Fuction
	
	//==============================================================================================
	// Create Device and SwapChain
	//==============================================================================================
	HRESULT		CreateDevice();			// Create Device Instance 
	HRESULT		CreateGIFactory();	// Create Factory Instance
	HRESULT		CreateSwapChain(HWND hWnd, UINT iWidth, UINT iHeight, BOOL IsFullScreen);		// Create SwapChain Instance

	//==============================================================================================
	// Setting RenderTargetView and ViewPort
	//==============================================================================================
	virtual		HRESULT		SetRenderTargetView();				// Create RanderTargetView
	virtual		HRESULT		SetViewPort();						// ViewPort Binding
	virtual		HRESULT		DeleteDxResource();		
	virtual		HRESULT		CreateDxResource();

public:
	HRESULT		ReSizeDevice(UINT iWidth, UINT iHeight);	// Resize Device
	bool		CleanupDevice();		// Release this

public:
	SDevice();
	virtual ~SDevice();
};

