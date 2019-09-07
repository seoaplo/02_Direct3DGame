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

	if (m_Direction.Load(GetDevice()) == false)
	{
		return false;
	}

	// Plane Setting
	PLANE_VERTEX_LIST planevertices;
	m_Plane.Set(GetDevice(), m_pImmediateContext, Vector, m_rcClientRect);
	if (m_Plane.Load(GetDevice(), L"../../data/Image/Attacker3.bmp", planevertices) == false)
	{
		return false;
	}

	// Box Setting
	BOX_VERTEX_LIST vertices;
	if (m_Box.Load(GetDevice(), L"../../data/Image/checker_with_numbers.bmp", vertices) == false)
	{
		return false;
	}

	D3DXMatrixIdentity(&m_World[0]);
	D3DXMatrixIdentity(&m_World[1]);

	D3DXVECTOR3 vUpVector(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 vCameraPosition(0.0f, 10.0f, -1.0f);

	// Top g_matView
	m_Camera[0].SetViewMatrix(vCameraPosition, m_vObjectPosition, vUpVector);
	// Front g_matView
	vCameraPosition = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_Camera[1].SetViewMatrix(vCameraPosition, m_vObjectPosition, vUpVector);
	// Side g_matView
	vCameraPosition = D3DXVECTOR3(10.0f, 0.0f, 0.0f);
	m_Camera[2].SetViewMatrix(vCameraPosition, m_vObjectPosition, vUpVector);
	// User g_matView
	vCameraPosition = D3DXVECTOR3(10.0f, 10.0f, -10.0f);
	m_Camera[3].SetViewMatrix(vCameraPosition, m_vObjectPosition, vUpVector);

	D3DXVECTOR3 vMax = D3DXVECTOR3(2.0f, 2.0f, 2.0f);
	D3DXVECTOR3 vMin = D3DXVECTOR3(-2.0f, -2.0f, -2.0f);

	m_Camera[0].SetObjectView(vMax, vMin);
	m_Camera[1].SetObjectView(vMax, vMin);
	m_Camera[2].SetObjectView(vMax, vMin);
	m_Camera[3].SetObjectView(vMax, vMin);

	m_MainCamera.SetViewMatrix( m_Camera[m_iCameraType].GetPositionVector(),
								m_Camera[m_iCameraType].GetTargetVector(),
								vUpVector);

	m_fRadius = D3DXVec3Length(&(m_MainCamera.GetPositionVector() - m_MainCamera.GetTargetVector()));
	
	DXGI_SWAP_CHAIN_DESC Desc;
	HRESULT_FAILED_RETURN_BOOL(GetSwapChain()->GetDesc(&Desc));
	for (int iCamera = 0; iCamera < 4; iCamera++)
	{

		m_Camera[iCamera].SetProjMatrix(D3DX_PI * 0.25,
			(float)m_ViewPort[iCamera].m_vp.Width /
			(float)m_ViewPort[iCamera].m_vp.Height,
			1.0f,
			100.0f);
	}
	m_MainCamera.SetProjMatrix(D3DX_PI * 0.25,
		(float)Desc.BufferDesc.Width /
		(float)Desc.BufferDesc.Height,
		1.0f,
		100.0f);

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

	D3DXMatrixRotationY(&m_World[0], t * 0.1f);

	if (I_InputManager.KeyBoardState(DIK_LCONTROL) == KEY_UP)
	{
		m_iCameraType = (m_iCameraType + 1) % 3;
		m_fCameraYaw = m_Camera[m_iCameraType].GetYaw();
		m_fCameraPitch = m_Camera[m_iCameraType].GetPitch();
		// 카메라 타입이 교체되면 교체되는 카메라 타켓위치로 메인카메라의 타켓을 변경한다.
		m_vObjectPosition = m_Camera[m_iCameraType].GetTargetVector();
	}
	if (I_InputManager.KeyBoardState(DIK_W) == KEY_HOLD)
	{
		m_fRadius += I_Timer.GetSPF();
	}
	if (I_InputManager.KeyBoardState(DIK_S) == KEY_HOLD)
	{
		m_fRadius -= I_Timer.GetSPF();
	}
	if (I_InputManager.KeyBoardState(DIK_D) == KEY_HOLD)
	{
		m_vObjectPosition.x += I_Timer.GetSPF();
	}
	if (I_InputManager.KeyBoardState(DIK_A) == KEY_HOLD)
	{
		m_vObjectPosition.x -= I_Timer.GetSPF();
	}
	m_fRadius += I_Timer.GetSPF() * I_InputManager.m_pMouseInput->MouseLocation().z;

	if (I_InputManager.m_pMouseInput->ButtonCheck(0) == KEY_HOLD)
	{
		m_fCameraYaw += I_Timer.GetSPF() * I_InputManager.m_pMouseInput->MouseLocation().x;
		m_fCameraPitch += I_Timer.GetSPF() * I_InputManager.m_pMouseInput->MouseLocation().y;
	}
	m_MainCamera.Update(D3DXVECTOR4(m_fCameraPitch, m_fCameraYaw, m_fCameraRoll, m_fRadius));

	m_MainCamera.Frame();
	m_Camera[0].Frame();
	m_Camera[1].Frame();
	m_Camera[2].Frame();
	m_Camera[3].Frame();

	m_Box.Frame();
	m_Direction.Frame();
	m_Plane.Frame();

	return true;
}
bool Sample::Render()
{
	HRESULT hr;

	m_World[0]._41 = m_World[1]._41 = m_vObjectPosition.x;
	for (int iObj = 0; iObj < 2; iObj++)
	{
		m_Box.SetMatrix(&m_World[0], &m_MainCamera.GetViewMatrix(), &m_MainCamera.GetProjMatrix());
		m_Box.Render(m_pImmediateContext);

		m_Plane.SetMatrix(&m_World[1], &m_MainCamera.GetViewMatrix(), &m_MainCamera.GetProjMatrix());
		m_Plane.Render(m_pImmediateContext);
	}

	D3D11_VIEWPORT vpOld[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
	UINT nViewPorts = 1;

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);

	m_pImmediateContext->RSGetViewports(&nViewPorts, vpOld);

	//-----------------------------------------------------------------------
	// 뷰포트
	//-----------------------------------------------------------------------
	m_World[0]._41 = m_World[1]._41 = 0.0f;
	for (int iVp = 0; iVp < 4; iVp++)
	{
		m_ViewPort[iVp].Apply(m_pImmediateContext, GetRenderTargetView(), GetDepthStencilView());
		m_Box.SetMatrix(&m_World[0], &m_Camera[iVp].GetViewMatrix(), &m_Camera[iVp].GetProjMatrix());
		m_Box.Render(m_pImmediateContext);

		m_Plane.SetMatrix(&m_World[1], &m_Camera[iVp].GetViewMatrix(), &m_Camera[iVp].GetProjMatrix());
		m_Plane.Render(m_pImmediateContext);
	}
	//-----------------------------------------------------------------------
	// 기본 뷰포트 정보로 복원
	//-----------------------------------------------------------------------
	m_pImmediateContext->RSSetViewports(nViewPorts, vpOld);

	//-----------------------------------------------------------------------
	// Debug Direction
	//-----------------------------------------------------------------------
	m_pImmediateContext->RSGetViewports(&nViewPorts, vpOld);

	DXGame::SDxState::SetDepthStencilState(m_pImmediateContext, DebugDSSDepthDisable);
	m_ViewPort[0].Apply(m_pImmediateContext, GetRenderTargetView(), GetDepthStencilView());
	m_Direction.SetMatrix(nullptr, &m_Camera[0].GetViewMatrix(), &m_Camera[0].GetProjMatrix());
	m_Direction.Render(m_pImmediateContext);

	m_ViewPort[1].Apply(m_pImmediateContext, GetRenderTargetView(), GetDepthStencilView());
	m_Direction.SetMatrix(nullptr, &m_Camera[1].GetViewMatrix(), &m_Camera[1].GetProjMatrix());
	m_Direction.Render(m_pImmediateContext);

	m_ViewPort[2].Apply(m_pImmediateContext, GetRenderTargetView(), GetDepthStencilView());
	m_Direction.SetMatrix(nullptr, &m_Camera[2].GetViewMatrix(), &m_Camera[2].GetProjMatrix());
	m_Direction.Render(m_pImmediateContext);

	m_ViewPort[3].Apply(m_pImmediateContext, GetRenderTargetView(), GetDepthStencilView());
	m_Direction.SetMatrix(nullptr, &m_Camera[3].GetViewMatrix(), &m_Camera[3].GetProjMatrix());
	m_Direction.Render(m_pImmediateContext);

	DXGame::SDxState::SetDepthStencilState(m_pImmediateContext, DebugDSSDepthEnable);

	m_pImmediateContext->RSSetViewports(nViewPorts, vpOld);

	//-----------------------------------------------------------------------
	// 뷰포트 번호 출력
	//-----------------------------------------------------------------------
	RECT rc;
	for (int iVp = 0; iVp < 4; iVp++)
	{
		rc.left = m_ViewPort[iVp].m_vp.TopLeftX + m_ViewPort[iVp].m_vp.Width*0.5f;
		rc.top = m_ViewPort[iVp].m_vp.TopLeftY;
		rc.right = m_ViewPort[iVp].m_vp.Width + rc.left;
		rc.bottom = m_ViewPort[iVp].m_vp.Height + rc.top;

		T_STR str = CameraViewStyle[iVp];
		TCHAR strNumber[32];
		str += _itow_s(iVp, strNumber, 10);// _wtoi
		I_DirectWrite.DrawText(rc, const_cast<TCHAR*>(str.c_str()));
	}
	//-----------------------------------------------------------------------
	// 적용되어 있는 카메라 타입 표시
	//-----------------------------------------------------------------------	
	rc.left = m_DefaultRT.m_vp.TopLeftX + m_DefaultRT.m_vp.Width*0.5f;
	rc.top = m_DefaultRT.m_vp.TopLeftY;
	rc.right = m_DefaultRT.m_vp.Width;
	rc.bottom = m_DefaultRT.m_vp.Height;
	T_STR str = CameraViewStyle[m_iCameraType];
	TCHAR strNumber[32];
	str += _itow_s(m_iCameraType, strNumber, 10);// _wtoi
	I_DirectWrite.DrawText(rc, const_cast<TCHAR*>(str.c_str()));

	//-----------------------------------------------------------------------
	// 적용되어 있는 카메라의 방향벡터 표시
	//-----------------------------------------------------------------------
	str.clear();
	TCHAR pBuffer[256];
	memset(pBuffer, 0, sizeof(TCHAR) * 256);
	_stprintf_s(pBuffer, _T("Look:%10.4f,%10.4f,%10.4f \n"), m_MainCamera.GetLookVector().x,
		m_MainCamera.GetLookVector().y,
		m_MainCamera.GetLookVector().z);
	str += pBuffer;

	memset(pBuffer, 0, sizeof(TCHAR) * 256);
	_stprintf_s(pBuffer, _T("Up:%10.4f,%10.4f,%10.4f \n"),
		m_MainCamera.GetUpVector().x,
		m_MainCamera.GetUpVector().y,
		m_MainCamera.GetUpVector().z);
	str += pBuffer;

	memset(pBuffer, 0, sizeof(TCHAR) * 256);
	_stprintf_s(pBuffer, _T("Right:%10.4f,%10.4f,%10.4f "),
		m_MainCamera.GetRightVector().x,
		m_MainCamera.GetRightVector().y,
		m_MainCamera.GetRightVector().z);
	str += pBuffer;

	rc.left = m_DefaultRT.m_vp.TopLeftX + m_DefaultRT.m_vp.Width*0.5f;
	rc.top = m_DefaultRT.m_vp.Height - 75;
	rc.right = m_DefaultRT.m_vp.Width;
	rc.bottom = m_DefaultRT.m_vp.Height;
	I_DirectWrite.DrawText(rc, const_cast<TCHAR*>(str.c_str()));
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
	if (m_pImmediateContext) m_pImmediateContext->ClearState();
	return S_OK;
}
HRESULT Sample::ScreenViewPort(UINT iWidth, UINT iHeight)
{
	HRESULT hr = S_OK;

	UINT iRectWidth = iWidth / 5;
	UINT iRectHeight = iHeight / 4;

	m_ViewPort[0].Set(GetDevice(), 0, 0, iRectWidth, iRectHeight, 0.0f, 1.0f);
	m_ViewPort[1].Set(GetDevice(), 0, iRectHeight, iRectWidth, iRectHeight, 0.0f, 1.0f);
	m_ViewPort[2].Set(GetDevice(), 0, iRectHeight * 2, iRectWidth, iRectHeight, 0.0f, 1.0f);
	m_ViewPort[3].Set(GetDevice(), 0, iRectHeight * 3, iRectWidth, iRectHeight, 0.0f, 1.0f);

	return hr;
}

Sample::Sample()
{
}
Sample::~Sample()
{

}
