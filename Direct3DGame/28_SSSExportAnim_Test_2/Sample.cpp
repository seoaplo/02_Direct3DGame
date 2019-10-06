#include "Sample.h"
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Sample g_Sample;
	RECT rc{ 0, 0, 1024, 768 };
	if (!g_Sample.InitWindow(hInstance, nCmdShow, const_cast<TCHAR*>(L"TextureTest"), rc, false))
	{
		MessageBox(0, _T("Sample  �ʱ�ȭ ����"), _T("Fatal error"), MB_OK);
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

bool Sample::Init()
{
	int iKey;
	I_SSSFileLoaderManeger.Init(GetDevice(), GetContext());
	iKey = I_SSSFileLoaderManeger.Load( L"../../testData/3DMax/TestAnim3.SSS");

	for (int i = 0; i < I_SSSFileLoaderManeger.GetSize(); i++)
	{
		m_ObjectList.push_back(I_SSSFileLoaderManeger.GetPtr(i));
	}
	

	//--------------------------------------------------------------------------------------
	// ����  ���
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matScale, matTrans;
	D3DXMatrixScaling(&m_matInitWorld, 1.0f, 1.0f, 1.0f);
	//--------------------------------------------------------------------------------------
	// ī�޶�  ��� 
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

	// 2�ʴ� 1ȸ��( 1 �� * D3DX_PI = 3.14 )

	//m_pObj->Frame();
	m_pMainCamera->Frame();
	for (int i = 0; i < m_ObjectList.size(); i++)
	{
		m_ObjectList[i]->Frame();
	}
	return true;
}
bool Sample::Render()
{
	//--------------------------------------------------------------------------------------
	// Direction ������Ʈ ������
	//--------------------------------------------------------------------------------------
	m_Direction.SetMatrix(&m_matWorld, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);
	//m_pObj->SetMatrix(nullptr, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);

	//m_pObj->Render(GetContext());
	for (int i = 0; i < m_ObjectList.size(); i++)
	{
		m_ObjectList[i]->SetMatrix(nullptr, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);
		m_ObjectList[i]->Render(GetContext());
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