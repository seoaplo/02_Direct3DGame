#include "Sample.h"

//========================================================================
// 정점 버퍼 생성
//========================================================================
HRESULT Sample::CreateVertexBuffer()
{

	Vertex vertice[] = 
	{
		{ -0.5, 0.5, 0.0 },
		{ 0.5, 0.5, 0.0 },
		{ 0.5, -0.5, 0.0 },
		{ -0.5, -0.5, 0.0 },
	};

	int VerticeSize = sizeof(vertice) / sizeof(Vertex);

	D3D11_BUFFER_DESC Desc;
	D3D11_SUBRESOURCE_DATA InitialData;

	ZeroMemory(&Desc, sizeof(Desc));
	Desc.ByteWidth = sizeof(Vertex) * VerticeSize;
	Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Desc.Usage = D3D11_USAGE_DEFAULT;

	ZeroMemory(&InitialData, sizeof(InitialData));
	InitialData.pSysMem = vertice;

	HRESULT_FAILED_RETURN(GetDevice()->CreateBuffer(&Desc, &InitialData, &m_pVertexBuffer));
	
	DWORD Indexbuffer[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	int IndexBufferSize = sizeof(Indexbuffer) / sizeof(DWORD);

	ZeroMemory(&Desc, sizeof(Desc));
	Desc.ByteWidth = sizeof(DWORD) * IndexBufferSize;
	Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	Desc.Usage = D3D11_USAGE_DEFAULT;

	ZeroMemory(&InitialData, sizeof(InitialData));
	InitialData.pSysMem = Indexbuffer;

	HRESULT_FAILED_RETURN(GetDevice()->CreateBuffer(&Desc, &InitialData, &m_pIndexBuffer));

	return true;
}
//========================================================================
// 정점 및 픽쉘 쉐이더 로딩 및 생성
//========================================================================
HRESULT Sample::LoadShaderAndInputLayout()
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlag = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG) || defined(_DEBUG)
	dwShaderFlag |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* VertexBlob = nullptr;

	D3DX11CompileFromFile(L"../../shader/IndexTestVS.shader", nullptr, nullptr, "VS", "vs_5_0", dwShaderFlag,
		NULL, nullptr, &VertexBlob, nullptr, &hr);

	GetDevice()->CreateVertexShader(VertexBlob->GetBufferPointer(), VertexBlob->GetBufferSize(),
									nullptr, &m_pVS);
	HRESULT_FAILED_RETURN(hr);

	ID3DBlob* PixelBlob = nullptr; 

	D3DX11CompileFromFile(L"../../shader/IndexTestPS.shader", nullptr, nullptr, "PS", "ps_5_0", dwShaderFlag,
		NULL, nullptr, &PixelBlob, nullptr, &hr);

	GetDevice()->CreatePixelShader(PixelBlob->GetBufferPointer(), PixelBlob->GetBufferSize(),
		nullptr, &m_pPS);

	HRESULT_FAILED_RETURN(hr);

	const D3D11_INPUT_ELEMENT_DESC Layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	GetDevice()->CreateInputLayout(Layout, 1, VertexBlob->GetBufferPointer(), VertexBlob->GetBufferSize(),
									&m_pVertexLayout);

	SAFE_RELEASE(VertexBlob);
	SAFE_RELEASE(PixelBlob);

	return true;
}
//========================================================================
// 재정의 함수들
//========================================================================
bool Sample::Init()
{
	if (CreateVertexBuffer() == false) return false;
	if (LoadShaderAndInputLayout() == false) return false;

	return true;
}
bool Sample::Frame()
{
	return true;
}
bool Sample::Render()
{
	m_pImmediateContext->VSSetShader(m_pVS, nullptr, 0);
	m_pImmediateContext->PSSetShader(m_pPS, nullptr, 0);
	m_pImmediateContext->IASetInputLayout(m_pVertexLayout);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	m_pImmediateContext->DrawIndexed(3, 0, 0);
	return true;
}
bool Sample::Release()
{
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pPS);
	return true;
}

Sample::Sample()
{
	m_pIndexBuffer = nullptr;
	m_pVertexBuffer = nullptr;
	m_pVertexLayout = nullptr;
	m_pVS = nullptr;
	m_pPS = nullptr;
}


Sample::~Sample()
{
}

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