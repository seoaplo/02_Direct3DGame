#pragma once
#include "SDxBasic.h"
#include "SUtils.h"

namespace DXGame
{
	class SDxSO
	{
	public:
		Microsoft::WRL::ComPtr<ID3D11Buffer>	m_pDrawFrom;
		Microsoft::WRL::ComPtr<ID3D11Buffer>	m_pStreamTo;
	public:
		bool BeginRTS(ID3D11DeviceContext*    pImmediateContext);
		void EndRTS(ID3D11DeviceContext*    pImmediateContext);

		HRESULT		CreateStreams(ID3D11Device* pd3dDevice,
			DWORD dwSize,
			DWORD dwCount);
	public:
		SDxSO();
		~SDxSO();
	};


}
