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

	D3DX11CompileFromFile(L"../../data/shader/IndexTestVS.shader", nullptr, nullptr, "VS", "vs_5_0", dwShaderFlag,
		NULL, nullptr, &VertexBlob, nullptr, &hr);

	HRESULT_FAILED_RETURN(hr);

	ID3DBlob* PixelBlob = nullptr; 

	D3DX11CompileFromFile(L"../../data/shader/IndexTestPS.shader", nullptr, nullptr, "PS", "ps_5_0", dwShaderFlag,
		NULL, nullptr, &PixelBlob, nullptr, &hr);

	HRESULT_FAILED_RETURN(hr);

	

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

	return true;
}
bool Sample::Release()
{

	return true;
}

Sample::Sample()
{
}


Sample::~Sample()
{
}
