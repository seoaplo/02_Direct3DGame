#pragma once
#include "SDxBasic.h"
namespace DXGame
{
	class SDXState
	{
	public:
		//ID3D11BlendState
		static ID3D11BlendState*	m_pAlphaBlend;
		static ID3D11BlendState*	m_pAlphaBlendDisable;

		//ID3D11RasterizerState
		static ID3D11RasterizerState*  m_pRSWireFrame;
		static ID3D11RasterizerState*  m_pRSSolidFrame;

		//ID3D11SamplerState*  
		static ID3D11SamplerState*   m_pSSWrapLinear;

		static void SetState(ID3D11Device* pd3dDevice);
		static void Release();

	static void ApplySS(ID3D11DeviceContext* pContext,
		ID3D11SamplerState* pState, UINT iSlot = 0, UINT iArray = 1)
	{
		pContext->PSSetSamplers(iSlot, iArray, &pState);
	};

	static void ApplyRS(ID3D11DeviceContext* pContext,
		ID3D11RasterizerState* pState)
	{
		pContext->RSSetState(pState);
	};
	static void ApplyBS(ID3D11DeviceContext* pContext,
		ID3D11BlendState*	   pState,
		const FLOAT fBlendFactor[] = 0,
		UINT iMask = 0xffffffff)
	{
		pContext->OMSetBlendState(pState, fBlendFactor, iMask);
	};
	public:
		SDXState();
		~SDXState();
	};

}