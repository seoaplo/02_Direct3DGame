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

bool Sample::Init()
{
	HRESULT hr;
	D3DXVECTOR3 Vector;

	TestObject.Set(GetDevice(), m_pImmediateContext, Vector, m_rcClientRect);
	TestObject.Load(GetDevice(), L"");

	D3DXMatrixIdentity(&m_matObjWorld);

	m_CameraBack.Init();
	DXGame::SVector3 Eye(0.0f, 0.0f, 2.0f);
	DXGame::SVector3 At(0.0f, 0.0f, 0.0f);
	DXGame::SVector3 Up(0.0f, 1.0f, 0.0f);
	m_CameraBack.CreateViewMatrix(Eye, At, Up);
	m_CameraBack.CreateProjMatrix(m_rcClientRect);

	m_CameraSide.Init();
	Eye = DXGame::SVector3(2.0f, 0.0f, -0.3f);
	m_CameraSide.CreateViewMatrix(Eye, At, Up);
	m_CameraSide.CreateProjMatrix(m_rcClientRect);

	m_pMainCamera = &m_CameraBack;
	m_DebugString = L"BackCamera";
	return true;
}
bool Sample::Frame()
{
	if (I_InputManager.m_pMouseInput->ButtonCheck(0) == KEY_PUSH)
	{
		if (m_pMainCamera == &m_CameraBack)
		{
			m_pMainCamera = &m_CameraSide;
			m_DebugString = L"SideCamera";
		}
		else
		{
			m_pMainCamera = &m_CameraBack;
			m_DebugString = L"BackCamera";
		}
	}

	if (I_InputManager.KeyBoardState(DIK_W) == KEY_HOLD)
	{
		m_pMainCamera->Forward();
	}
	if (I_InputManager.KeyBoardState(DIK_S) == KEY_HOLD)
	{
		m_pMainCamera->Backward();
	}
	m_pMainCamera->Frame();
	TestObject.Frame();
	return true;
}
bool Sample::Render()
{
	
	TestObject.SetMatrix(nullptr, (D3DXMATRIX*)&m_pMainCamera->m_matView, (D3DXMATRIX*)&m_pMainCamera->m_matProj);
	TestObject.Render(m_pImmediateContext);

	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = 200;
	rc.bottom = 40;
	I_DirectWrite.DrawText(rc, m_DebugString.c_str());
	return true;
}
bool Sample::Release()
{
	TestObject.Release();
	ReportLiveObjects();
	return true;
}
Sample::Sample()
{
}
Sample::~Sample()
{

}
