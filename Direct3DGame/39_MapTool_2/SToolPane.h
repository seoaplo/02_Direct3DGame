#pragma once
#include "SToolForm.h"

// SToolPane

class SToolPane : public CDockablePane
{
	DECLARE_DYNAMIC(SToolPane)
public:
	SToolForm* m_wndForm;
public:
	SToolPane();
	virtual ~SToolPane();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


