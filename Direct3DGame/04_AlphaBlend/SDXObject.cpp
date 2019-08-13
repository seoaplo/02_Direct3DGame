#include "SDXObject.h"

HRESULT SDXObject::LoadShaderAndInputLayout()
{
	HRESULT hr = S_OK;
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	ID3DBlob* pVSBuffer = nullptr;

#if defined( _DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pBufferErrors = nullptr;
	hr = D3DX11CompileFromFile(L"../../shader/AlphaBlendTest/AlphaBlendTestVS.shader", nullptr, nullptr, "VS", "vs_5_0",
		dwShaderFlags, NULL, nullptr, &pVSBuffer, &pBufferErrors, nullptr);
	if (FAILED(hr))
	{
		TCHAR pMessage[500];
		mbstowcs(pMessage, (CHAR*)pBufferErrors->GetBufferPointer(), 500);
		MessageBox(NULL, pMessage, _T("Error"), MB_OK);
		return hr;
	}

	hr = m_pDevice->CreateVertexShader((DWORD*)pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(),
		nullptr, &m_pVertexShader);
	HRESULT_FAILED_RETURN(hr);

	ID3DBlob* pPixelBuffer = nullptr;
	hr = D3DX11CompileFromFile(L"../../shader/AlphaBlendTest/AlphaBlendTestPS.shader", nullptr, nullptr, "PS", "ps_5_0", dwShaderFlags, NULL,
		nullptr, &pPixelBuffer, &pBufferErrors, nullptr);

	if (FAILED(hr))
	{
		TCHAR pMessage[500];
		mbstowcs(pMessage, (CHAR*)pBufferErrors->GetBufferPointer(), 500);
		MessageBox(NULL, pMessage, _T("Error"), MB_OK);
		return hr;
	}

	hr = m_pDevice->CreatePixelShader((DWORD*)pPixelBuffer->GetBufferPointer(), pPixelBuffer->GetBufferSize(),
		nullptr, &m_pPixelShader);
	HRESULT_FAILED_RETURN(hr);

	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	hr = m_pDevice->CreateInputLayout(layout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(),
		&m_pVertexLayout);
	SAFE_RELEASE(pVSBuffer);
	SAFE_RELEASE(pPixelBuffer);
	return hr;
}

HRESULT SDXObject::CreateVertexBuffer()
{
	HRESULT hr = S_OK;

	//Create vertex buffer
	PCT_Vertex vertices[] =
	{
		{D3DXVECTOR4(-0.5f, 0.5f, 1.0f, 1.0f),	D3DXVECTOR4(1.0f, 1.0, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f)},
		{D3DXVECTOR4(0.5f, 0.5f, 1.0f, 1.0f), D3DXVECTOR4(1.0f, 1.0, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f)},
		{D3DXVECTOR4(0.5f, -0.5f, 1.0f, 1.0f), D3DXVECTOR4(1.0f, 1.0, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f)},
		{D3DXVECTOR4(-0.5f, -0.5f, 1.0f, 1.0f), D3DXVECTOR4(1.0f, 1.0, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f)},
	};

	UINT numVertices = sizeof(vertices) / sizeof(vertices[0]);

	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = numVertices * sizeof(PCT_Vertex);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	CD3D11_BUFFER_DESC cbc(sizeof(vertices) * 4, D3D11_BIND_VERTEX_BUFFER);

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	hr = m_pDevice->CreateBuffer(&cbc, &InitData, &m_pVertexBuffer);
	return hr;
}
HRESULT SDXObject::CreateIndexBuffer()
{
	HRESULT hr = S_OK;

	DWORD indices[] =
	{
		0, 1, 2,
		0, 2, 3,
	};

	UINT iNumIndex = sizeof(indices) / sizeof(indices[0]);

	D3D11_BUFFER_DESC ibdc;
	ibdc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibdc.ByteWidth = iNumIndex * sizeof(DWORD);
	ibdc.Usage = D3D11_USAGE_DEFAULT;
	ibdc.CPUAccessFlags = 0;
	ibdc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA ibInitData;
	ZeroMemory(&ibInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	ibInitData.pSysMem = indices;
	hr = m_pDevice->CreateBuffer(&ibdc, &ibInitData, &m_pIndexBurffer);
	return hr;
}
HRESULT SDXObject::CreateConstantBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VSCB);
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	hr = m_pDevice->CreateBuffer(&bd, NULL, &m_pConstantBuffer);
	return hr;
}


