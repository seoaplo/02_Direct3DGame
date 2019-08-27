#pragma once
#include "SDxBasic.h"
#include "SUtils.h"

class SDevice
{
public:
	
	//==============================================================================================
	// DirectX Interface Pointer
	//==============================================================================================
	ID3D11Device*			m_pD3DDevice;				// DirectX Interface Device ver.11	( Resource )
	ID3D11DeviceContext*	m_pImmediateContext;		// DirectX Interface DeviceContext ver.11 ( Rendering )
	ID3D11RenderTargetView*	m_pRenderTargetView;		// DirectX Interface RenderTargetView ver.11	( Camera )

	IDXGIFactory*			m_pDxgiFactory;				// DirectX Interface Factory ( Create Interface )
	IDXGISwapChain*			m_pDxgiSwapChain;			// DirectX interface SwapChain ( Buffers )
	
	//==============================================================================================
	// DirectX Enum or Structure
	//==============================================================================================
	D3D_DRIVER_TYPE			m_DriverType;				// Device Type
	D3D_FEATURE_LEVEL		m_FeatureLevel;				// Device Feature Level
	D3D11_VIEWPORT			m_ViewPort;					// ViewPort
	BOOL					m_IsFullSceenMode;			// FullScreen Flag
	DXGI_SWAP_CHAIN_DESC	m_SwapChainDesc;
public:
	//==============================================================================================
	// Getter and Setter Functions
	//==============================================================================================
	BOOL					GetFullScreenModeFlag() { assert(m_IsFullSceenMode); return m_IsFullSceenMode; }
	void					SetFullScreenModeFlag(BOOL bFlag){  m_IsFullSceenMode = bFlag; }
	ID3D11Device*			GetDevice() { assert(m_pD3DDevice); return m_pD3DDevice;}
	ID3D11DeviceContext*	GetContext(){ assert(m_pImmediateContext); return m_pImmediateContext; }
	ID3D11RenderTargetView*	GetRenderTargetView(){ assert(m_pRenderTargetView); return m_pRenderTargetView; }
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
	HRESULT		SetRenderTargetView();	// Create RanderTargetView
	HRESULT		SetViewPort();			// ViewPort Binding

public:
	HRESULT		ReSizeDevice(UINT iWidth, UINT iHeight);	// Resize Device
	bool		CleanupDevice();		// Release this

public:
	SDevice();
	virtual ~SDevice();
};

