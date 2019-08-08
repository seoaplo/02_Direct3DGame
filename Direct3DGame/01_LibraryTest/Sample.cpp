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

HRESULT Sample::CreateVertexBuffer()
{
	HRESULT hr = S_OK;
	P3_VERTEX verices[] =
	{
		0.0f, 0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
	};

	//========================================================================
	// D3D11_BUFFER_DESC
	//========================================================================
	// UINT ByteWidth					생성되는 버퍼 크기(Byte)
	// D3D11_USAGE Usage				생성되는 버퍼의 읽기 및 쓰기 방법을 지정한다. 버퍼의 갱신되는 정도에 따라 이 값이 지정된다.
	//									일반적으로 D3D11_USAGE_DEFAULT를 사용한다. 이외의 사용할 수 있는 모든 값에 대해서는
	//									"D3D11_USAGE" 열거형에 지정되어 있다.
	// UINT BindFlags					버퍼가 파이프라인에 바인딩하는 방법을 지정하며 여러 가지 방법으로 버퍼에 엑세스 할 수 있도록
	//									D3D11_BIND_FLAG 열거형 값을 논리합으로 조립할 수 있다.
	// UINT CPUAccessFlags				CPU가 생성되는 버퍼에 접근하는 유형을 지정한다. CPU 액세스가 필요 없는 경우에는 0을 이용한다
	//									D3D11_CPU_ACCESS_FLAG 열거형 다수를 논리합으로 조립할 수 있다.
	// UINT MiscFlags					추가적인 옵션 플래그로 "D3D11_RESOURCE_MISC_FLAG" 열거형이 사용된다.
	// UINT StructureByteStride			구조화된 버퍼를 사용할 경우에 구조체 버퍼의 크기
	//========================================================================

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.ByteWidth = sizeof(P3_VERTEX) * 3;

	/*
	========================================================================
		D3D11_SUBRESOURCE_DATA
	========================================================================
		const void *pSysMem			초기화 데이터에 대한 포인터
		UINT SysMemPitch			텍스쳐 리소스의 한줄 크기의(바이트 단위)이다. 이 값은 2D와 3D 텍스쳐 데이터에만 사용되며
									다른 종류의 리소스에서는 의미를 갖지 않는다.
		UINT SysMemSlicePitch		3차원 텍스쳐의 깊이의 간격(바이트 단위) 크기이다. 이 값은 3D 텍스쳐 데이터에만 사용되며
									다른 종류의 리소스에서는 의미를 갖지 않는다.
	========================================================================
	*/
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = verices;
	return GetDevice()->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);
}