HRESULT SDXObject::CreateTexture(const TCHAR* wRouteString)
{
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(m_pDevice, wRouteString, nullptr, nullptr, &m_pTextureSRV, nullptr);
	return hr;
}

HRESULT SDXObject::AlphaBlendSet()
{
	HRESULT hr = S_OK;
	ZeroMemory(&m_BlendDesc, sizeof(D3D11_BLEND_DESC));

	m_BlendDesc.AlphaToCoverageEnable = TRUE;
	// 픽셀을 렌더링 대상으로 설정할 때 알파 투 커버리지를 멀티 샘플링 기술로 사용할지 여부를 지정합니다.
	m_BlendDesc.IndependentBlendEnable = TRUE;
	m_BlendDesc.RenderTarget[0].BlendEnable = TRUE;				// 블렌드 사용 여부

	m_BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;	// 컬러 블렌드 계산 방식 
	m_BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;		// 소스 컬러 설정
	m_BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;	// 타겟 컬러 설정

	//m_BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_SRC_ALPHA;		// 소스 컬러 설정
	//m_BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_ALPHA;	// 타겟 컬러 설정

	m_BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;		// 알파 블렌드 계산 방식
	m_BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;		// 소스 알파 설정
	m_BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;		// 타겟 알파 설정

	//m_BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;		// 소스 알파 설정
	//m_BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;		// 타겟 알파 설정

	m_BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // 처리 하는 방법

	//m_BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED;
	//m_BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_GREEN;
	//m_BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_BLUE;
	//m_BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALPHA ;



	hr = m_pDevice->CreateBlendState(&m_BlendDesc, &m_pAlphaBlend);

	return hr;
}


bool SDXObject::Init()
{
	HRESULT_FAILED_RETURN(CreateVertexBuffer());
	HRESULT_FAILED_RETURN(CreateIndexBuffer());
	HRESULT_FAILED_RETURN(LoadShaderAndInputLayout());
	HRESULT_FAILED_RETURN(CreateConstantBuffer());
	HRESULT_FAILED_RETURN(CreateTexture(L"../../data/Image/Attacker3.png"));
	HRESULT_FAILED_RETURN(AlphaBlendSet());

	m_PrimType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	return true;
}


bool SDXObject::Frame()
{
	float fTime = I_Timer.GetElapsedTime();
	float fBoundedTime = cosf(fTime) * 0.5f + 0.5f;

	float fcos = cosf(fTime);
	float fsin = sinf(fTime);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pDeviceContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	VSCB* pConstData = (VSCB*)MappedResource.pData;
	pConstData->mat = D3DXMATRIX(fcos, -fsin, 0.0f, 0.0f,
		fsin, fcos, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	pConstData->fTime = fBoundedTime;
	m_pDeviceContext->Unmap(m_pConstantBuffer, 0);
	return true;
}
bool SDXObject::Render()
{
	m_pDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader, NULL, 0);
	m_pDeviceContext->GSSetShader(NULL, NULL, 0);
	m_pDeviceContext->IASetInputLayout(m_pVertexLayout);

	UINT stride = sizeof(PCT_Vertex);
	UINT offset = 0;
	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pDeviceContext->IASetIndexBuffer(m_pIndexBurffer, DXGI_FORMAT_R32_UINT, 0);
	m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTextureSRV);
	m_pDeviceContext->OMSetBlendState(m_pAlphaBlend, nullptr, -1);
	m_pDeviceContext->IASetPrimitiveTopology(m_PrimType);
	m_pDeviceContext->DrawIndexed(6, 0, 0);
	return true;
}

bool SDXObject::Release()
{
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBurffer);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pTextureSRV);
	SAFE_RELEASE(m_pAlphaBlend);

	return true;
}

SDXObject::SDXObject()
{
	m_pDevice = nullptr;
	m_pDeviceContext = nullptr;

	m_pVertexLayout= nullptr;
	m_pVertexBuffer= nullptr;
	m_pIndexBurffer= nullptr;
	m_pConstantBuffer = nullptr;
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pTextureSRV = nullptr;
	m_pAlphaBlend = nullptr;
}


SDXObject::~SDXObject()
{
	m_pDevice = nullptr;
	m_pDeviceContext = nullptr;
	m_pVertexLayout = nullptr;
	m_pVertexBuffer = nullptr;
	m_pIndexBurffer = nullptr;
	m_pConstantBuffer = nullptr;
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pTextureSRV = nullptr;
	m_pAlphaBlend = nullptr;
}
