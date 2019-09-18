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
bool Sample::DrawDebug()
{
	if (I_InputManager.KeyBoardState(DIK_P))
	{
		// 정점노말 표시
		m_LineDraw.SetMatrix(nullptr, &m_MainCamera._matView, &m_MainCamera._matProj);

		D3DXMATRIX invWorld;
		D3DXMatrixIdentity(&invWorld);
		D3DXMatrixInverse(&invWorld, 0, &m_MainCamera._matWorld);
		D3DXMatrixTranspose(&invWorld, &invWorld);
		D3DXVECTOR3 vStart, vEnd, vDir, vEye;
		vEye = m_MainCamera._vLook;

		for (UINT row = 0; row < m_HeightMap.m_iNumRows; row += 10)
		{
			for (UINT col = 0; col < m_HeightMap.m_iNumCols; col += 10)
			{
				// vStart는 위치벡터, vDir는 방향벡터
				D3DXVec3TransformCoord(&vStart, &m_HeightMap.m_VertexList[row*m_HeightMap.m_iNumCols + col].p, &m_MainCamera._matWorld);
				D3DXVec3TransformNormal(&vDir, &m_HeightMap.m_VertexList[row*m_HeightMap.m_iNumCols + col].n, &invWorld);
				D3DXVec3Normalize(&vDir, &vDir);
				vEnd = vStart + vDir * 2.0f;
				float fDot = D3DXVec3Dot(&vEye, &vDir);
				if (fDot < 0)
				{
					vDir.x = vDir.x * 0.5f + 0.5f;
					vDir.y = vDir.y * 0.5f + 0.5f;
					vDir.z = vDir.z * 0.5f + 0.5f;
					m_LineDraw.Draw(m_pImmediateContext, vStart, vEnd, D3DXVECTOR4(vDir.x, vDir.y, vDir.z, 1.0f));
				}
			}
		}
	}
	if (!SCore::DrawDebug()) return false;
}
bool Sample::Init()
{
	HRESULT hr;

	m_cbLight.g_cAmbientMaterial = D3DXVECTOR4(0.1f, 0.1f, 0.1f, 1);
	m_cbLight.g_cDiffuseMaterial = D3DXVECTOR4(1, 1, 1, 1);
	m_cbLight.g_cSpecularMaterial = D3DXVECTOR4(1, 1, 1, 1);

	m_cbLight.g_cAmbientLightColor = D3DXVECTOR4(1, 1, 1, 1);
	m_cbLight.g_cDiffuseLightColor = D3DXVECTOR4(1, 1, 1, 1);
	m_cbLight.g_cSpecularLightColor = D3DXVECTOR4(1, 1, 1, 1);

	m_pConstantBufferLight.Attach(DXGame::CreateConstantBuffer(
		m_pDevice, &m_cbLight, 1, sizeof(LIGHT_CONSTANT_BUFFER)));

	if (FAILED(m_LineDraw.Create(GetDevice(), GetContext())))
	{
		MessageBox(0, _T("m_LineDraw 실패"), _T("Fatal error"), MB_OK);
		return 0;
	}

	//--------------------------------------------------------------------------------------
	// 박스 오브젝트를 구 오브젝트로 변환(기하 쉐이더 및 스트림 아웃 처리)
	//--------------------------------------------------------------------------------------
	if (FAILED(m_SphereObj.Create(GetDevice(), GetContext(), L"../../data/tilea.jpg", L"../../shader/18_SpecularLight/BoxSO.hlsl")))
	{
		MessageBox(0, _T("m_SphereObj 실패"), _T("Fatal error"), MB_OK);
		return 0;
	}
	m_SphereObj.SetSO(GetDevice(), GetContext());
	//--------------------------------------------------------------------------------------
	// 높이맵 생성
	//--------------------------------------------------------------------------------------
	m_HeightMap.Init(GetDevice(), m_pImmediateContext);
	if (m_HeightMap.CreateHeightMap(L"../../data/heightMap513.bmp") == false)
	{
		return false;
	}

	SMapDesc MapDesc = { m_HeightMap.m_iNumRows,
							m_HeightMap.m_iNumCols,
							2.0f, 0.5f,
							L"../../data/baseColor.jpg",
							L"../../shader/18_SpecularLight/SpecularLight.hlsl" };
	if (!m_HeightMap.Load(MapDesc))
	{
		return false;
	}
	if (FAILED(CreateResource()))
	{
		return false;
	}

	//--------------------------------------------------------------------------------------
	// 카메라  행렬 
	//--------------------------------------------------------------------------------------
	m_MainCamera.SetViewMatrix(D3DXVECTOR3(0.0f, 300.0f, -300.0f),
		D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	m_MainCamera.SetProjMatrix(D3DX_PI * 0.25f,
		m_SwapChainDesc.BufferDesc.Width / (float)(m_SwapChainDesc.BufferDesc.Height),
		1.0f, 3000.0f);

	return true;
}

bool Sample::Frame()
{

	float t = 0;//I_Timer.GetElapsedTime() * D3DX_PI;
	m_MainCamera.Frame();

	D3DXMATRIX mLightWorld, mTranslate, mRotation;
	D3DXMatrixTranslation(&mTranslate, 100.0f, 100.0f, 0.0f);
	D3DXMatrixRotationY(&mRotation, t*0.0f);
	D3DXMatrixIsIdentity(&mRotation);
	D3DXMatrixMultiply(&mLightWorld, &mTranslate, &mRotation);

	m_vLightVector.x = mLightWorld._41;
	m_vLightVector.y = mLightWorld._42;
	m_vLightVector.z = mLightWorld._43;

	D3DXVec3Normalize(&m_vLightVector, &m_vLightVector);
	m_vLightVector *= -1.0f;


	if (I_InputManager.KeyBoardState(DIK_F6) == KEY_UP)
	{
		for (UINT row = 0; row < m_HeightMap.m_iNumRows; row += 1)
		{
			for (UINT col = 0; col < m_HeightMap.m_iNumCols; col += 1)
			{
				m_HeightMap.m_VertexList[row*m_HeightMap.m_iNumCols + col].p.y *= -1.0f;
				m_HeightMap.m_VertexList[row*m_HeightMap.m_iNumCols + col].c = D3DXVECTOR4(1, 1, 1, 1);
			}
		}
		m_HeightMap.ReLoadVBuffer();
	}

	m_SphereObj.Frame();
	return m_HeightMap.Frame();
}
bool Sample::Render()
{
	D3DXMATRIX matWorld, matScale;
	D3DXMatrixScaling(&matScale, 100, 100, 100);
	D3DXMatrixRotationY(&matWorld, I_Timer.GetElapsedTime());
	D3DXMatrixMultiply(&matWorld, &matScale, &matWorld);
	matWorld._42 = 200.0f;

	m_cbLight.g_vLightDir.x = m_vLightVector.x;
	m_cbLight.g_vLightDir.y = m_vLightVector.y;
	m_cbLight.g_vLightDir.z = m_vLightVector.z;
	m_cbLight.g_vLightDir.w = 1;

	D3DXMATRIX matInvWorld;
	D3DXMatrixInverse(&matInvWorld, NULL, &matWorld);
	D3DXMatrixTranspose(&matInvWorld, &matInvWorld);
	D3DXMatrixTranspose(&m_cbLight.g_matInvWorld, &matInvWorld);

	m_cbLight.g_vEyeDir.x = m_MainCamera._vLook.x;
	m_cbLight.g_vEyeDir.y = m_MainCamera._vLook.y;
	m_cbLight.g_vEyeDir.z = m_MainCamera._vLook.z;
	m_cbLight.g_vEyeDir.w = 10.0f; // 강도
	m_cbLight.g_cSpecularMaterial = D3DXVECTOR4(3.0f, 1.0f, 1.0f, 1);

	m_pImmediateContext->UpdateSubresource(m_pConstantBufferLight.Get(), 0, NULL, &m_cbLight, 0, 0);
	m_pImmediateContext->VSSetConstantBuffers(1, 1, m_pConstantBufferLight.GetAddressOf());
	m_pImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBufferLight.GetAddressOf());

	m_SphereObj.SetMatrix(&matWorld, &m_MainCamera._matView, &m_MainCamera._matProj);
	m_SphereObj.Render(m_pImmediateContext);

	D3DXMatrixIdentity(&m_cbLight.g_matInvWorld);
	m_pImmediateContext->UpdateSubresource(m_pConstantBufferLight.Get(), 0, NULL, &m_cbLight, 0, 0);

	m_HeightMap.SetMatrix( &m_MainCamera._matWorld, &m_MainCamera._matView, &m_MainCamera._matProj);
	return m_HeightMap.Render(m_pImmediateContext);
}
bool Sample::Release()
{
	return m_HeightMap.Release();
}
HRESULT Sample::CreateResource()
{
	HRESULT hr = S_OK;
	float fAspectRatio = m_SwapChainDesc.BufferDesc.Width / (float)m_SwapChainDesc.BufferDesc.Height;
	m_MainCamera.SetProjMatrix(D3DX_PI / 4, fAspectRatio, 0.1f, 500.0f);

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

}
Sample::~Sample()
{

}
