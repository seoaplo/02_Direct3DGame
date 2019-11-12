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

TCHAR* Sample::SaveFileDiallog(const TCHAR* szName, const TCHAR* szTitle)
{
	TCHAR ARRAY_TCHAR_file[MAX_PATH] = { 0, };
	TCHAR ARRAY_TCHAR_filetitle[MAX_PATH] = { 0, };
	static TCHAR* Static_Pointer_TCHAR_filter;
	OPENFILENAME OPENFILENAME_filename;

	//=======================================================================
	// Initiallize
	//=======================================================================
	ZeroMemory(&OPENFILENAME_filename, sizeof(OPENFILENAME));
	_tcscpy_s(ARRAY_TCHAR_file, _T("*."));
	_tcscat_s(ARRAY_TCHAR_file, szName);
	_tcscat_s(ARRAY_TCHAR_file, _T("\0"));

	//=======================================================================
	// Set OPENFILENAME
	//=======================================================================
	OPENFILENAME_filename.lStructSize = sizeof(OPENFILENAME);
	OPENFILENAME_filename.hwndOwner = GetActiveWindow();
	OPENFILENAME_filename.lpstrFilter = Static_Pointer_TCHAR_filter;
	OPENFILENAME_filename.lpstrCustomFilter = NULL;
	OPENFILENAME_filename.nMaxCustFilter = 0L;
	OPENFILENAME_filename.nFilterIndex = 1;
	OPENFILENAME_filename.lpstrFile = ARRAY_TCHAR_file;
	OPENFILENAME_filename.nMaxFile = sizeof(ARRAY_TCHAR_file);
	OPENFILENAME_filename.lpstrFileTitle = ARRAY_TCHAR_filetitle;
	OPENFILENAME_filename.nMaxFileTitle = sizeof(ARRAY_TCHAR_filetitle);
	OPENFILENAME_filename.lpstrInitialDir = NULL;
	OPENFILENAME_filename.lpstrTitle = szTitle;
	OPENFILENAME_filename.Flags = 0L;
	OPENFILENAME_filename.nFileOffset = 0;
	OPENFILENAME_filename.nFileExtension = 0;
	OPENFILENAME_filename.lpstrDefExt = szName;


	//=======================================================================
	// Save File
	//=======================================================================
	if (!GetOpenFileName(&OPENFILENAME_filename))
	{
		return NULL;
	}
	return ARRAY_TCHAR_file;
}
bool Sample::LoadSKMFile()
{
	TCHAR ProgramPath[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, ProgramPath);
	while (1)
	{
		int iKey;
		TCHAR* TargetFile = SaveFileDiallog(L"skm", L"SkinViewer");
		::SetCurrentDirectory(ProgramPath);

		if (TargetFile != nullptr)
		{
			iKey = I_SkinFileLoaderManeger.Load(TargetFile);
		}
		else
		{
			break;
		}
		if (iKey < 0)
		{
			break;
		}

		m_SCAObject.SetSkinObject(iKey);
		break;
	}
	return true;
}

bool Sample::LoadSMCFile()
{
	TCHAR ProgramPath[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, ProgramPath);
	while (1)
	{
		int iKey;
		TCHAR* TargetFile = SaveFileDiallog(L"smc", L"MatViewer");
		::SetCurrentDirectory(ProgramPath);

		if (TargetFile != nullptr)
		{
			iKey = I_MatrixFileLoader.Load(TargetFile);
		}
		else
		{
			break;
		}
		if (iKey < 0) break;

		m_SCAObject.SetMatrixObjectList(iKey);
		break;
	}
	return true;
}
void Sample::ClearSCAObject()
{
	m_SCAObject.Release();
	I_SkinObjectManager.Clear();
	I_MatrixObjectListManager.Clear();
}
bool Sample::Init()
{
	int iKey;

	I_SkinFileLoaderManeger.Init(GetDevice(), GetContext());
	I_MatrixFileLoader.Init(GetDevice(), GetContext());
	I_MatrixObjectListManager.Init(GetDevice(), GetContext());

	//--------------------------------------------------------------------------------------
	// 월드  행렬
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matScale, matTrans;
	D3DXMatrixScaling(&m_matInitWorld, 1.0f, 1.0f, 1.0f);
	//--------------------------------------------------------------------------------------
	// 카메라  행렬 
	//--------------------------------------------------------------------------------------	
	m_pMainCamera = make_shared<SCamera>();

	m_SCAObject.Create(GetDevice(), GetContext());

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

	if (I_InputManager.KeyBoardState(DIK_K) == KEY_PUSH)
	{
		LoadSKMFile();
	}
	if (I_InputManager.KeyBoardState(DIK_L) == KEY_PUSH)
	{
		LoadSMCFile();
	}
	if (I_InputManager.KeyBoardState(DIK_P) == KEY_PUSH)
	{
		ClearSCAObject();
	}
	if (I_InputManager.KeyBoardState(DIK_ESCAPE) == KEY_PUSH)
	{
		return false;
	}

	m_SCAObject.Frame();
	m_pMainCamera->Frame();
	return true;
}
bool Sample::Render()
{
	//--------------------------------------------------------------------------------------
	// Direction 오브젝트 랜더링
	//--------------------------------------------------------------------------------------
	
	m_Direction.SetMatrix(&m_matWorld, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixScaling(&m_matWorld, 1.0f, 1.0f, 1.0f);

	m_SCAObject.SetMatrix(&m_matWorld, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);
	m_SCAObject.Render(GetContext());
	return true;
}
bool Sample::Release()
{
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
