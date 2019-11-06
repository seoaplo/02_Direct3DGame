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
	if (m_pMainCamera == nullptr) return Message;
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
bool Sample::LoadFile()
{
	TCHAR ProgramPath[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, ProgramPath);
	while (1)
	{
		int iKey;
		TCHAR* TargetFile = SaveFileDiallog(L"SOA", L"SOAViewer");
		::SetCurrentDirectory(ProgramPath);

		if (TargetFile != nullptr)
		{
			iKey = I_SSSFileLoaderManeger.Load(TargetFile);
		}
		else
		{
			continue;
		}
		if (iKey < 0) continue;

		m_ObjList.clear();
		for (int iCount = 0; iCount < I_DrawObjectManager.GetSize(); iCount++)
		{
			m_ObjList.push_back(I_DrawObjectManager.GetDrawObject(iCount));
		}
		break;
	}
	return true;
}
bool Sample::Init()
{
	I_SSSFileLoaderManeger.Init(GetDevice(), GetContext());
	LoadFile();
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

	if (I_InputManager.KeyBoardState(DIK_K) == KEY_PUSH)
	{
		LoadFile();
	}
	if (I_InputManager.KeyBoardState(DIK_ESCAPE) == KEY_PUSH)
	{
		return false;
	}

	// 2초당 1회전( 1 초 * D3DX_PI = 3.14 )

	for (int iCount = 0; iCount < I_DrawObjectManager.GetSize(); iCount++)
	{
		m_ObjList[iCount]->Frame();
	}

	//m_pObj->Frame();
	m_pMainCamera->Frame();
	return true;
}
bool Sample::Render()
{
	//--------------------------------------------------------------------------------------
	// Direction 오브젝트 랜더링
	//--------------------------------------------------------------------------------------

	m_Direction.SetMatrix(&m_matWorld, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);

	//D3DXMatrixScaling(&m_matWorld, 100, 100, 100);
	for (int iCount = 0; iCount < I_DrawObjectManager.GetSize(); iCount++)
	{
		m_ObjList[iCount]->SetMatrix(nullptr, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);
		m_ObjList[iCount]->Render(GetContext());
	}

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
