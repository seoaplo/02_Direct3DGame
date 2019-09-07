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

	// Direction Setting
	m_Direction.Set(GetDevice(), m_pImmediateContext, m_rcClientRect);
	m_Direction.Load(GetDevice());

	// Plane Setting
	PLANE_VERTEX_LIST planevertices;
	m_Plane.Set(GetDevice(), m_pImmediateContext, Vector, m_rcClientRect);
	m_Plane.Load(GetDevice(), L"../../data/Image/Attacker3.bmp", planevertices);

	// Box Setting
	BOX_VERTEX_LIST vertices;
	if (m_Box.Load(GetDevice(), L"../../data/Image/checker_with_numbers.bmp", vertices) == false)
	{
		return false;
	}

	D3DXMatrixIdentity(&m_World[0]);
	D3DXMatrixIdentity(&m_World[1]);
	D3DXVECTOR3 vTargetPosition(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 vUpVector(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 vCameraPosition(0.0f, 10.0f, -1.0f);

	// Top g_matView
	m_Camera[0].SetViewMatrix(vCameraPosition, vTargetPosition, vUpVector);
	// Front g_matView
	vCameraPosition = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_Camera[1].SetViewMatrix(vCameraPosition, vTargetPosition, vUpVector);
	// Side g_matView
	vCameraPosition = D3DXVECTOR3(10.0f, 0.0f, 0.0f);
	m_Camera[2].SetViewMatrix(vCameraPosition, vTargetPosition, vUpVector);
	// User g_matView
	vCameraPosition = D3DXVECTOR3(10.0f, 10.0f, -10.0f);
	m_Camera[3].SetViewMatrix(vCameraPosition, vTargetPosition, vUpVector);

	for (int iCamera = 0; iCamera < 4; iCamera++)
	{
		m_ViewPort[iCamera].m_vp.Width = m_nWindowWidth / 2;
		m_ViewPort[iCamera].m_vp.Height = m_nWindowHeight / 2;
		m_Camera[iCamera].SetProjMatrix(D3DX_PI * 0.25,
										(float)m_ViewPort[iCamera].m_vp.Width /
										(float)m_ViewPort[iCamera].m_vp.Height,
										1.0f,
										100.0f);
	}

	D3DXVECTOR3 vMax = D3DXVECTOR3(-10000.0f, -10000.0f, -10000.0f);
	D3DXVECTOR3 vMin = D3DXVECTOR3(10000.0f, 10000.0f, 10000.0f);

	for (int iVer = 0; iVer < 8; iVer++)
	{
		if (m_Box.m_BoxVertexList[iVer].p.x > vMax.x) vMax.x = m_Box.m_BoxVertexList[iVer].p.x;
		if (m_Box.m_BoxVertexList[iVer].p.y > vMax.y) vMax.y = m_Box.m_BoxVertexList[iVer].p.y;
		if (m_Box.m_BoxVertexList[iVer].p.z > vMax.z) vMax.z = m_Box.m_BoxVertexList[iVer].p.z;

		if (m_Box.m_BoxVertexList[iVer].p.x < vMin.x) vMin.x = m_Box.m_BoxVertexList[iVer].p.x;
		if (m_Box.m_BoxVertexList[iVer].p.y < vMin.y) vMin.y = m_Box.m_BoxVertexList[iVer].p.y;
		if (m_Box.m_BoxVertexList[iVer].p.z < vMin.z) vMin.z = m_Box.m_BoxVertexList[iVer].p.z;
	}

	m_Camera[0].SetObjectView(vMax, vMin);
	m_Camera[1].SetObjectView(vMax, vMin);
	m_Camera[2].SetObjectView(vMax, vMin);
	m_Camera[3].SetObjectView(vMax, vMin);

	D3DXMATRIX matRotX, matScale;
	D3DXMatrixIdentity(&matRotX);
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixRotationX(&matRotX, D3DXToRadian(90));
	D3DXMatrixScaling(&matScale, 2.0f, 2.0f, 2.0f);
	D3DXMatrixMultiply(&m_World[1], &matScale, &matRotX);

	return true;
}
bool Sample::Frame()
{
	float t = I_Timer.GetElapsedTime() * D3DX_PI;

	if (I_InputManager.KeyBoardState(DIK_W) == KEY_HOLD)
	{
		m_fPitch += I_Timer.GetSPF();
	}
	if (I_InputManager.KeyBoardState(DIK_S) == KEY_HOLD)
	{
		m_fPitch -= I_Timer.GetSPF();
	}
	if (I_InputManager.KeyBoardState(DIK_D) == KEY_HOLD)
	{
		m_fYaw += I_Timer.GetSPF();
	}
	if (I_InputManager.KeyBoardState(DIK_A) == KEY_HOLD)
	{
		m_fYaw -= I_Timer.GetSPF();
	}
	if (I_InputManager.KeyBoardState(DIK_Q) == KEY_HOLD)
	{
		m_fRoll += I_Timer.GetSPF();
	}
	if (I_InputManager.KeyBoardState(DIK_E) == KEY_HOLD)
	{
		m_fRoll += I_Timer.GetSPF();
	}

	D3DXMATRIX matRot;
	D3DXMatrixRotationYawPitchRoll(&m_World[0], m_fYaw, m_fPitch, m_fRoll);

	m_Box.Frame();
	m_Direction.Frame();
	m_Plane.Frame();

	return true;
}
bool Sample::Render()
{
	HRESULT hr;

	D3D11_VIEWPORT vpOld[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
	UINT nViewPorts = 1;


	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);

	m_pImmediateContext->RSGetViewports(&nViewPorts, vpOld);

	//-----------------------------------------------------------------------
	// 상단 뷰포트
	//-----------------------------------------------------------------------	
	for (int iVp = 0; iVp < 2; iVp++)
	{
		m_ViewPort[iVp].Apply(m_pImmediateContext, GetRenderTargetView(), GetDepthStencilView());
		m_Box.SetMatrix(&m_World[0], m_Camera[iVp].GetViewMatrix(), m_Camera[iVp].GetProjMatrix());
		m_Box.Render(m_pImmediateContext);

		m_Plane.SetMatrix(&m_World[1], m_Camera[iVp].GetViewMatrix(), m_Camera[iVp].GetProjMatrix());
		m_Plane.Render(m_pImmediateContext);
	}
	//-----------------------------------------------------------------------
	// 하단 뷰포트
	//-----------------------------------------------------------------------
	for (int iVp = 2; iVp < 4; iVp++)
	{
		m_ViewPort[iVp].Apply(m_pImmediateContext, GetRenderTargetView(), GetDepthStencilView());
		m_Box.SetMatrix(&m_World[0], m_Camera[iVp].GetViewMatrix(), m_Camera[iVp].GetProjMatrix());
		m_Box.Render(m_pImmediateContext);

		m_Plane.SetMatrix(&m_World[1], m_Camera[iVp].GetViewMatrix(), m_Camera[iVp].GetProjMatrix());
		m_Plane.Render(m_pImmediateContext);
	}
	//-----------------------------------------------------------------------
	// 기본 뷰포트 정보로 복원
	//-----------------------------------------------------------------------
	m_pImmediateContext->RSSetViewports(nViewPorts, vpOld);

	m_pImmediateContext->RSGetViewports(&nViewPorts, vpOld);
	//-----------------------------------------------------------------------
	// Debug Direction
	//-----------------------------------------------------------------------
	DXGame::SDxState::SetDepthStencilState(m_pImmediateContext, DebugDSSDepthDisable);
	m_ViewPort[0].Apply(m_pImmediateContext, GetRenderTargetView(), GetDepthStencilView());
	m_Direction.SetMatrix(nullptr, m_Camera[0].GetViewMatrix(), m_Camera[0].GetProjMatrix());
	m_Direction.Render(m_pImmediateContext);

	m_ViewPort[1].Apply(m_pImmediateContext, GetRenderTargetView(), GetDepthStencilView());
	m_Direction.SetMatrix(nullptr, m_Camera[1].GetViewMatrix(), m_Camera[1].GetProjMatrix());
	m_Direction.Render(m_pImmediateContext);

	m_ViewPort[2].Apply(m_pImmediateContext, GetRenderTargetView(), GetDepthStencilView());
	m_Direction.SetMatrix(nullptr, m_Camera[2].GetViewMatrix(), m_Camera[2].GetProjMatrix());
	m_Direction.Render(m_pImmediateContext);

	m_ViewPort[3].Apply(m_pImmediateContext, GetRenderTargetView(), GetDepthStencilView());
	m_Direction.SetMatrix(nullptr, m_Camera[3].GetViewMatrix(), m_Camera[3].GetProjMatrix());
	m_Direction.Render(m_pImmediateContext);

	DXGame::SDxState::SetDepthStencilState(m_pImmediateContext, DebugDSSDepthEnable);

	m_pImmediateContext->RSSetViewports(nViewPorts, vpOld);

	//-----------------------------------------------------------------------
	// 뷰포트 번호 출력
	//-----------------------------------------------------------------------
	for (int iVp = 0; iVp < 4; iVp++)
	{
		RECT rc;
		rc.left = m_ViewPort[iVp].m_vp.TopLeftX + m_ViewPort[iVp].m_vp.Width*0.5f;
		rc.top = m_ViewPort[iVp].m_vp.TopLeftY;
		rc.right = m_ViewPort[iVp].m_vp.Width + rc.left;
		rc.bottom = m_ViewPort[iVp].m_vp.Height + rc.top;

		// wchar_t* _itow( int value, wchar_t* sr, int radix )	// 숫자형식을 유니코드 문자열로 바꾼다.
		// int _wtoi( const wchar_t* str );						// 유티코드 문자열을 숫자형식으로 바꾼다.
		T_STR str = CameraViewStyle[iVp];
		TCHAR strNumber[32];
		str += _itow_s(iVp, strNumber, 10);// _wtoi
		I_DirectWrite.DrawText(rc, const_cast<TCHAR*>(str.c_str()));
	}

	return true;
}
bool Sample::Release()
{
	m_Box.Release();
	m_Plane.Release();
	m_Direction.Release();
	return true;
}
HRESULT Sample::CreateResource()
{
	HRESULT hr;
	if (FAILED(hr = ScreenViewPort(m_SwapChainDesc.BufferDesc.Width, m_SwapChainDesc.BufferDesc.Height)))
	{
		return hr;
	}

	return S_OK;
}
HRESULT Sample::DeleteResource()
{
	HRESULT hr = S_OK;
	return S_OK;
}
HRESULT Sample::ScreenViewPort(UINT iWidth, UINT iHeight)
{
	HRESULT hr = S_OK;

	UINT iRectWidth = iWidth / 2;
	UINT iRectHeight = iHeight / 2;

	m_ViewPort[0].Set(GetDevice(), 0, 0, iRectWidth, iRectHeight, 0.0f, 1.0f);
	m_ViewPort[1].Set(GetDevice(), iRectWidth, 0, iRectWidth, iRectHeight, 0.0f, 1.0f);
	m_ViewPort[2].Set(GetDevice(), 0, iRectHeight, iRectWidth, iRectHeight, 0.0f, 1.0f);
	m_ViewPort[3].Set(GetDevice(), iRectWidth, iRectHeight, iRectWidth, iRectHeight, 0.0f, 1.0f);

	return hr;
}

Sample::Sample()
{
}
Sample::~Sample()
{

}
