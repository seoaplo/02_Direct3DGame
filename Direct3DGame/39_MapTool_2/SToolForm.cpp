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
, m_fDistance(0)
, m_fHeightValue(0)
{

}


SToolForm::~SToolForm()
{
}

void SToolForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, MapColl);
	DDX_Text(pDX, IDC_EDIT3, CellNumSize);
	DDX_Text(pDX, IDC_EDIT5, CellDistance);
	DDX_Text(pDX, IDC_EDIT2, m_fDistance);
	DDV_MinMaxInt(pDX, m_fDistance, 0, INT_MAX);
	DDX_Text(pDX, IDC_EDIT4, m_fHeightValue);
}


BEGIN_MESSAGE_MAP(SToolForm, CFormView)
//	ON_WM_USERCHANGED()
ON_BN_CLICKED(IDC_BUTTON1, &SToolForm::OnBnClickedButton1)
ON_BN_CLICKED(IDC_BUTTON2, &SToolForm::OnBnClickedButton2)
ON_BN_CLICKED(IDC_BUTTON3, &SToolForm::OnBnClickedButton3)
END_MESSAGE_MAP()



// SToolForm 메시지 처리기


void SToolForm::OnBnClickedButton1()
{
	UpdateData(TRUE);
	CMapToolApp* pApp = (CMapToolApp*)AfxGetApp();
	pApp->m_Tool.iColl = MapColl;
	// Node 당 Cell의 갯수
	pApp->m_Tool.iNumSize = CellNumSize;
	pApp->m_Tool.fDistance =  CellDistance;
	pApp->m_Tool.bCreateMap = true;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void SToolForm::OnBnClickedButton2()
{
	UpdateData(TRUE);
	CMapToolApp* pApp = (CMapToolApp*)AfxGetApp();
	pApp->m_Tool.m_fHeightDistance = m_fDistance;
	pApp->m_Tool.m_fHeightValue = m_fHeightValue;
	pApp->m_Tool.m_bHeightSet = true;
}


void SToolForm::OnBnClickedButton3()
{
	UpdateData(TRUE);
	CMapToolApp* pApp = (CMapToolApp*)AfxGetApp();
	pApp->m_Tool.m_bHeightSet = false;
}
