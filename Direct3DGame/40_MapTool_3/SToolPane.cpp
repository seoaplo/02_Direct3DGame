// SToolPane.cpp: 구현 파일
//

#include "stdafx.h"
#include "CMapTool.h"
#include "SToolPane.h"


// SToolPane

IMPLEMENT_DYNAMIC(SToolPane, CDockablePane)

SToolPane::SToolPane()
{

}

SToolPane::~SToolPane()
{
}


BEGIN_MESSAGE_MAP(SToolPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// SToolPane 메시지 처리기




int SToolPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_wndForm = SToolForm::CreateOne(this);
	return 0;
}


void SToolPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_wndForm)
	{
		m_wndForm->SetWindowPos(nullptr, 0, 0, cx, cy, SWP_NOZORDER);
	}
}

int SToolPane::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CFrameWnd* pParentFrame = GetParentFrame();
	if (pParentFrame == pDesktopWnd ||
		pDesktopWnd->IsChild(pParentFrame))
	{
		return CDockablePane::OnMouseActivate(pDesktopWnd, nHitTest, message);
	}
	return MA_NOACTIVATE;
}

