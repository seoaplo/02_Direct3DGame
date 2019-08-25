#include "SDXObject.h"

bool SDXObject::Init()
{
	return true;
}


bool SDXObject::Frame()
{
	float fTime = I_Timer.GetSPF();
	D3D11_MAPPED_SUBRESOURCE msr;
	if (SUCCEEDED(m_pDeviceContext->Map(m_dxObj.m_pConstantBuffer.Get(), 0,
		D3D11_MAP_WRITE_DISCARD, 0, &msr)))
	{
		VSCB* data = (VSCB*)msr.pData;
		data->color[0] = cosf(fTime);
		data->color[1] = sinf(fTime);
		data->color[2] = 1 - cosf(fTime);
		data->color[3] = 1.0f;
		data->etc[0] = fTime;
		m_pDeviceContext->Unmap(m_dxObj.m_pConstantBuffer.Get(), 0);
	}

	return true;
}
bool SDXObject::Render(ID3D11DeviceContext*	pContext, UINT  iSize, UINT iCount)
{
	m_dxObj.Render(pContext, iSize, iCount);
	return true;
}	
bool SDXObject::Release()
{
	return true;
}
bool SDXObject::SetBlendState()
{
	HRESULT hr;
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


	if (FAILED(hr = m_pDevice->CreateBlendState(&ds,
		&m_pAlphaBlend)))
	{
		return false;
	}
	return true;
}

//==============================================================================================
// Create Buffer (Vertex, index, Constant)
//==============================================================================================
bool SDXObject::CreateVertexBuffer(void* vertices, int iNumCount, int iSize)
{
	m_dxObj.m_iNumVertex = iNumCount;
	m_dxObj.m_pVertexBuffer.Attach(
		DXGame::CreateVertexBuffer(m_pDevice, vertices, iNumCount, iSize));

	if (m_dxObj.m_pVertexBuffer.Get() == nullptr) return false;

	else return true;
}
bool SDXObject::CreateIndexBuffer(void* pData, int iNumCount, int iSize)
{
	m_dxObj.m_iNumIndex = iNumCount;
	m_dxObj.m_pIndexBuffer.Attach(
		DXGame::CreateIndexBuffer(m_pDevice, pData, iNumCount, iSize));
	
	if (m_dxObj.m_pIndexBuffer.Get() == nullptr) return false;
	
	else return true;
}
bool SDXObject::CreateConstantBuffer(void* pData, int iNumCount, int iSize, bool bDynamic)
{
	m_dxObj.m_pConstantBuffer.Attach(
		DXGame::CreateConstantBuffer(m_pDevice, pData, iNumCount, iSize, bDynamic));

	if (m_dxObj.m_pConstantBuffer.Get() == nullptr) return false;

	else return true;
}
//==============================================================================================
// Load  (Shader, Input layout, Shader Resource View)
//==============================================================================================
bool SDXObject::LoadShaderAndInputlayout(const char* pVertexShader, const char* pPixelShader)
{

	HRESULT hr;
	m_dxObj.m_pVertexShader.Attach(
		DXGame::LoadVertexShaderFile(m_pDevice, L"../../shader/SDXObjectTest/SDXObjectTestVS.hlsl", m_dxObj.m_pVSBlob.GetAddressOf(), pVertexShader)
	);
	m_dxObj.m_pPixelShader.Attach(
		DXGame::LoadPixelShaderFile(m_pDevice, L"../../shader/SDXObjectTest/SDXObjectTestPS.hlsl")
	);

	// input layout
	// 정점 버퍼의 데이터를 정점 쉐이더의 인자값으로 설정
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};

	int iNumElement = sizeof(layout) / sizeof(layout[0]);
	m_dxObj.m_pInputLayout.Attach(
		DXGame::CreateInputLayout(	m_pDevice,
									m_dxObj.m_pVSBlob->GetBufferSize(),
									m_dxObj.m_pVSBlob->GetBufferPointer(),
									layout, iNumElement)
	);

	return true;
}
bool SDXObject::LoadSRV(T_STR name, int iIndex)
{
	return true;
}

SDXObject::SDXObject()
{

}


SDXObject::~SDXObject()
{

}
