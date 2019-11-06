
// Etc.01_GameToolView.cpp: CEtc01GameToolView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Etc.01_GameTool.h"
#endif

#include "Etc.01_GameToolDoc.h"
#include "Etc.01_GameToolView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEtc01GameToolView

IMPLEMENT_DYNCREATE(CEtc01GameToolView, CView)

BEGIN_MESSAGE_MAP(CEtc01GameToolView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CEtc01GameToolView 생성/소멸

CEtc01GameToolView::CEtc01GameToolView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CEtc01GameToolView::~CEtc01GameToolView()
{
}

BOOL CEtc01GameToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CEtc01GameToolView 그리기

void CEtc01GameToolView::OnDraw(CDC* /*pDC*/)
{
	CEtc01GameToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CEtc01GameToolView 인쇄

BOOL CEtc01GameToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CEtc01GameToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CEtc01GameToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CEtc01GameToolView 진단

#ifdef _DEBUG
void CEtc01GameToolView::AssertValid() const
{
	CView::AssertValid();
}

void CEtc01GameToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEtc01GameToolDoc* CEtc01GameToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEtc01GameToolDoc)));
	return (CEtc01GameToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CEtc01GameToolView 메시지 처리기
