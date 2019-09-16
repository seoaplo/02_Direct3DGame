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
	return m_MainCamera.WndProc(hWnd, Message, wParam, lParam);
}
bool Sample::Init()
{
	HRESULT hr;
	//====================================================================================================
	// 박스 오브젝트 생성 (100개 박스가 공유해서 사용함)
	//====================================================================================================
	SAFE_NEW(m_pBoxShape, SBox);
	HRESULT_FAILED_RETURN_BOOL(m_pBoxShape->Create(GetDevice(), m_pImmediateContext, L"../../data/Image/checker_with_numbers.bmp" , L"../../shader/Shape/Box.hlsl"));
	
	//====================================================================================================
	// 카메라 프로스텀 랜더링용 박스 오브젝트 생성
	//====================================================================================================
	m_MainCamera.CreateRenderBox(GetDevice(), m_pImmediateContext);
	m_pPixelShader.Attach(DXGame::LoadPixelShaderFile(GetDevice(), L"../../shader/Shape/box.hlsl", "PS_Color"));


	//====================================================================================================
	// 박스 오브젝트 생성 (100개 박스가 공유해서 사용함)
	//====================================================================================================
	for (int iBox = 0; iBox < 100; iBox++)
	{
		m_vBoxPosition[iBox] = D3DXVECTOR3(25 - rand() % 50, 0.0f, 25 - rand() % 50);
		D3DXVECTOR4 vColor = D3DXVECTOR4((rand() % 256) / 255.0f, (rand() % 256) / 255.0f, (rand() % 256) / 255.0f, 1.0f);
		m_vBoxColor[iBox] = vColor;
	}
	//====================================================================================================
	// 미니맵 영역에 랜더링할 랜더 타겟용  텍스쳐 생성
	//====================================================================================================
	if (m_MiniMap.Create(GetDevice(), m_pImmediateContext, nullptr ,L"../../shader/Shape/Plane.hlsl") == false)
	{
		return false;
	}
	m_MiniMap.Set(GetDevice(), 0, m_SwapChainDesc.BufferDesc.Height - 300, 300, 300);


	D3DXMatrixIdentity(&m_matBoxWorld);
	D3DXVECTOR3 vUpVector(0.0f, 1.0f, 0.0f);
	// 메인카메라 뷰 행렬 셋팅
	m_MainCamera.SetViewMatrix(D3DXVECTOR3(0.0f, 0.0f, -30.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), vUpVector);
	m_fRadius = D3DXVec3Length(&(m_MainCamera._vPos - m_MainCamera._vTarget));
	m_fCameraYaw	= m_MainCamera._fYawAngle;
	m_fCameraPitch	= m_MainCamera._fPitchAngle;
	// 투영행렬 세팅
	DXGI_SWAP_CHAIN_DESC Desc;
	HRESULT_FAILED_RETURN_BOOL(GetSwapChain()->GetDesc(&Desc));

	m_MainCamera.SetProjMatrix(D3DX_PI * 0.25f,
		Desc.BufferDesc.Width / (float)(Desc.BufferDesc.Height),
		0.1f, 50.0f);

	return true;
}

