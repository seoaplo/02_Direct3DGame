// SToolForm.cpp: 구현 파일
//

#include "stdafx.h"
#include "CMapTool.h"
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
, MapColl(0)
, MapRow(0)
, CellNumSize(0)
, CellDistance(0)
{

}


SToolForm::~SToolForm()
{
}

void SToolForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, MapColl);
	DDX_Text(pDX, IDC_EDIT2, MapRow);
	DDX_Text(pDX, IDC_EDIT3, CellNumSize);
	DDX_Text(pDX, IDC_EDIT5, CellDistance);
}


BEGIN_MESSAGE_MAP(SToolForm, CFormView)
//	ON_WM_USERCHANGED()
ON_BN_CLICKED(IDC_BUTTON1, &SToolForm::OnBnClickedButton1)
ON_EN_CHANGE(IDC_EDIT5, &SToolForm::OnEnChangeEdit5)
END_MESSAGE_MAP()



// SToolForm 메시지 처리기


void SToolForm::OnBnClickedButton1()
{
	UpdateData(TRUE);
	CMapToolApp* pApp = (CMapToolApp*)AfxGetApp();
	pApp->m_Tool.iColl = MapColl;
	// Map의 Row
	pApp->m_Tool.iRow = MapRow;
	// Node 당 Cell의 갯수
	pApp->m_Tool.iNumSize = CellNumSize;
	pApp->m_Tool.fDistance =  CellDistance;
	pApp->m_Tool.bCreateMap = true;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void SToolForm::OnEnChangeEdit5()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
