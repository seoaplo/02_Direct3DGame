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
	if(m_pMainCamera == nullptr) return Message;
		return m_pMainCamera->WndProc(hWnd, Message, wParam, lParam);
}

bool Sample::Init()
{
	int iKey;

	I_SkinFileLoaderManeger.Init(GetDevice(), GetContext());
	I_MatrixFileLoader.Init(GetDevice(), GetContext());
	I_MatrixObjectListManager.Init(GetDevice(), GetContext());

	//iKey = I_SkinFileLoaderManeger.Load(L"../../testData/3DMax/Swat.skm");
	//iKey = I_MatrixFileLoader.Load(L"../../testData/3DMax/Swat.smc");
	iKey = I_SkinFileLoaderManeger.Load(L"../../testData/3DMax/TestSkin.skm");
	iKey = I_SkinFileLoaderManeger.Load(L"../../testData/3DMax/TestSkin1.skm");
	iKey = I_SkinFileLoaderManeger.Load(L"../../testData/3DMax/TestSkin2.skm");
	iKey = I_MatrixFileLoader.Load(L"../../testData/3DMax/TestMatrix.smc");

	//--------------------------------------------------------------------------------------
	// 월드  행렬
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matScale, matTrans;
	D3DXMatrixScaling(&m_matInitWorld, 1.0f, 1.0f, 1.0f);
	//--------------------------------------------------------------------------------------
	// 카메라  행렬 
	//--------------------------------------------------------------------------------------	
	m_pMainCamera = make_shared<SCamera>();

	/*for (int iCount = 0; iCount < I_SkinObjectManager.GetSize(); iCount++)
	{
		m_SkinObjList.push_back(I_SkinObjectManager.GetSkinObject(iCount));
	}
	for (int iCount = 0; iCount < I_SkinObjectManager.GetSize(); iCount++)
	{
		m_MatObjLists.push_back(I_MatrixObjectListManager.GetMatrixObjectList(iCount));
	}*/
	for (int iCount = 0; iCount < I_SkinObjectManager.GetSize(); iCount++)
	{
		m_SCAObject.SetSkinObject(iCount);
	}
	m_SCAObject.SetMatrixObjectList(0);

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

	// 2초당 1회전( 1 초 * D3DX_PI = 3.14 )

	//for (int iCount = 0; iCount < I_SkinObjectManager.GetSize(); iCount++)
	//{
	//	m_SkinObjList[iCount]->Frame();
	//}

	//for (int iCount = 0; iCount < I_MatrixObjectListManager.GetSize(); iCount++)
	//{
	//	m_MatObjLists[iCount]->Frame();
	//}

	m_SCAObject.Frame();
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

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixScaling(&m_matWorld, 1.0f, 1.0f, 1.0f);

	/*for (int iCount = 0; iCount < I_SkinObjectManager.GetSize(); iCount++)
	{
		m_SkinObjList[iCount]->SetMatrix(nullptr, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);
		m_SkinObjList[iCount]->Render(GetContext());
	}
	for (int iCount = 0; iCount < I_MatrixObjectListManager.GetSize(); iCount++)
	{
		m_MatObjLists[iCount]->SetMatrix(nullptr, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);
		m_MatObjLists[iCount]->Render(GetContext());
	}*/
	m_SCAObject.SetMatrix(&m_matWorld, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);
	m_SCAObject.Render(GetContext());
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
