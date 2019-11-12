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

	if (FAILED(m_LineDraw.Create(GetDevice(), GetContext(),	nullptr	, L"../../shader/Shape/Line.hlsl")))
	{
		MessageBox(0, _T("m_LineDraw 실패"), _T("Fatal error"), MB_OK);
		return 0;
	}

	SMapDesc MapDesc = { 50, 50, 1.0f, 0.1f,L"../../data/map/castle.jpg", L"../../shader/Shape/Plane.hlsl" };
	m_CustomMap.Init(GetDevice(), m_pImmediateContext);
	if (FAILED(m_CustomMap.Load(MapDesc)))
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
	m_MainCamera.SetViewMatrix(D3DXVECTOR3(0.0f, 30.0f, 0.0f),
		D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	m_MainCamera.SetProjMatrix(D3DX_PI * 0.25f,
		m_SwapChainDesc.BufferDesc.Width / (float)(m_SwapChainDesc.BufferDesc.Height),
		1.0f, 1000.0f);

	return true;
}

bool Sample::Frame()
{
	// 2초당 1회전( 1 초 * D3DX_PI = 3.14 )
	float t = cosf(I_Timer.GetElapsedTime())*0.5f;// *D3DX_PI;
	for (int z = 0; z < m_CustomMap.m_iNumRows; z++)
	{
		for (int x = 0; x < m_CustomMap.m_iNumCols; x++)
		{
			int iIndex = z * m_CustomMap.m_iNumCols + x;
			float fCos = cosf(t*x);
			float fSin = sinf(t*z);
			//m_CustomMap.m_VertexList[iIndex].p.y = fCos + fSin;
		}
	}
	m_pImmediateContext->UpdateSubresource(
		m_CustomMap._dxobj.g_pVertexBuffer.Get(), 0, 0, &m_CustomMap.m_VertexList.at(0), 0, 0);

	m_MainCamera.Frame();
	return m_CustomMap.Frame();
}
bool Sample::Render()
{

	DXGame::SDxState::SetDepthStencilState(GetContext(), DebugDSSDepthEnable);
	DXGame::SDxState::SetBlendState(GetContext(), DebugBSAlphaBlend);
	m_CustomMap.SetMatrix( &m_MainCamera._matWorld, &m_MainCamera._matView, &m_MainCamera._matProj);

	m_CustomMap.Render(m_pImmediateContext);

	DXGame::SDxState::SetDepthStencilState(GetContext(), DebugDSSDepthDisable);
	m_LineDraw.SetMatrix(nullptr, &m_MainCamera._matView, &m_MainCamera._matProj);
	D3DXVECTOR3 vStart(0, 0, 0);
	D3DXVECTOR3 vEnd(1000, 0, 0);
	m_LineDraw.Draw(m_pImmediateContext, vStart, vEnd, D3DXVECTOR4(1, 0, 0, 1.0f));
	vEnd = D3DXVECTOR3(0, 1000, 0);
	m_LineDraw.Draw(m_pImmediateContext, vStart, vEnd, D3DXVECTOR4(0, 1, 0, 1.0f));
	vEnd = D3DXVECTOR3(0, 0, 1000);
	m_LineDraw.Draw(m_pImmediateContext, vStart, vEnd, D3DXVECTOR4(0, 0, 1, 1.0f));

	return true;
}
bool Sample::Release()
{
	return true;
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
