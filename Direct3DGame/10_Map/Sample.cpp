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

	BOX_VERTEX_LIST vertices;

	if (TestObject.Load(GetDevice(), L"../../data/Image/checker_with_numbers.bmp", vertices) == false)
	{
		return false;
	}
	m_TestDir.Set(GetDevice(), m_pImmediateContext, Vector, Vector, m_rcClientRect);
	m_TestDir.Load(GetDevice());

	PLANE_VERTEX_LIST planevertices;

	m_Plane.Set(GetDevice(), m_pImmediateContext, Vector, m_rcClientRect);
	m_Plane.Load(GetDevice(), L"../../data/Image/Attacker3.bmp", planevertices);

	D3DXMatrixIdentity(&m_matObjWorld);
	XDir = D3DXVECTOR3(1000.0f, 0.0f, 0.0f);
	YDir = D3DXVECTOR3(0.0f, 1000.0f, 0.0f);
	ZDir = D3DXVECTOR3(0.0f, 0.0f, 1000.0f);

	m_CameraBack.Init();
	DXGame::SVector3 Eye(0.0f, 0.0f, -4.0f);
	DXGame::SVector3 At(0.0f, 0.0f, 0.0f);
	DXGame::SVector3 Up(0.0f, 1.0f, 0.0f);
	m_CameraBack.CreateViewMatrix(Eye, At, Up);
	m_CameraBack.CreateProjMatrix(m_rcClientRect);

	m_CameraSide.Init();
	Eye = DXGame::SVector3(4.0f, 0.0f, 0.0f);
	m_CameraSide.CreateViewMatrix(Eye, At, Up);
	m_CameraSide.CreateProjMatrix(m_rcClientRect);

	m_CameraUp.Init();
	Eye = DXGame::SVector3(0.0001f, 2.0f, 0.0f);
	m_CameraUp.CreateViewMatrix(Eye, At, Up);
	m_CameraUp.CreateProjMatrix(m_rcClientRect);

	m_pMainCamera = &m_CameraUp;
	m_DebugString = L"UpCamera";

	D3DXMatrixScaling(&m_matObjWorld, (212 - 247), (282 - 328), 1.0f);
	D3DXMatrixIdentity(&m_matObjWorld);
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
	if (I_InputManager.KeyBoardState(DIK_D) == KEY_HOLD)
	{
		m_pMainCamera->RightRotation();
	}
	if (I_InputManager.KeyBoardState(DIK_A) == KEY_HOLD)
	{
		m_pMainCamera->LeftRotation();
	}

	m_pMainCamera->Frame();
	TestObject.Frame();
	m_TestDir.Frame();
	m_Plane.Frame();
	return true;
}
bool Sample::Render()
{
	//DXGame::ApplyDSS(m_pImmediateContext, DXGame::SDxState::g_pDSSDepthEnableNoWrite, 0xFF);
	//m_pImmediateContext->OMSetDepthStencilState(DXGame::SDxState::g_pDSSDepthEnable, 0xFF);
	//DXGame::ApplyRS(m_pImmediateContext, DXGame::SDxState::g_pRSNoneCullSolid);

	D3DXMatrixIdentity(&m_matObjWorld);

	TestObject.SetMatrix(&m_matObjWorld, (D3DXMATRIX*)&m_pMainCamera->m_matView, (D3DXMATRIX*)&m_pMainCamera->m_matProj);
	TestObject.Render(m_pImmediateContext);
	
	D3DXMatrixIdentity(&m_matObjWorld);
	m_matObjWorld._43 = 2.0f;
	m_Plane.SetMatrix(&m_matObjWorld, (D3DXMATRIX*)&m_pMainCamera->m_matView, (D3DXMATRIX*)&m_pMainCamera->m_matProj);
	m_Plane.Render(m_pImmediateContext);

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	m_TestDir.SetMatrix(&mat, (D3DXMATRIX*)&m_pMainCamera->m_matView, (D3DXMATRIX*)&m_pMainCamera->m_matProj);
	
	m_TestDir.SetVector(D3DXVECTOR3(0.0f, 0.0f, 0.0f), XDir, D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f));
	m_TestDir.Render(m_pImmediateContext);
	
	m_TestDir.SetVector(D3DXVECTOR3(0.0f, 0.0f, 0.0f), YDir, D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));
	m_TestDir.Render(m_pImmediateContext);

	m_TestDir.SetVector(D3DXVECTOR3(0.0f, 0.0f, 0.0f), ZDir, D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f));
	m_TestDir.Render(m_pImmediateContext);

	RECT rc;
	rc.left = 0;
	rc.top = 110;
	rc.right = m_nWindowWidth;
	rc.bottom = m_nWindowHeight;
	I_DirectWrite.DrawText(rc, m_DebugString.c_str());
	return true;
}
bool Sample::Release()
{
	TestObject.Release();
	return true;
}
Sample::Sample()
{
}
Sample::~Sample()
{

}
