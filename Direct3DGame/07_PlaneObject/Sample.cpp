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
	RECT rcSize;



	ZeroMemory(&rcSize, sizeof(rcSize));
	TestObject.Set(GetDevice(), m_pImmediateContext, Vector, m_rcClientRect);
	TestObject.Load(GetDevice(), L"");

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
