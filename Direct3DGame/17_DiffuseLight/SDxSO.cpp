#include "SDxSO.h"

namespace DXGame
{
	HRESULT SDxSO::CreateStreams(ID3D11Device* pd3dDevice,
		DWORD dwSize, DWORD dwCount)
	{
		HRESULT hr = S_OK;

		D3D11_BUFFER_DESC vbdesc =
		{
			dwSize*dwCount,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT,
			0,
			0
		};
		HRESULT_FAILED_RETURN(pd3dDevice->CreateBuffer(&vbdesc, NULL, m_pDrawFrom.GetAddressOf()));
		HRESULT_FAILED_RETURN(pd3dDevice->CreateBuffer(&vbdesc, NULL, m_pStreamTo.GetAddressOf()));
		return hr;
	}
	bool SDxSO::BeginRTS(ID3D11DeviceContext*    pImmediateContext)
	{
		if (m_pStreamTo == NULL) return false;
		UINT StrideOffset[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		ID3D11Buffer* pBuffers[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		pImmediateContext->IASetVertexBuffers(0, 16, pBuffers, StrideOffset, StrideOffset);
		//ClearD3D11DeviceContext(pImmediateContext);

		UINT offset = 0;
		ID3D11Buffer* pVB[1];
		pVB[0] = NULL;
		pImmediateContext->SOSetTargets(1, pVB, &offset);


		pVB[0] = { m_pStreamTo.Get() };
		pImmediateContext->SOSetTargets(1, pVB, &offset);
		return true;
	}
	void SDxSO::EndRTS(ID3D11DeviceContext*    pImmediateContext)
	{
		UINT offset = 0;
		ID3D11Buffer* pVB[1];
		pVB[0] = NULL;
		pImmediateContext->SOSetTargets(1, pVB, &offset);

		m_pStreamTo.Swap(m_pDrawFrom);
	}

	SDxSO::SDxSO(void)
	{
		m_pDrawFrom = nullptr;
		m_pStreamTo = nullptr;
	}

	SDxSO::~SDxSO(void)
	{
	}
}