HRESULT Sample::LoadShaderAndInputLayout()
{
	HRESULT hr = S_OK;
	// Create Effect
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	// WCHAR str[MAX_PATH]

	/*
	========================================================================
		ID3DBlob
	========================================================================
		LPVOID STDMETHODCALLTYPE GetBufferPointer	데이터에 대한 포인터를 얻는다
		SIZE_T STDMETHODCALLTYPE GetBufferSize		크기를 얻는다
	========================================================================
	*/
	ID3DBlob* pVSBuf = nullptr;

#if defined(_DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	/*
	========================================================================
		D3DX11CompileFromFile
	========================================================================
		LPCWSTR pSrcFile						컴파일 하고자 하는 쉐이더의 파일 명
		CONST D3D10_SHADER_MACRO* pDefines		쉐이더 코드 안에서 사용할 매크로 배열의 주소. 사용하지 않을 경우 NULL
		LPD3D10INCLUDE pInclude					ID3DInclude인터페이스를 지정하며 사용하지 않을 경우는 NULL로 한다.
												인터페이스를 사용할 때 쉐이더 코드 안에 #Include가 포함되어 있으면 오류가 발생한다.
		LPCSTR pFunctionName					쉐이더 진입점 함수 이름
		LPCSTR pProfile							컴파일할 쉐이더 모듈을 지정하는 문자열, 쉐이더 모델 2 쉐이더 모델 3
												또는 쉐이더 모델 4의 모든 프로필을 사용할 수 있다.
		UINT Flags1								쉐이더 컴파일 플래그
		UINT Flags2								이펙트 옵션 컴파일 플래그
		ID3DX11ThreadPump* pPump				ID3DX11ThreadPump 인터페이스를 지정하며 비동기적인 리소스의 로딩에 사용한다.
												사용하지 않을 경우 NULL
		ID3D10Blob** ppShader					컴파일 된 쉐이더와 포함된 디버깅 정보와 심볼 테이블 정보를 저장하는 ID3DBlob
												인터페이스에 대한 포인터이다.
		ID3D10Blob** ppErrorMsgs				컴파일 할 때 오류 및 경고 목록을 저장하는 ID3D10Blob 인터페이스에 대한 포인터.
												이러한 오류와 경고는 디버거에서 디버깅 출력과 동일하다.
		HRESULT* pHResult						반환값에 대한 주소. 필요하지 않은 경우에는 NULL로 지정. pPump가 NULL이 아닌 경우
												비동기적으로 실행이 완료될 때까지 pHResult으로 반환 값을 리턴 받기 위해 사용하는
												포인터는 변경되지 않도록 해야 한다.
	========================================================================
	*/

	hr = D3DX11CompileFromFile(L"HLSLwithoutFX10.vsh", NULL, NULL, "VS", "vs_5_0", dwShaderFlags,
		NULL, NULL, &pVSBuf, NULL, NULL);
	HRESULT_FAILED_RETURN_BOOL(hr);
	/*
	========================================================================
		CreateVertexShader		컴파일된 쉐이더 코드를 사용하여 해당 정점 쉐이더를 생성
	========================================================================
	const void *pShaderBytecode				컴파일 된 쉐이더에 대한 포인터
	SIZE_T BytecodeLength					컴파일 된 쉐이더 코드의 길이
	ID3D11VertexShader **ppVertexShader		반한되는 정점 쉐이더 포인터
	========================================================================
	*/


	hr = GetDevice()->CreateVertexShader((DWORD*)pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), NULL, &m_pVS);
	HRESULT_FAILED_RETURN_BOOL(hr);

	// Compile the PS from the file
	ID3DBlob* pPSBuf = NULL;
	hr = D3DX11CompileFromFile(L"HLSLwithoutFX.psh", NULL, NULL, "PS", "ps_5_0", dwShaderFlags,
		NULL, NULL, &pPSBuf, NULL, NULL);
	HRESULT_FAILED_RETURN_BOOL(hr);

	hr = GetDevice()->CreatePixelShader((DWORD*)pPSBuf->GetBufferPointer(), pPSBuf->GetBufferSize(), NULL, &m_pPS);
	HRESULT_FAILED_RETURN_BOOL(hr);

	// Create our vertex input layout 
	/*
	========================================================================
		D3D11_INPUT_ELEMENT_DESC		정점 레이아웃 객체 생성
	========================================================================
	LPCSTR SemanticName								쉐이더 입력 서명에서 이 요소에 연결된 HLSL을 의미
	UINT SemanticIndex								요소의 의미 인덱스. 의미 인덱스는 정수 인덱스를 통해 의미를 한정하는 것이다.
													의미 인덱스는 같은 의미의 요소가 둘 이상있는 경우에만 필요한다.
													예를 들어 4x4 매트릭스에는 4개의 구성요소가 있고, 각 구성 요소는 구문 이름으로
													matrix로 표시되지만, 4개의 구성 요소는 서로 다른 의미 색인(0, 1, 2, 3)가 할당된다.
	DXGI_FORMAT Format								요소 데이터 형식이다.
	UINT InputSlot									입력 어셈블러를 식별하는 정수이다. 유효한 값은 0~15dlrh D3D11.h에 정의되어 있다.
	UINT AlignedByteOffset							(옵션) 각 요소 사이의 오프셋(바이트)이다.
	D3D11_INPUT_CLASSIFICATION InputSlotClass		단일 입력, 슬롯 입력 데이터 클래스를 식별한다.
	UINT InstanceDataStepRate						인스턴스 데이터가 들어있는 정점 버퍼에서 렌더링되는 인스턴스의 수이다.
													슬롯 종류가 D3D11_INPUT_PER_INSTANCE_DATA의 경우 임의의 부호없는 정수 값을 사용할 수 있다.
													슬롯 종류가 D3D11_INPUT_PER_VERTEX_DATA의 경우는 0이어야 한다.
	========================================================================
	*/
	const D3D11_INPUT_ELEMENT_DESC layout[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	/*
	========================================================================
		CreateInputLayout  입력 레이아웃 객체 생성
	========================================================================
	const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs	입력 어셈블러 스테이지의 입력 셔식의 배열이며 데이터 형식은
														요소 기술에 의해 표시된다
	UINT NumElements									입력 요소의 배열의 입력 데이터 형식의 수
	const void *pShaderBytecodeWithInputSignature		컴파일된 쉐이더에 대한 포인터.
														컴파일된 쉐이더 코드는 입력 서명을 포함,
														이들은 요소의 배열에 대해 확인한다.
	ID3D11InputLayout **ppInputLayout)					생성되는 입력 레이아웃 포인터.
														다른 입력 매개 변수를 확인하려면
														이 포인터를 NULL로 설정하고 함수에서 S_FALSE를
														반환되는지 확인한다
	========================================================================
	*/
	hr = GetDevice()->CreateInputLayout(layout, 1, pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), &m_pVertexLayout);
	HRESULT_FAILED_RETURN_BOOL(hr);

	SAFE_RELEASE(pVSBuf);
	SAFE_RELEASE(pPSBuf);
	return hr;
}

