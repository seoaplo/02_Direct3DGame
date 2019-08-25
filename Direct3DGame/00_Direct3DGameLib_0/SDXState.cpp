#include "SDXState.h"


namespace DXGame
{
	ID3D11BlendState*	   SDXState::m_pAlphaBlend = 0;
	ID3D11BlendState*	   SDXState::m_pAlphaBlendDisable = 0;
	ID3D11RasterizerState*  SDXState::m_pRSWireFrame = 0;
	ID3D11RasterizerState*  SDXState::m_pRSSolidFrame = 0;
	ID3D11SamplerState*     SDXState::m_pSSWrapLinear = 0;

	void SDXState::SetState(ID3D11Device* pd3dDevice)
	{
		if (pd3dDevice == nullptr) return;

		HRESULT hr;
#pragma region ID3D11SamplerState
		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(D3D11_SAMPLER_DESC));
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.MaxLOD = FLT_MAX;
		sd.MinLOD = FLT_MIN;
		if (FAILED(hr = pd3dDevice->CreateSamplerState(
			&sd,
			&m_pSSWrapLinear)))
		{
			return;
		}

#pragma endregion ID3D11SamplerState
#pragma region BLEND
		D3D11_BLEND_DESC ds;
		ZeroMemory(&ds, sizeof(D3D11_BLEND_DESC));
		ds.AlphaToCoverageEnable = TRUE;
		ds.IndependentBlendEnable = TRUE;
		ds.RenderTarget[0].BlendEnable = TRUE;
		ds.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		ds.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		ds.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

		ds.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		ds.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		ds.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

		ds.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


		if (FAILED(hr = pd3dDevice->CreateBlendState(&ds,
			&m_pAlphaBlend)))
		{
			return;
		}
		ds.RenderTarget[0].BlendEnable = FALSE;
		if (FAILED(hr = pd3dDevice->CreateBlendState(&ds,
			&m_pAlphaBlendDisable)))
		{
			return;
		}
#pragma endregion			BLEND
#pragma region rasterizer
		//레스터라이즈 상태 객체 생성
		D3D11_RASTERIZER_DESC rsDesc;
		ZeroMemory(&rsDesc, sizeof(rsDesc));
		rsDesc.DepthClipEnable = TRUE;
		rsDesc.FillMode = D3D11_FILL_WIREFRAME;
		rsDesc.CullMode = D3D11_CULL_NONE;
		if (FAILED(hr =
			pd3dDevice->CreateRasterizerState(&rsDesc, &m_pRSWireFrame)))
		{
			return;
		}
		rsDesc.FillMode = D3D11_FILL_SOLID;
		if (FAILED(hr =
			pd3dDevice->CreateRasterizerState(&rsDesc, &m_pRSSolidFrame)))
		{
			return;
		}
#pragma endregion		RESTERIZER
	}
	void SDXState::Release()
	{
		if (m_pSSWrapLinear)
			m_pSSWrapLinear->Release();
		if (m_pAlphaBlend)
			m_pAlphaBlend->Release();

		if (m_pAlphaBlendDisable)
			m_pAlphaBlendDisable->Release();

		if (m_pRSWireFrame)
			m_pRSWireFrame->Release();
		if (m_pRSSolidFrame)
			m_pRSSolidFrame->Release();
		m_pRSSolidFrame = 0;
		m_pRSWireFrame = 0;
		m_pAlphaBlend = 0;
		m_pSSWrapLinear = 0;
	}

	SDXState g_DXState;

	SDXState::SDXState()
	{
	}


	SDXState::~SDXState()
	{
		Release();
	}

}

