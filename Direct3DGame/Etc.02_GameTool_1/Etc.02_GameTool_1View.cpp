﻿
// Etc.02_GameTool_1View.cpp: CEtc02GameTool1View 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Etc.02_GameTool_1.h"
#endif

#include "Etc.02_GameTool_1Doc.h"
#include "Etc.02_GameTool_1View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEtc02GameTool1View

IMPLEMENT_DYNCREATE(CEtc02GameTool1View, CView)

BEGIN_MESSAGE_MAP(CEtc02GameTool1View, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CEtc02GameTool1View::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CEtc02GameTool1View 생성/소멸

CEtc02GameTool1View::CEtc02GameTool1View() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CEtc02GameTool1View::~CEtc02GameTool1View()
{
}

BOOL CEtc02GameTool1View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CEtc02GameTool1View 그리기

void CEtc02GameTool1View::OnDraw(CDC* /*pDC*/)
{
	CEtc02GameTool1Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


void CEtc02GameTool1View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

void CEtc02GameTool1View::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CView::OnBeginPrinting(pDC, pInfo);
}


BOOL CEtc02GameTool1View::OnPreparePrinting(CPrintInfo* pInfo)
{

	// TODO:  [인쇄] 대화 상자를 호출하는 DoPreparePrinting을 호출합니다.

	return DoPreparePrinting(pInfo);
}


void CEtc02GameTool1View::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CView::OnEndPrinting(pDC, pInfo);
}


void CEtc02GameTool1View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CEtc02GameTool1View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CEtc02GameTool1View 진단

#ifdef _DEBUG
void CEtc02GameTool1View::AssertValid() const
{
	CView::AssertValid();
}

void CEtc02GameTool1View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEtc02GameTool1Doc* CEtc02GameTool1View::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEtc02GameTool1Doc)));
	return (CEtc02GameTool1Doc*)m_pDocument;
}
#endif //_DEBUG


// CEtc02GameTool1View 메시지 처리기

void CEtc02GameTool1View::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CEtc02GameTool1App* pApp = (CEtc02GameTool1App*)AfxGetApp();
	pApp->m_Tool.ReSizeDevice(cx, cy);
}

LRESULT CEtc02GameTool1View::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CEtc02GameTool1App* pApp = (CEtc02GameTool1App*)AfxGetApp();
	HWND hWnd = pApp->m_Tool.m_hWnd;
	pApp->m_Tool.WindowProc(hWnd, message, wParam, lParam);
	return CView::WindowProc(message, wParam, lParam);
}