bool Sample::Init()
{
	HRESULT hr;
	hr = CreateVertexBuffer();
	HRESULT_FAILED_RETURN_BOOL(hr);

	hr = LoadShaderAndInputLayout();
	HRESULT_FAILED_RETURN_BOOL(hr);

	return true;
}

bool Sample::Frame()
{
	return true;
}

bool Sample::Render()
{
	/*
	====================================================================================================================
		ID3D11DeviceContext::IASetInputLayout 정점 레이아웃을 디바이스 컨텍스트에 바인딩 한다.
	====================================================================================================================
	ID3D11InputLayout *pInputLayout
	====================================================================================================================
	*/
	m_pImmediateContext->IASetInputLayout(m_pVertexLayout);

	// Shaders

	/*
	====================================================================================================================
		ID3D11DeviceContext::VSSetShader		정점 쉐이더를 셋팅한다. 쉐이더 셋팅 함수는 해당 쉐이더 포인터를 제외하면 같다.
	====================================================================================================================
		ID3D11VertexShader *pVertexShader				버텍스 쉐이더에 대한 포인터
														NULL을 전달하면 이 파이프라인 스테이지의 쉐이더가 해제된다.
		ID3D11ClassInstance *const *ppClassInstances	클래스 인스턴스 인터페이스의 배열에 대한 포인터.
														쉐이더에서 사용하는 인터페이스는 해당 클래스 인스턴스를 가질 필요가 있다.
														만약 이 인스턴스가 없는 경우에는 쉐이더는 비활성화 된다.
														쉐이더 인터페이스를 사용하지 않는 경우는 NULL 값을 사용하면 된다.
		UINT NumClassInstances							배열 클래스 인스턴스 인터페이스의 갯수이다.
	====================================================================================================================
	*/
	m_pImmediateContext->VSSetShader(m_pVS, NULL, 0);
	m_pImmediateContext->HSSetShader(NULL, NULL, 0);
	m_pImmediateContext->DSSetShader(NULL, NULL, 0);
	m_pImmediateContext->GSSetShader(NULL, NULL, 0);
	m_pImmediateContext->PSSetShader(m_pPS, NULL, 0);

	// Set vertex buffer
	UINT stride = sizeof(P3_VERTEX);
	UINT offset = 0;
	/*
	====================================================================================================================
		ID3D11DeviceContext::IASetVertexBuffers		IA-Stage에 정점 버퍼를 바인딩한다.
	====================================================================================================================
		UINT StartSlot							바인딩에 사용되는 첫 번째 입력 슬롯. 첫번째 버테스 버퍼 시작 슬롯에 명시적으로 바인딩된다.
												따라서 배열의 각 후속 정점 버퍼는 암시적으로 후속 입력 슬롯에 각 바인딩되며
												최대 입력 슬롯이 16개 있다.
		UINT NumBuffers							배열의 정점 버퍼수이다. 버퍼 수는 IA 스테이지 입력 슬롯 수를 초과할 수 없다.
		ID3D11Buffer *const *ppVertexBuffers	버텍스 버퍼의 배열에 대한 주소. 정점 버퍼는 D3D11_BIND_VERTEX_BUFFER 플래그를
												사용하여 미리 만들어야 한다.
		const UINT *pStrides					스트라이드 값 배열에 대한 주소
		const UINT *pOffsets					오프셋 배열에 대한 포인터이며, 오프셋 값은 정점 버퍼 배열들의 거리를
												바이트 단위로 나타낸 것이다.
	====================================================================================================================
	*/
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	/*
	====================================================================================================================
		ID3D11DeviceContext::Draw			정점 버퍼에 있는 내용을 토대로 화면에 그린다.
	====================================================================================================================
		UINT VertexCount					정점의 개수
		UINT StartVertexLocation			첫번째 정점의 인덱스. 일반적으로 정점 버퍼의 오프셋.
											하지만 SV_TargetId System - Value 의미로 표시되는 쉐이더 매개 변수에 대해
											생성되는 첫 번쨰 정점 ID로 사용되는 경우도 있다.
	====================================================================================================================
	*/

	m_pImmediateContext->Draw(3, 0);
	return true;
}

bool Sample::Release()
{
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pPS);
	return true;
}

Sample::Sample()
{
	m_pVertexLayout = nullptr;
	m_pVertexBuffer = nullptr;
	m_pVS = nullptr;
	m_pPS = nullptr;
}


Sample::~Sample()
{
}