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
int Sample::WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if(m_pMainCamera == nullptr) return Message;
		return m_pMainCamera->WndProc(hWnd, Message, wParam, lParam);
}

bool Sample::Init()
{
	int iKey;
	I_SSSFileLoaderManeger.Init(GetDevice(), GetContext());
	iKey = I_SSSFileLoaderManeger.Load( L"../../testData/3DMax/Test2.SSS");
	m_pObj = I_SSSFileLoaderManeger.GetPtr(iKey);


	//--------------------------------------------------------------------------------------
	// 월드  행렬
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matScale, matTrans;
	D3DXMatrixScaling(&m_matInitWorld, 1.0f, 1.0f, 1.0f);
	//--------------------------------------------------------------------------------------
	// 카메라  행렬 
	//--------------------------------------------------------------------------------------	
	m_pMainCamera = make_shared<SCamera>();


	float fAspectRatio = m_nWindowWidth / (FLOAT)m_nWindowHeight;
	m_pMainCamera->SetViewMatrix(D3DXVECTOR3(0.0f, 0.0f, -10.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_pMainCamera->SetProjMatrix(D3DX_PI * 0.25f,
		m_SwapChainDesc.BufferDesc.Width / (float)(m_SwapChainDesc.BufferDesc.Height),
		1.0f, 3000.0f);

	m_pMainCamera->Init();
	return true;
}

bool Sample::Frame()
{

	// 2초당 1회전( 1 초 * D3DX_PI = 3.14 )
	float t = I_Timer.GetElapsedTime() * D3DX_PI;
	m_pMainCamera->Frame();
	D3DXMatrixIdentity(&m_matWorld);

	m_pObj->Frame();
	return true;
}
bool Sample::Render()
{
	//--------------------------------------------------------------------------------------
	// Direction 오브젝트 랜더링
	//--------------------------------------------------------------------------------------
	m_Direction.SetMatrix(&m_matWorld, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);
	m_pObj->SetMatrix(nullptr, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);

	m_pObj->Render(GetContext());
	return true;
}
bool Sample::Release()
{
	return true;
}
bool Sample::ExportData(T_STR FilePath)
{

	FILE* pStream = nullptr;
	_wfopen_s(&pStream, FilePath.c_str(), _T("rb"));

	static TCHAR ExporterCheck[MAX_PATH];
	static TCHAR CheckFile[MAX_PATH];
	static TCHAR ObjectCheck[MAX_PATH];
	static TCHAR ObjectNameCheck[MAX_PATH];
	static TCHAR PNCT_LINE[MAX_PATH];

	int iMaxObj = 0;

	ZeroMemory(ExporterCheck, _countof(ExporterCheck));
	ZeroMemory(CheckFile, _countof(CheckFile));
	ZeroMemory(ObjectCheck, _countof(ObjectCheck));
	ZeroMemory(PNCT_LINE, _countof(PNCT_LINE));

	_ftscanf_s(pStream, _T("%s %d"), CheckFile, _countof(CheckFile),&iMaxObj);

	T_STR FileCheck;
	FileCheck = CheckFile;
	if (wcscmp(FileCheck.data(), _T("sssexporter100")) != 0)
	{
		FileCheck = FilePath;
		FileCheck += _T("은 SSS Export File이 아닙니다");
		MessageBox(0, FileCheck.data(), _T("Fatal error"), MB_OK);
		return false;
	}

	VertexList.resize(iMaxObj);
	for (int iCount = 0; iCount < iMaxObj; iCount++)
	{
		int iMaxVertex;
		_ftscanf_s(pStream, _T("%s %d"), ObjectNameCheck, _countof(ObjectNameCheck), &iMaxVertex);

		VertexList[iCount].resize(iMaxVertex * 3);

		for (int iVertex = 0; iVertex < iMaxVertex * 3; iVertex++)
		{
			PNCT_VERTEX pnct;
			_ftscanf_s(pStream, _T("%f %f %f"), &pnct.p.x, &pnct.p.y, &pnct.p.z);

			_ftscanf_s(pStream, _T("%f %f %f"),	&pnct.n.x, &pnct.n.y, &pnct.n.z);

			_ftscanf_s(pStream, _T("%f %f %f %f"),	&pnct.c.x, &pnct.c.y, &pnct.c.z, &pnct.c.w);

			_ftscanf_s(pStream, _T("%f %f"),
				&pnct.t.x, &pnct.t.y);

			VertexList[iCount][iVertex] = pnct;
		}
	}
	fclose(pStream);



	return true;
}
HRESULT Sample::CreateResource()
{
	HRESULT hr = S_OK;
	//float fAspectRatio = m_SwapChainDesc.BufferDesc.Width / (float)m_SwapChainDesc.BufferDesc.Height;
	//m_pMainCamera->SetProjMatrix(D3DX_PI / 4, fAspectRatio, 0.1f, 500.0f);

	return S_OK;
}
HRESULT Sample::DeleteResource()
{
	HRESULT hr = S_OK;
	if (m_pImmediateContext) m_pImmediateContext->ClearState();
	return S_OK;
}

Sample::Sample()
{
	m_pMainCamera = nullptr;
	m_bDebugRender = true;
}
Sample::~Sample()
{

}