bool Sample::Frame()
{
	//--------------------------------------------------------------------------------------
	// 카메라 포지션 전후 좌우 이동
	//--------------------------------------------------------------------------------------
	m_fRadius = 0;
	if (I_InputManager.KeyBoardState(DIK_W) == KEY_HOLD)
	{
		m_MainCamera.MoveLook(I_Timer.GetSPF() * 2.0f);
	}
	if (I_InputManager.KeyBoardState(DIK_S) == KEY_HOLD)
	{
		m_MainCamera.MoveLook(-I_Timer.GetSPF() * 2.0f);
	}
	if (I_InputManager.KeyBoardState(DIK_D) == KEY_HOLD)
	{
		m_MainCamera.MoveSide(I_Timer.GetSPF() * 2.0f);
	}
	if (I_InputManager.KeyBoardState(DIK_A) == KEY_HOLD)
	{
		m_MainCamera.MoveSide(-I_Timer.GetSPF() * 2.0f);
	}
	if (I_InputManager.KeyBoardState(DIK_Q) == KEY_HOLD)
	{
		m_MainCamera.MoveUp(I_Timer.GetSPF() * 2.0f);
	}
	if (I_InputManager.KeyBoardState(DIK_E) == KEY_HOLD)
	{
		m_MainCamera.MoveUp(-I_Timer.GetSPF() * 2.0f);
	}
	//--------------------------------------------------------------------------------------
	// 카메라 회전
	//--------------------------------------------------------------------------------------
	if (I_InputManager.m_pMouseInput->ButtonCheck(0) == KEY_HOLD)
	{
		m_fCameraYaw += I_Timer.GetSPF() * I_InputManager.m_pMouseInput->MouseLocation().x;
		m_fCameraPitch += I_Timer.GetSPF() * I_InputManager.m_pMouseInput->MouseLocation().y;
	}
	if (I_InputManager.m_pMouseInput->MouseLocation().z != 0)
	{
		m_fRadius = -1.0f * (I_Timer.GetSPF() * I_InputManager.m_pMouseInput->MouseLocation().z);
	}

	m_MainCamera.Update(D3DXVECTOR4(m_fCameraPitch, m_fCameraYaw, m_fCameraRoll, m_fRadius));

	return m_MiniMap.Frame();
}
bool Sample::Render()
{
	HRESULT hr;
	S_SPHERE SphereObj;

	DXGame::SDxState::SetBlendState(m_pImmediateContext, DebugBSAlphaBlend);
	DXGame::SDxState::SetDepthStencilState(m_pImmediateContext, DebugDSSDepthEnable);
	DXGame::SDxState::SetRasterizerState(m_pImmediateContext, DebugRSBackCullSolid);

	
	//-----------------------------------------------------------------------
	// 박스 렌더링
	//-----------------------------------------------------------------------
	for (int iBox = 0; iBox < 100; iBox++)
	{
		m_matBoxWorld._41 = m_vBoxPosition[iBox].x;
		m_matBoxWorld._42 = m_vBoxPosition[iBox].y;
		m_matBoxWorld._43 = m_vBoxPosition[iBox].z;

		float fDistance = D3DXVec3Length(&(m_MainCamera._vPos - m_vBoxPosition[iBox])) / 100.0f;
		m_pBoxShape->_cbData.Color = D3DXVECTOR4(fDistance, fDistance, fDistance, 1.0f);
		m_pBoxShape->SetMatrix(&m_matBoxWorld, &m_MainCamera._matView, &m_MainCamera._matProj);

		// frusturm code
		if (m_MainCamera.ClassifyPoint(&m_vBoxPosition[iBox]))
		{
			m_pBoxShape->Render(m_pImmediateContext);
		}
		else
		{
			SphereObj.vCenter = m_vBoxPosition[iBox];
			SphereObj.fRadius = sqrt(3.0f);
			if (m_MainCamera.ClassifySphere(&SphereObj))
			{
				m_pBoxShape->_cbData.Color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
				m_pBoxShape->Render(m_pImmediateContext);
			}
		}
	}
	//-----------------------------------------------------------------------
	// 미니맵의 랜더타켓 텍스처에 100개 오브젝트를 탑뷰로 랜더링
	//-----------------------------------------------------------------------
	if (m_MiniMap.BeginRender(m_pImmediateContext))
	{
		for (int iBox = 0; iBox < 100; iBox++)
		{
			m_matBoxWorld._41 = m_vBoxPosition[iBox].x;
			m_matBoxWorld._42 = m_vBoxPosition[iBox].y;
			m_matBoxWorld._43 = m_vBoxPosition[iBox].z;

			DXGame::SDxState::SetBlendState(m_pImmediateContext, DebugBSNoAlphaBlend);
			m_pBoxShape->SetMatrix(&m_matBoxWorld, &m_MiniMap._matView, &m_MiniMap._matProj);

			// frusturm code
			// 완전히 포함된 오브젝트
			if (m_MainCamera.ClassifyPoint(&m_vBoxPosition[iBox]))
			{
				float fDistance = D3DXVec3Length(&(m_MainCamera._vPos - m_vBoxPosition[iBox])) / 100.0f;
				m_pBoxShape->_cbData.Color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
				m_pBoxShape->Render(m_pImmediateContext);
			}
			else
			{
				SphereObj.vCenter = m_vBoxPosition[iBox];
				SphereObj.fRadius = sqrt(3.0f);
				// 밖으로 걸쳐 있는 오브젝트
				if (m_MainCamera.ClassifySphere(&SphereObj))
				{
					m_pBoxShape->_cbData.Color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
					m_pBoxShape->Render(m_pImmediateContext);
				}
				else
				{   // 완전히 제외된 오브젝트
					m_pBoxShape->_cbData.Color = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
					m_pBoxShape->Render(m_pImmediateContext);
				}
			}
		}
		// 프로스텀 렌더링
		m_MainCamera.SetMatrix(NULL, &m_MiniMap._matView, &m_MiniMap._matProj);
		DXGame::SDxState::SetBlendState(m_pImmediateContext, DebugBSAlphaBlend);
		m_MainCamera._pBoxShape->_cbData.Color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.5f);
		m_MainCamera.PreRender(m_pImmediateContext);
		{
			m_pImmediateContext->PSSetShader(m_pPixelShader.Get(), NULL, 0);
			m_MainCamera.PostRender(m_pImmediateContext);
		}
		m_MiniMap.EndRender(m_pImmediateContext);
	}

	DXGame::SDxState::SetBlendState(m_pImmediateContext, DebugBSOneZero);
	DXGame::SDxState::SetDepthStencilState(m_pImmediateContext, DebugDSSDepthDisable);
	m_MiniMap.Render(m_pImmediateContext);

	//-----------------------------------------------------------------------
	// 적용되어 있는 카메라의 방향벡터 표시
	//-----------------------------------------------------------------------
	T_STR	str;
	RECT			rc;
	str.clear();
	TCHAR pBuffer[256];
	memset(pBuffer, 0, sizeof(TCHAR) * 256);
	_stprintf_s(pBuffer, _T("Look:%10.4f,%10.4f,%10.4f \n"), m_MainCamera._vLook.x,
		m_MainCamera._vLook.y,
		m_MainCamera._vLook.z);
	str += pBuffer;

	memset(pBuffer, 0, sizeof(TCHAR) * 256);
	_stprintf_s(pBuffer, _T("Up:%10.4f,%10.4f,%10.4f \n"),
		m_MainCamera._vLook.x,
		m_MainCamera._vLook.y,
		m_MainCamera._vLook.z);
	str += pBuffer;

	memset(pBuffer, 0, sizeof(TCHAR) * 256);
	_stprintf_s(pBuffer, _T("Right:%10.4f,%10.4f,%10.4f "),
		m_MainCamera._vLook.x,
		m_MainCamera._vLook.y,
		m_MainCamera._vLook.z);
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
	SAFE_DEL(m_pBoxShape);
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

	UINT iRectWidth = iWidth / 3;
	UINT iRectHeight = iHeight / 3;

	m_MiniMapVP.Set(GetDevice(), 0, iHeight - iRectHeight, iRectWidth, iRectHeight, 0.0f, 1.0f);

	return hr;
}

Sample::Sample()
{
	m_fCameraYaw = 0.0f;
	m_fCameraPitch = 0.0f;
	m_fCameraRoll = 0.0f;
	m_fRadius = 0.0f;
	SAFE_ZERO(m_pBoxShape);
}
Sample::~Sample()
{

}
