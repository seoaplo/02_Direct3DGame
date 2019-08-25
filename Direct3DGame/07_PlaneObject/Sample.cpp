#include "Sample.h"
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Sample g_Sample;
	RECT rc{ 0, 0, 800, 600 };
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
	RECT rcSize;

	DXGame::SDXState::SetState(GetDevice());

	ZeroMemory(&rcSize, sizeof(rcSize));
	TestObject.Set(GetDevice(), m_pImmediateContext, Vector, rcSize);
	TestObject.Load(GetDevice(), L"");

	DXGame:: SVector3 vX(1, 0, 0);
	DXGame::SVector3 vY(0, 1, 0);
	DXGame::SVector3 vZ(0, 0, 1);

	float fDot = vX | vY;
	DXGame::SVector3 vNormal = vX ^ vY;

	DXGame::SVector3 vT(0, 10, 10);
	float fsize = vT.Length();
	DXGame::SVector3 vS = vT;
	vS.Nomalize();
	return true;
}
bool Sample::Frame()
{
	TestObject.Frame();
	return true;
}
bool Sample::Render()
{
	TestObject.Render(m_pImmediateContext);

	//m_Obj[0].m_dxObj.PreRender(m_pImmediateContext, sizeof(PT_VERTEX));
	//I_TextureMgr.Apply(m_pImmediateContext, m_Tex[0], 0);
	//I_TextureMgr.Apply(m_pImmediateContext, m_Tex[1], 1);
	//m_Tex[0].Apply(m_pImmediateContext, 0);
	//m_Tex[1].Apply(m_pImmediateContext, 0);
	//m_Obj[0].m_dxObj.PostRender(m_pImmediateContext, 0);
	return true;
}
bool Sample::Release()
{
	TestObject.Release();
	return true;
}
Sample::Sample()
{
}
Sample::~Sample()
{

}
