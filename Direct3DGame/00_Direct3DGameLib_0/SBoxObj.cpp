#include "SBoxObj.h"


bool SBoxObj::Init()
{
	D3DXMatrixIdentity(&m_matWorld);
	m_matWorld._11 = 5.0f;
	m_matWorld._22 = 5.0f;
	m_matWorld._33 = 5.0f;
	m_matWorld._42 = 40.0f;
	return true;
}
HRESULT SBoxObj::LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pShaderFile)
{
	HRESULT hr = S_OK;
	SModel::LoadShaderFile(pDevice, pShaderFile);

	m_pVSPassThroughMain.Attach(DXGame::LoadVertexShaderFile(pDevice, pShaderFile, m_pVSBlob.GetAddressOf(), "VS_SO"));
	m_pGS.Attach(DXGame::LoadGeometryShaderFile(pDevice, pShaderFile, m_pGSBlob.GetAddressOf(), "GS_SO"));

	D3D11_SO_DECLARATION_ENTRY pDecl[] =
	{
		{ 0, "POSITION", 0, 0, 3, 0 },
		{ 0, "NORMAL", 0, 0, 3, 0 },
		{ 0, "COLOR", 0, 0, 4, 0 },
		{ 0, "TEXCOORD", 0, 0, 2, 0 },
	};

	int size = ARRAYSIZE(pDecl);
	UINT strides[] = { sizeof(PNCT_VERTEX) };

	hr = pDevice->CreateGeometryShaderWithStreamOutput(
		m_pGSBlob.Get()->GetBufferPointer(),
		m_pGSBlob.Get()->GetBufferSize(),
		pDecl,
		size,
		strides,
		1,
		D3D11_SO_NO_RASTERIZED_STREAM,// 11.01 이하이면 0을 사용한다. dss=disable
		NULL,
		m_pGSStreamOutput.GetAddressOf());

	return hr;
}

bool SBoxObj::SetSO(ID3D11Device* pDevice, ID3D11DeviceContext*  pContext)
{
	DXGame::SDxState::SetDepthStencilState(pContext, DebugDSSDepthDisable);
	m_RenderSO.CreateStreams(pDevice, sizeof(PNCT_VERTEX), 100000);

	m_RenderSO.BeginRTS(pContext);
	RenderMesh(pContext, true);
	m_RenderSO.EndRTS(pContext);

	for (int iStream = 0; iStream < m_iNumSteamOut; iStream++)
	{
		m_RenderSO.BeginRTS(pContext);
		RenderMesh(pContext, false);
		m_RenderSO.EndRTS(pContext);
	}
	return true;
}
bool SBoxObj::RenderMesh(ID3D11DeviceContext*  pContext, bool bFirstRender)
{
	PreRender(pContext);
	pContext->PSSetShader(nullptr, nullptr, 0);
	pContext->VSSetShader(m_pVSPassThroughMain.Get(), nullptr, 0);
	pContext->GSSetShader(m_pGSStreamOutput.Get(), nullptr, 0);

	if (bFirstRender)
	{
		SModel::PostRender(pContext);
	}
	else
	{
		ID3D11Buffer* pVB[1] = { m_RenderSO.m_pDrawFrom.Get() };
		UINT Strides[1] = { sizeof(PNCT_VERTEX) };
		UINT Offsets[1] = { 0 };
		pContext->IASetVertexBuffers(0, 1, pVB, Strides, Offsets);
		pContext->DrawAuto();
	}
	return true;
}
bool SBoxObj::PostRender(ID3D11DeviceContext*  pContext)
{
	pContext->UpdateSubresource(m_dxobj.g_pConstantBuffer.Get(), 0, NULL, &m_cbData, 0, 0);
	ID3D11Buffer* pVB[1] = { m_RenderSO.m_pDrawFrom.Get() };
	UINT Strides[1] = { sizeof(PNCT_VERTEX) };
	UINT Offsets[1] = { 0 };
	pContext->IASetVertexBuffers(0, 1, pVB, Strides, Offsets);
	pContext->GSSetShader(NULL, NULL, 0);
	pContext->DrawAuto();
	return true;
}
bool SBoxObj::Render(ID3D11DeviceContext*  pContext)
{
	PreRender(pContext);
	PostRender(pContext);
	return true;
}

SBoxObj::SBoxObj()
{
	m_iNumSteamOut = 2;
}


SBoxObj::~SBoxObj()
{
}
