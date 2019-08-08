#include "SWindow.h"

SWindow* g_pWindow;


#pragma region WINDOWPROC
LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	assert(g_pWindow);
	return g_pWindow->WndProc(hWnd, Message, wParam, lParam);
}

int SWindow::WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return -1;
}

LRESULT SWindow::WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	int iRet = WindowProc( hWnd, Message, wParam, lParam);
	if (iRet >= 0)
	{
		return iRet;
	}

	switch (Message)
	{
		case WM_SIZE:
		{
			if (SIZE_MINIMIZED != wParam)
			{
				UINT width = (LOWORD(lParam));
				UINT height = (HIWORD(lParam));
				ReSizeDevice(width, height);
				m_nWindowHeight = height;
				m_nWindowWidth = width;
				GetClientRect(hWnd, &m_rcClientRect);
				GetWindowRect(hWnd, &m_rcWindowBounds);
			}
		}
		break;
		case WM_CLOSE:
		{
			HMENU hMenu;
			hMenu = GetMenu(hWnd);
			if (hMenu != NULL)
			{
				DestroyMenu(hMenu);
			}
			DestroyWindow(hWnd);
			UnregisterClass(m_lpszClientName, NULL);
		}
		return 0;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;
	}
	return DefWindowProc(hWnd, Message, wParam, lParam);
}
#pragma endregion

bool SWindow::InitWindow(HINSTANCE hInstance, int nCmdShow, TCHAR* lpszClientName, RECT ClientRect, BOOL IsFullScreen)
{
	// 윈도우 클래스 구조체 초기화
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));

	// 윈도우 클래스 구조체 설정
	wcex.cbSize = sizeof(wcex);
	wcex.hInstance = hInstance;
	wcex.hbrBackground = (HBRUSH)COLOR_BACKGROUND + 1;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.lpfnWndProc = StaticWndProc;
	wcex.lpszClassName = lpszClientName;
	wcex.lpszMenuName = lpszClientName;
	wcex.style = CS_HREDRAW | CS_VREDRAW;

	// 윈도우 등록
	if (!RegisterClassEx(&wcex))
	{
		return false;
	}

	// 윈도우 크기를 얻는다
	AdjustWindowRect(&ClientRect, WS_OVERLAPPEDWINDOW, FALSE);

	// 윈도우 생성
	m_hWnd = CreateWindowEx(WS_EX_TOPMOST, lpszClientName, lpszClientName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top,
		NULL, NULL, m_hInstance, NULL);

	if (!m_hWnd)
		return false;

	m_hInstance = hInstance;
	// Save window properties
	m_dwWindowStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	GetWindowRect(m_hWnd, &m_rcWindowBounds);
	GetClientRect(m_hWnd, &m_rcClientRect);

	CenterWindow(m_hWnd);
	UpdateWindow(m_hWnd);

	m_nWindowWidth = m_rcClientRect.right - m_rcClientRect.left;
	m_nWindowHeight = m_rcClientRect.bottom - m_rcClientRect.top;

	ShowWindow(m_hWnd, nCmdShow);

	return true;

}

void SWindow::CenterWindow(HWND hWnd)
{
	// Get the Width and Height of th Screen
	int iScreenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
	int iScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN);

	// do the centering math
	int iDestX = (iScreenWidth - (m_rcWindowBounds.right - m_rcWindowBounds.left)) / 2;
	int iDestY = (iScreenHeight - (m_rcWindowBounds.bottom - m_rcWindowBounds.top)) / 2;

	// center the window
	MoveWindow(hWnd, iDestX, iDestY,
			m_rcWindowBounds.right - m_rcWindowBounds.left, m_rcWindowBounds.bottom - m_rcWindowBounds.top,
			true);
}

bool SWindow::Run()
{
	if (!Init())
	{
		return false;
	}
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, NULL, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (!Frame() || !Render())
			{
				break;
			}
		}
	}
	if (!Release())
	{
		return false;
	}
	return true;
}

SWindow::SWindow()
{
	m_hWnd = NULL;
	m_hInstance = NULL;
	g_pWindow = this;
}


SWindow::~SWindow()
{
}
