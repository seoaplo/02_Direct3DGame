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

	if (FAILED(m_LineDraw.Create(GetDevice(), GetContext(),	nullptr	, L"../../shader/Shape/Line.hlsl")))
	{
		MessageBox(0, _T("m_LineDraw 실패"), _T("Fatal error"), MB_OK);
		return 0;
	}

	m_HeightMap.Init(GetDevice(), m_pImmediateContext);
	if (m_HeightMap.CreateHeightMap(L"../../data/heightMap513.bmp") == false)
	{
		return false;
	}

	SMapDesc MapDesc = { m_HeightMap.m_iNumRows,
							m_HeightMap.m_iNumCols,
							1.0f, 1.0f,
							L"../../data/baseColor.jpg",
							L"../../shader/16_HeightMap/HeightMap.hlsl" };
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
		1.0f, 1000.0f);

	return true;
}

bool Sample::Frame()
{

	float t = I_Timer.GetElapsedTime() * D3DX_PI;
	m_MainCamera.Frame();

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

	return m_HeightMap.Frame();
}
bool Sample::Render()
{
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
