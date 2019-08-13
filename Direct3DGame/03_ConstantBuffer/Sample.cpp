#include "Sample.h"
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Sample g_Sample;
	RECT rc{ 0, 0, 1024, 768 };
	if (!g_Sample.InitWindow(hInstance, nCmdShow, const_cast<TCHAR*>(L"TextureTest"), rc, false))
	{
		MessageBox(0, _T("Sample  초기화 실패"), _T("Fatal error"), MB_OK);
		return 0;
	}
	ShowCursor(TRUE);
	g_Sample.Run();
	return 1;
}

HRESULT Sample::LoadShaderAndInputLayout()
{
	HRESULT hr = S_OK;
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	ID3DBlob* pVSBuffer = nullptr;

#if defined( _DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pBufferErrors = nullptr;
	hr = D3DX11CompileFromFile(L"../../shader/ConstantBufferTest/ConstantTestVS.shader", nullptr, nullptr, "VS", "vs_5_0",
		dwShaderFlags, NULL, nullptr, &pVSBuffer, &pBufferErrors, nullptr);
	if (FAILED(hr))
	{
		TCHAR pMessage[500];
		mbstowcs(pMessage, (CHAR*)pBufferErrors->GetBufferPointer(), 500);
		MessageBox(NULL, pMessage, _T("Error"), MB_OK);
		return hr;
	}

	hr = GetDevice()->CreateVertexShader((DWORD*)pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(),
		nullptr, &m_pVertexShader);
	HRESULT_FAILED_RETURN(hr);

	ID3DBlob* pPixelBuffer = nullptr;
	hr = D3DX11CompileFromFile(L"../../shader/ConstantBufferTest/ConstantTestPS.shader", nullptr, nullptr, "PS", "ps_5_0", dwShaderFlags, NULL,
		nullptr, &pPixelBuffer, &pBufferErrors, nullptr);

	if (FAILED(hr))
	{
		TCHAR pMessage[500];
		mbstowcs(pMessage, (CHAR*)pBufferErrors->GetBufferPointer(), 500);
		MessageBox(NULL, pMessage, _T("Error"), MB_OK);
		return hr;
	}

	hr = GetDevice()->CreatePixelShader((DWORD*)pPixelBuffer->GetBufferPointer(), pPixelBuffer->GetBufferSize(),
		nullptr, &m_pPixelShader);
	HRESULT_FAILED_RETURN(hr);

	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	hr = GetDevice()->CreateInputLayout(layout, 1, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(),
		&m_pVertexLayout);
	SAFE_RELEASE(pVSBuffer);
	SAFE_RELEASE(pPixelBuffer);
	return hr;
}

HRESULT Sample::CreateVertexBuffer()
{
	HRESULT hr = S_OK;

	//Create vertex buffer
	SimpleVertex vertices[] =
	{
		-0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
	};

	UINT numVertices = sizeof(vertices) / sizeof(vertices[0]);

	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = numVertices * sizeof(SimpleVertex);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	CD3D11_BUFFER_DESC cbc(sizeof(SimpleVertex) * 4, D3D11_BIND_VERTEX_BUFFER);

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	hr = GetDevice()->CreateBuffer(&cbc, &InitData, &m_pVertexBuffer);
	return hr;
}
HRESULT Sample::CreateIndexBuffer()
{
	HRESULT hr = S_OK;

	WORD indices[] =
	{
		0, 1, 2,
		0, 2, 3,
	};

	UINT iNumIndex = sizeof(indices) / sizeof(indices[0]);

	D3D11_BUFFER_DESC ibdc;
	ibdc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibdc.ByteWidth = iNumIndex * sizeof(WORD);
	ibdc.Usage = D3D11_USAGE_DEFAULT;
	ibdc.CPUAccessFlags = 0;
	ibdc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA ibInitData;
	ZeroMemory(&ibInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	ibInitData.pSysMem = indices;
	hr = GetDevice()->CreateBuffer(&ibdc, &ibInitData, &m_pIndexBurffer);
	return hr;
}
HRESULT Sample::CreateConstantBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VSCB);
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	hr = GetDevice()->CreateBuffer(&bd, NULL, &m_pConstantBuffer);
	return hr;
}
HRESULT Sample::CreateTriangle(D3D11_PRIMITIVE_TOPOLOGY iTopology)
{
	HRESULT hr = S_OK;

	HRESULT_FAILED_RETURN(CreateVertexBuffer());
	HRESULT_FAILED_RETURN(CreateIndexBuffer());
	HRESULT_FAILED_RETURN(LoadShaderAndInputLayout());

	m_pImmediateContext->IASetPrimitiveTopology(iTopology);

	return hr;
}

bool Sample::Init()
{
	HRESULT_FAILED_RETURN(CreateTriangle());
	HRESULT_FAILED_RETURN(CreateConstantBuffer());
	return true;
}

bool Sample::Frame()
{
	float fTime = I_Timer.GetElapsedTime();
	float fBoundedTime = cosf(fTime) * 0.5f + 0.5f;

	float fcos = cosf(fTime);
	float fsin = sinf(fTime);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pImmediateContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	VSCB* pConstData = (VSCB*)MappedResource.pData;
	pConstData->mat = D3DXMATRIX(fcos, -fsin, 1.0f, 0.0f,
								fsin, fcos, 1.0f, 0.0f,
								1.0f, 1.0f, 1.0f, 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f);
	pConstData->fTime = fBoundedTime;
	m_pImmediateContext->Unmap(m_pConstantBuffer, 0);
	return true;
}
bool Sample::Render()
{
	m_pImmediateContext->VSSetShader(m_pVertexShader, NULL, 0);
	m_pImmediateContext->PSSetShader(m_pPixelShader, NULL, 0);
	m_pImmediateContext->GSSetShader(NULL, NULL, 0);
	m_pImmediateContext->IASetInputLayout(m_pVertexLayout);

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pImmediateContext->IASetIndexBuffer(m_pIndexBurffer, DXGI_FORMAT_R16_UINT, 0);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->IASetPrimitiveTopology(m_PrimType);
	m_pImmediateContext->DrawIndexed(6, 0, 0);
	return true;
}

bool Sample::Release()
{
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBurffer);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);

	return true;
}

Sample::Sample()
{
	m_pVertexLayout = NULL;
	m_pVertexBuffer = NULL;
	m_pIndexBurffer = NULL;
	m_pConstantBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_PrimType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}


Sample::~Sample()
{
}
