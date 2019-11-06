// SToolForm.cpp: 구현 파일
//

#include "stdafx.h"
#include "Etc.02_GameTool_1.h"
#include "SToolForm.h"


// SToolForm

IMPLEMENT_DYNCREATE(SToolForm, CFormView)

SToolForm::SToolForm()
{

}

SToolForm::~SToolForm()
{
}


BEGIN_MESSAGE_MAP(SToolForm, CFormView)
END_MESSAGE_MAP()


// SToolForm 그리기

void SToolForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 이 뷰의 전체 크기를 계산합니다.
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void SToolForm::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// SToolForm 진단

#ifdef _DEBUG
void SToolForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void SToolForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// SToolForm 메시지 처리기
