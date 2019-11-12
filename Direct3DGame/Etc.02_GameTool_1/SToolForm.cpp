// SToolForm.cpp: 구현 파일
//

#include "stdafx.h"
#include "Etc.02_GameTool_1.h"
#include "SToolForm.h"
#include "afxdialogex.h"

// SToolForm

IMPLEMENT_DYNCREATE(SToolForm, CFormView)

SToolForm*  SToolForm::CreateOne(CWnd* pParent)
{
	SToolForm* pForm = new SToolForm;
	pForm->Create(NULL, NULL, WS_CHILD | WS_VISIBLE,
		CRect(0, 0, 500, 500), pParent, 0, NULL);
	return pForm;
}

SToolForm::SToolForm() : CFormView(IDD_SToolForm)
{

}


SToolForm::~SToolForm()
{
}

void SToolForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SToolForm, CFormView)
//	ON_WM_USERCHANGED()
END_MESSAGE_MAP()



// SToolForm 메시지 처리기

