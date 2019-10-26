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
	if (m_pMainCamera == nullptr) return Message;
	return m_pMainCamera->WndProc(hWnd, Message, wParam, lParam);
}

bool Sample::Init()
{
	//--------------------------------------------------------------------------------------
	// 월드  행렬
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matScale, matTrans;
	D3DXMatrixScaling(&m_matInitWorld, 1.0f, 1.0f, 1.0f);
	//--------------------------------------------------------------------------------------
	// 카메라  행렬 
	//--------------------------------------------------------------------------------------	
	m_pMainCamera = make_shared<SCamera>();

	float fAspectRatio = m_nWindowWidth / (FLOAT)m_nWindowHeight;
	m_pMainCamera->SetViewMatrix(D3DXVECTOR3(0.0f, 0.0f, -10.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_pMainCamera->SetProjMatrix(D3DX_PI * 0.25f,
		m_SwapChainDesc.BufferDesc.Width / (float)(m_SwapChainDesc.BufferDesc.Height),
		1.0f, 1000.0f);

	m_pMainCamera->Init();
	m_Line.Create(GetDevice(), GetContext());
	m_QuadTree.SetMaxDepthLimit(5);
	m_QuadTree.SetMinDivideSize(1);
	m_QuadTree.Build(500.0f, 500.0f);
	m_QuadTree.SetRenderDepth(0);
	return true;
}

bool Sample::Frame()
{

	// 2초당 1회전( 1 초 * D3DX_PI = 3.14 )
	float t = I_Timer.GetElapsedTime() * D3DX_PI;

	if (I_InputManager.KeyBoardState(DIK_1) == KEY_PUSH) m_QuadTree.SetRenderDepth(0);
	if (I_InputManager.KeyBoardState(DIK_2) == KEY_PUSH) m_QuadTree.SetRenderDepth(1);
	if (I_InputManager.KeyBoardState(DIK_3) == KEY_PUSH) m_QuadTree.SetRenderDepth(2);
	if (I_InputManager.KeyBoardState(DIK_4) == KEY_PUSH) m_QuadTree.SetRenderDepth(3);
	if (I_InputManager.KeyBoardState(DIK_5) == KEY_PUSH) m_QuadTree.SetRenderDepth(4);
	if (I_InputManager.KeyBoardState(DIK_6) == KEY_PUSH) m_QuadTree.SetRenderDepth(5);
	if (I_InputManager.KeyBoardState(DIK_7) == KEY_PUSH) m_QuadTree.SetRenderDepth(6);
	if (I_InputManager.KeyBoardState(DIK_8) == KEY_PUSH) m_QuadTree.SetRenderDepth(7);
	if (I_InputManager.KeyBoardState(DIK_9) == KEY_PUSH) m_QuadTree.SetRenderDepth(-1);

	m_pMainCamera->Frame();
	m_Line.Frame();
	D3DXMatrixIdentity(&m_matWorld);

	return true;
}
bool Sample::Render()
{
	//--------------------------------------------------------------------------------------
	// Direction 오브젝트 랜더링
	//--------------------------------------------------------------------------------------
	m_Direction.SetMatrix(&m_matWorld, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);
	m_Line.SetMatrix(&m_matWorld, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);
	DrawQuadLine(m_QuadTree.m_pRootNode);
	return true;
}
bool Sample::Release()
{
	return true;
}

bool Sample::DrawQuadLine(SNode* pNode)
{
	if (pNode == NULL) return false;
	if (m_QuadTree.m_iRenderDepth < pNode->m_dwDepth) return false;

	D3DXVECTOR4 vColor = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
	if (pNode->m_dwDepth == 0) vColor = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	if (pNode->m_dwDepth == 1) vColor = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	if (pNode->m_dwDepth == 2) vColor = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
	if (pNode->m_dwDepth == 3) vColor = D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f);
	if (pNode->m_dwDepth == 4) vColor = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);
	if (pNode->m_dwDepth == 5) vColor = D3DXVECTOR4(0.0f, 0.5f, 0.0f, 1.0f);
	if (pNode->m_dwDepth == 6) vColor = D3DXVECTOR4(1.0f, 0.5f, 0.0f, 1.0f);
	if (pNode->m_dwDepth == 7) vColor = D3DXVECTOR4(0.5f, 0.5f, 0.5f, 1.0f);
	if (pNode->m_dwDepth == 8) vColor = D3DXVECTOR4(1.0f, 0.5f, 0.5f, 1.0f);
	if (pNode->m_dwDepth == 9) vColor = D3DXVECTOR4(1.0f, 0.5f, 1.0f, 1.0f);

	S_BOX& TargetBox = pNode->m_sBox;
	D3DXVECTOR3 vPoint[4];
	vPoint[0] = D3DXVECTOR3(TargetBox.vMin.x, TargetBox.vMax.y, TargetBox.vMax.z);
	vPoint[0].y -= 1.0f * pNode->m_dwDepth;

	vPoint[1] = D3DXVECTOR3(TargetBox.vMax.x, TargetBox.vMax.y, TargetBox.vMax.z);
	vPoint[1].y -= 1.0f * pNode->m_dwDepth;

	vPoint[2] = D3DXVECTOR3(TargetBox.vMin.x, TargetBox.vMax.y, TargetBox.vMin.z);
	vPoint[2].y -= 1.0f * pNode->m_dwDepth;

	vPoint[3] = D3DXVECTOR3(TargetBox.vMax.x, TargetBox.vMax.y, TargetBox.vMin.z);
	vPoint[3].y -= 1.0f * pNode->m_dwDepth;

	m_Line.Draw(GetContext(), vPoint[0], vPoint[1], vColor);
	m_Line.Draw(GetContext(), vPoint[1], vPoint[3], vColor);
	m_Line.Draw(GetContext(), vPoint[2], vPoint[3], vColor);
	m_Line.Draw(GetContext(), vPoint[0], vPoint[2], vColor);

	if (pNode->m_ChildList.size() != 4) return true;
	for (int iNode = 0; iNode < 4; iNode++)
	{
		DrawQuadLine(pNode->m_ChildList[iNode]);
	}
	return true;
}

Sample::Sample()
{
	m_pMainCamera = nullptr;
	m_bDebugRender = true;
}
Sample::~Sample()
{

}
