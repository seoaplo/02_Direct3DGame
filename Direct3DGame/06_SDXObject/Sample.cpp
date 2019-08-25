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
	TestObject.Init();
	TestObject.SetDevice(GetDevice(), m_pImmediateContext);
	TestObject.LoadShaderAndInputlayout("VS", "PS");
	PCT_Vertex vertices1[] =
	{
		-1.0f, 1.0f, 0.5f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.5f, 1.0f, 1.0f,
		-1.0f, -1.0f, 0.5f, 0.0f, 1.0f,
	};
	int iNumCount = sizeof(vertices1) / sizeof(vertices1[0]);
	if (TestObject.CreateVertexBuffer(
		vertices1,
		iNumCount,
		sizeof(PCT_Vertex)) == false)
	{
		return false;
	}

	DWORD indexlist[] =
	{
		1,2,0,
		2,3,0,
	};
	iNumCount = sizeof(indexlist) / sizeof(indexlist[0]);
	if (TestObject.CreateIndexBuffer(
		indexlist, iNumCount, sizeof(DWORD)) == false) return false;

	VSCB vc;
	ZeroMemory(&vc, sizeof(VSCB));
	vc.color[0] = 1.0f;
	vc.etc[0] = 0.0f;
	iNumCount = 1;
	if (TestObject.CreateConstantBuffer(
		(LPVOID)&vc, iNumCount, sizeof(VSCB), true) == false) return false;


	m_TexList.resize(1);
	m_TexList[0].Load(GetDevice(), L"../../data/Image/Attacker3.png");
	TestObject.m_dxObj.m_pSRV.Attach(m_TexList[0].m_pSRV);

	//레스터라이즈 상태 객체 생성
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_NONE;
	if (FAILED(hr =
		GetDevice()->CreateRasterizerState(&rsDesc, &m_pRSWireFrame)))
	{
		return false;
	}
	rsDesc.FillMode = D3D11_FILL_SOLID;
	if (FAILED(hr =
		GetDevice()->CreateRasterizerState(&rsDesc, &m_pRSSolidFrame)))
	{
		return false;
	}
	return true;
}

bool Sample::Frame()
{
	
	if (I_InputManager.m_pMouseInput->ButtonCheck(0) == KEY_PUSH)
	{
		m_bWireFrameRender = !m_bWireFrameRender;
	}
	TestObject.Frame();
	return true;
}
bool Sample::Render()
{
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if (m_bWireFrameRender)
	{
		m_pImmediateContext->RSSetState(m_pRSWireFrame);
	}
	else
	{
		m_pImmediateContext->RSSetState(m_pRSSolidFrame);
	}
	TestObject.Render(m_pImmediateContext, sizeof(PCT_Vertex), 0);
	return true;
}

bool Sample::Release()
{
	if (m_pRSWireFrame)m_pRSWireFrame->Release();
	if (m_pRSSolidFrame)m_pRSSolidFrame->Release();
	m_pRSWireFrame = 0;
	m_pRSSolidFrame = 0;
	TestObject.Release();
	return true;
}

Sample::Sample()
{
	m_pRSWireFrame = 0;
	m_pRSSolidFrame = 0;
	m_bWireFrameRender = false;
}


Sample::~Sample()
{
}
