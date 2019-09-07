#pragma once
#include "STextureManager.h"
#include "SDXState.h"

namespace DXGame
{
	class SDxRT
	{
	public:
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pSRV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pDsvSRV; // Depth Buffer Resource View
		Microsoft::WRL::ComPtr<ID3D11Texture2D>	m_pTexture;

		D3D11_DEPTH_STENCIL_VIEW_DESC	m_DepthStencilDesc;
		D3D11_TEXTURE2D_DESC			m_TexDesc;
		DXGI_FORMAT						m_DSFormat;
		D3D11_VIEWPORT					m_vp;
		D3DXMATRIX						m_matProj;

		// Store The old render targets
		D3D11_VIEWPORT		m_vpOld[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
		UINT				m_nViewPorts;
		ID3D11RenderTargetView* m_pOldRTV;
		ID3D11DepthStencilView* m_pOldDSV;
	public:
		HRESULT Create(ID3D11Device* pDevice, float Width = 1024, float Height = 1024);
		HRESULT CreateCubeMap(ID3D11Device* pDevice, float fWidth = 1024, float fHeight = 1024);
		void	Set(ID3D11Device* pDevice, float TopLeftX, float TopLeftY, float Width, float Height,
			float MinDepth = 0, float MaxDepth = 1);
		HRESULT UpdateDepthStencilView(ID3D11Device* pDevice, UINT dwWidth, UINT dwHeight);
		void	Apply(ID3D11DeviceContext* pImmediateContext,
						ID3D11RenderTargetView* pRTV = nullptr,
						ID3D11DepthStencilView* pDepthStencilView = nullptr,
						bool bDepthClear = true, bool bStencilClear = true);
		bool	Begin(ID3D11DeviceContext* pContext,
			D3DXVECTOR4 vColor,
			bool bTargetClear = true,
			bool bDepthClear = true,
			bool bStencilClear = true);
		bool	Clear(ID3D11DeviceContext* pContext,
				D3DXVECTOR4 vColor,
				bool bTargetClear = true,
				bool bDepthClear = true,
				bool bStencilClear = true);
		bool	End(ID3D11DeviceContext* pContext);
		HRESULT SaveFile(ID3D11DeviceContext* pContext, T_STR name);
		HRESULT SaveBackBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, T_STR name);

	public:
		SDxRT();
		~SDxRT();
	};

}

