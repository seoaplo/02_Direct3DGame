#include "Sample.h"


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Sample g_Sample;
	RECT rc{ 0, 0, 1024, 768 };
	if (!g_Sample.InitWindow(hInstance, nCmdShow, const_cast<TCHAR*>(L"TextureTest"), rc, false))
	{
		MessageBox(0, _T("Sample  �ʱ�ȭ ����"), _T("Fatal error"), MB_OK);
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
	// UINT ByteWidth					�����Ǵ� ���� ũ��(Byte)
	// D3D11_USAGE Usage				�����Ǵ� ������ �б� �� ���� ����� �����Ѵ�. ������ ���ŵǴ� ������ ���� �� ���� �����ȴ�.
	//									�Ϲ������� D3D11_USAGE_DEFAULT�� ����Ѵ�. �̿��� ����� �� �ִ� ��� ���� ���ؼ���
	//									"D3D11_USAGE" �������� �����Ǿ� �ִ�.
	// UINT BindFlags					���۰� ���������ο� ���ε��ϴ� ����� �����ϸ� ���� ���� ������� ���ۿ� ������ �� �� �ֵ���
	//									D3D11_BIND_FLAG ������ ���� �������� ������ �� �ִ�.
	// UINT CPUAccessFlags				CPU�� �����Ǵ� ���ۿ� �����ϴ� ������ �����Ѵ�. CPU �׼����� �ʿ� ���� ��쿡�� 0�� �̿��Ѵ�
	//									D3D11_CPU_ACCESS_FLAG ������ �ټ��� �������� ������ �� �ִ�.
	// UINT MiscFlags					�߰����� �ɼ� �÷��׷� "D3D11_RESOURCE_MISC_FLAG" �������� ���ȴ�.
	// UINT StructureByteStride			����ȭ�� ���۸� ����� ��쿡 ����ü ������ ũ��
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
		const void *pSysMem			�ʱ�ȭ �����Ϳ� ���� ������
		UINT SysMemPitch			�ؽ��� ���ҽ��� ���� ũ����(����Ʈ ����)�̴�. �� ���� 2D�� 3D �ؽ��� �����Ϳ��� ���Ǹ�
									�ٸ� ������ ���ҽ������� �ǹ̸� ���� �ʴ´�.
		UINT SysMemSlicePitch		3���� �ؽ����� ������ ����(����Ʈ ����) ũ���̴�. �� ���� 3D �ؽ��� �����Ϳ��� ���Ǹ�
									�ٸ� ������ ���ҽ������� �ǹ̸� ���� �ʴ´�.
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
		LPVOID STDMETHODCALLTYPE GetBufferPointer	�����Ϳ� ���� �����͸� ��´�
		SIZE_T STDMETHODCALLTYPE GetBufferSize		ũ�⸦ ��´�
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
		LPCWSTR pSrcFile						������ �ϰ��� �ϴ� ���̴��� ���� ��
		CONST D3D10_SHADER_MACRO* pDefines		���̴� �ڵ� �ȿ��� ����� ��ũ�� �迭�� �ּ�. ������� ���� ��� NULL
		LPD3D10INCLUDE pInclude					ID3DInclude�������̽��� �����ϸ� ������� ���� ���� NULL�� �Ѵ�.
												�������̽��� ����� �� ���̴� �ڵ� �ȿ� #Include�� ���ԵǾ� ������ ������ �߻��Ѵ�.
		LPCSTR pFunctionName					���̴� ������ �Լ� �̸�
		LPCSTR pProfile							�������� ���̴� ����� �����ϴ� ���ڿ�, ���̴� �� 2 ���̴� �� 3
												�Ǵ� ���̴� �� 4�� ��� �������� ����� �� �ִ�.
		UINT Flags1								���̴� ������ �÷���
		UINT Flags2								����Ʈ �ɼ� ������ �÷���
		ID3DX11ThreadPump* pPump				ID3DX11ThreadPump �������̽��� �����ϸ� �񵿱����� ���ҽ��� �ε��� ����Ѵ�.
												������� ���� ��� NULL
		ID3D10Blob** ppShader					������ �� ���̴��� ���Ե� ����� ������ �ɺ� ���̺� ������ �����ϴ� ID3DBlob
												�������̽��� ���� �������̴�.
		ID3D10Blob** ppErrorMsgs				������ �� �� ���� �� ��� ����� �����ϴ� ID3D10Blob �������̽��� ���� ������.
												�̷��� ������ ���� ����ſ��� ����� ��°� �����ϴ�.
		HRESULT* pHResult						��ȯ���� ���� �ּ�. �ʿ����� ���� ��쿡�� NULL�� ����. pPump�� NULL�� �ƴ� ���
												�񵿱������� ������ �Ϸ�� ������ pHResult���� ��ȯ ���� ���� �ޱ� ���� ����ϴ�
												�����ʹ� ������� �ʵ��� �ؾ� �Ѵ�.
	========================================================================
	*/

	hr = D3DX11CompileFromFile(L"HLSLwithoutFX10.vsh", NULL, NULL, "VS", "vs_5_0", dwShaderFlags,
		NULL, NULL, &pVSBuf, NULL, NULL);
	HRESULT_FAILED_RETURN_BOOL(hr);
	/*
	========================================================================
		CreateVertexShader		�����ϵ� ���̴� �ڵ带 ����Ͽ� �ش� ���� ���̴��� ����
	========================================================================
	const void *pShaderBytecode				������ �� ���̴��� ���� ������
	SIZE_T BytecodeLength					������ �� ���̴� �ڵ��� ����
	ID3D11VertexShader **ppVertexShader		���ѵǴ� ���� ���̴� ������
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
		D3D11_INPUT_ELEMENT_DESC		���� ���̾ƿ� ��ü ����
	========================================================================
	LPCSTR SemanticName								���̴� �Է� ������ �� ��ҿ� ����� HLSL�� �ǹ�
	UINT SemanticIndex								����� �ǹ� �ε���. �ǹ� �ε����� ���� �ε����� ���� �ǹ̸� �����ϴ� ���̴�.
													�ǹ� �ε����� ���� �ǹ��� ��Ұ� �� �̻��ִ� ��쿡�� �ʿ��Ѵ�.
													���� ��� 4x4 ��Ʈ�������� 4���� ������Ұ� �ְ�, �� ���� ��Ҵ� ���� �̸�����
													matrix�� ǥ�õ�����, 4���� ���� ��Ҵ� ���� �ٸ� �ǹ� ����(0, 1, 2, 3)�� �Ҵ�ȴ�.
	DXGI_FORMAT Format								��� ������ �����̴�.
	UINT InputSlot									�Է� ������� �ĺ��ϴ� �����̴�. ��ȿ�� ���� 0~15dlrh D3D11.h�� ���ǵǾ� �ִ�.
	UINT AlignedByteOffset							(�ɼ�) �� ��� ������ ������(����Ʈ)�̴�.
	D3D11_INPUT_CLASSIFICATION InputSlotClass		���� �Է�, ���� �Է� ������ Ŭ������ �ĺ��Ѵ�.
	UINT InstanceDataStepRate						�ν��Ͻ� �����Ͱ� ����ִ� ���� ���ۿ��� �������Ǵ� �ν��Ͻ��� ���̴�.
													���� ������ D3D11_INPUT_PER_INSTANCE_DATA�� ��� ������ ��ȣ���� ���� ���� ����� �� �ִ�.
													���� ������ D3D11_INPUT_PER_VERTEX_DATA�� ���� 0�̾�� �Ѵ�.
	========================================================================
	*/
	const D3D11_INPUT_ELEMENT_DESC layout[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	/*
	========================================================================
		CreateInputLayout  �Է� ���̾ƿ� ��ü ����
	========================================================================
	const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs	�Է� ����� ���������� �Է� �Ž��� �迭�̸� ������ ������
														��� ����� ���� ǥ�õȴ�
	UINT NumElements									�Է� ����� �迭�� �Է� ������ ������ ��
	const void *pShaderBytecodeWithInputSignature		�����ϵ� ���̴��� ���� ������.
														�����ϵ� ���̴� �ڵ�� �Է� ������ ����,
														�̵��� ����� �迭�� ���� Ȯ���Ѵ�.
	ID3D11InputLayout **ppInputLayout)					�����Ǵ� �Է� ���̾ƿ� ������.
														�ٸ� �Է� �Ű� ������ Ȯ���Ϸ���
														�� �����͸� NULL�� �����ϰ� �Լ����� S_FALSE��
														��ȯ�Ǵ��� Ȯ���Ѵ�
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
		ID3D11DeviceContext::IASetInputLayout ���� ���̾ƿ��� ����̽� ���ؽ�Ʈ�� ���ε� �Ѵ�.
	====================================================================================================================
	ID3D11InputLayout *pInputLayout
	====================================================================================================================
	*/
	m_pImmediateContext->IASetInputLayout(m_pVertexLayout);

	// Shaders

	/*
	====================================================================================================================
		ID3D11DeviceContext::VSSetShader		���� ���̴��� �����Ѵ�. ���̴� ���� �Լ��� �ش� ���̴� �����͸� �����ϸ� ����.
	====================================================================================================================
		ID3D11VertexShader *pVertexShader				���ؽ� ���̴��� ���� ������
														NULL�� �����ϸ� �� ���������� ���������� ���̴��� �����ȴ�.
		ID3D11ClassInstance *const *ppClassInstances	Ŭ���� �ν��Ͻ� �������̽��� �迭�� ���� ������.
														���̴����� ����ϴ� �������̽��� �ش� Ŭ���� �ν��Ͻ��� ���� �ʿ䰡 �ִ�.
														���� �� �ν��Ͻ��� ���� ��쿡�� ���̴��� ��Ȱ��ȭ �ȴ�.
														���̴� �������̽��� ������� �ʴ� ���� NULL ���� ����ϸ� �ȴ�.
		UINT NumClassInstances							�迭 Ŭ���� �ν��Ͻ� �������̽��� �����̴�.
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
		ID3D11DeviceContext::IASetVertexBuffers		IA-Stage�� ���� ���۸� ���ε��Ѵ�.
	====================================================================================================================
		UINT StartSlot							���ε��� ���Ǵ� ù ��° �Է� ����. ù��° ���׽� ���� ���� ���Կ� ��������� ���ε��ȴ�.
												���� �迭�� �� �ļ� ���� ���۴� �Ͻ������� �ļ� �Է� ���Կ� �� ���ε��Ǹ�
												�ִ� �Է� ������ 16�� �ִ�.
		UINT NumBuffers							�迭�� ���� ���ۼ��̴�. ���� ���� IA �������� �Է� ���� ���� �ʰ��� �� ����.
		ID3D11Buffer *const *ppVertexBuffers	���ؽ� ������ �迭�� ���� �ּ�. ���� ���۴� D3D11_BIND_VERTEX_BUFFER �÷��׸�
												����Ͽ� �̸� ������ �Ѵ�.
		const UINT *pStrides					��Ʈ���̵� �� �迭�� ���� �ּ�
		const UINT *pOffsets					������ �迭�� ���� �������̸�, ������ ���� ���� ���� �迭���� �Ÿ���
												����Ʈ ������ ��Ÿ�� ���̴�.
	====================================================================================================================
	*/
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	/*
	====================================================================================================================
		ID3D11DeviceContext::Draw			���� ���ۿ� �ִ� ������ ���� ȭ�鿡 �׸���.
	====================================================================================================================
		UINT VertexCount					������ ����
		UINT StartVertexLocation			ù��° ������ �ε���. �Ϲ������� ���� ������ ������.
											������ SV_TargetId System - Value �ǹ̷� ǥ�õǴ� ���̴� �Ű� ������ ����
											�����Ǵ� ù ���� ���� ID�� ���Ǵ� ��쵵 �ִ�.
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