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
, m_dwSmooth(0)
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
	DDX_Text(pDX, IDC_EDIT6, m_dwSmooth);
	DDV_MinMaxUInt(pDX, m_dwSmooth, 0, 100);
}


BEGIN_MESSAGE_MAP(SToolForm, CFormView)
//	ON_WM_USERCHANGED()
ON_BN_CLICKED(IDC_BUTTON1, &SToolForm::OnBnClickedButton1)
ON_BN_CLICKED(IDC_BUTTON2, &SToolForm::OnBnClickedButton2)
ON_BN_CLICKED(IDC_BUTTON3, &SToolForm::OnBnClickedButton3)
ON_BN_CLICKED(IDC_RADIO1, &SToolForm::OnBnClickedRadio1)
ON_BN_CLICKED(IDC_RADIO2, &SToolForm::OnBnClickedRadio2)
ON_BN_CLICKED(IDC_RADIO3, &SToolForm::OnBnClickedRadio3)
ON_BN_CLICKED(IDC_RADIO4, &SToolForm::OnBnClickedRadio4)
ON_BN_CLICKED(IDC_BUTTON4, &SToolForm::OnBnClickedButton4)
ON_BN_CLICKED(IDC_BUTTON7, &SToolForm::OnBnClickedButton7)
ON_BN_CLICKED(IDC_BUTTON6, &SToolForm::OnBnClickedButton6)
ON_BN_CLICKED(IDC_BUTTON5, &SToolForm::OnBnClickedButton5)
ON_BN_CLICKED(IDC_BUTTON8, &SToolForm::OnBnClickedButton8)
ON_BN_CLICKED(IDC_RADIO5, &SToolForm::OnBnClickedRadio5)
ON_BN_CLICKED(IDC_RADIO6, &SToolForm::OnBnClickedRadio6)
ON_BN_CLICKED(IDC_RADIO7, &SToolForm::OnBnClickedRadio7)
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
	pApp->m_Tool.m_fBrushSize = m_fDistance;
	pApp->m_Tool.m_fHeightValue = m_fHeightValue;
	pApp->m_Tool.m_bHeightSet = true;
	pApp->m_Tool.m_bSplatingSet = false;
}


void SToolForm::OnBnClickedButton3()
{
	CMapToolApp* pApp = (CMapToolApp*)AfxGetApp();
	pApp->m_Tool.m_bHeightSet = false;
	pApp->m_Tool.m_bSplatingSet = false;
}


void SToolForm::OnBnClickedRadio1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMapToolApp* pApp = (CMapToolApp*)AfxGetApp();
	pApp->m_Tool.m_AlphaVertex.c.x = 1.0f;
	pApp->m_Tool.m_AlphaVertex.c.y = 0.0f;
	pApp->m_Tool.m_AlphaVertex.c.z = 0.0f;
	pApp->m_Tool.m_AlphaVertex.c.w = 0.0f;

}


void SToolForm::OnBnClickedRadio2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMapToolApp* pApp = (CMapToolApp*)AfxGetApp();
	pApp->m_Tool.m_AlphaVertex.c.x = 0.0f;
	pApp->m_Tool.m_AlphaVertex.c.y = 1.0f;
	pApp->m_Tool.m_AlphaVertex.c.z = 0.0f;
	pApp->m_Tool.m_AlphaVertex.c.w = 0.0f;
}


void SToolForm::OnBnClickedRadio3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMapToolApp* pApp = (CMapToolApp*)AfxGetApp();
	pApp->m_Tool.m_AlphaVertex.c.x = 0.0f;
	pApp->m_Tool.m_AlphaVertex.c.y = 0.0f;
	pApp->m_Tool.m_AlphaVertex.c.z = 1.0f;
	pApp->m_Tool.m_AlphaVertex.c.w = 0.0f;
}


void SToolForm::OnBnClickedRadio4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMapToolApp* pApp = (CMapToolApp*)AfxGetApp();
	pApp->m_Tool.m_AlphaVertex.c.x = 0.0f;
	pApp->m_Tool.m_AlphaVertex.c.y = 0.0f;
	pApp->m_Tool.m_AlphaVertex.c.z = 0.0f;
	pApp->m_Tool.m_AlphaVertex.c.w = 1.0f;
}


void SToolForm::OnBnClickedButton4()
{
	UpdateData(TRUE);

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMapToolApp* pApp = (CMapToolApp*)AfxGetApp();
	pApp->m_Tool.m_fBrushSize = m_fDistance;
	pApp->m_Tool.m_fSmoothSize = m_dwSmooth * 0.01f;
	pApp->m_Tool.m_bHeightSet = false;
	pApp->m_Tool.m_bSplatingSet = true;
}

void SToolForm::OnBnClickedButton7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString selFileName;
	CFileDialog dlg(
		TRUE, L"dds", NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		  L"dds Files(*.dds)|*.dds| jpg Files(*.jpg)|*.jpg| All Files(*.*)|*.*|", this
	);

	if (dlg.DoModal() == IDOK)
	{
		selFileName = dlg.GetPathName();
		//selFileName += dlg.GetFileName();		
		//AfxMessageBox(selFileName);
		CMapToolApp* pApp = (CMapToolApp*)AfxGetApp();
		pApp->m_Tool.m_strAlphaTex2 = selFileName.GetBuffer();
		pApp->m_Tool.SetTexture();
	}
}


void SToolForm::OnBnClickedButton6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString selFileName;
	CFileDialog dlg(
		TRUE, L"dds", NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		  L"dds Files(*.dds)|*.dds| jpg Files(*.jpg)|*.jpg| All Files(*.*)|*.*|", this
	);

	if (dlg.DoModal() == IDOK)
	{
		selFileName = dlg.GetPathName();
		//selFileName += dlg.GetFileName();		
		//AfxMessageBox(selFileName);
		CMapToolApp* pApp = (CMapToolApp*)AfxGetApp();
		pApp->m_Tool.m_strAlphaTex1 = selFileName.GetBuffer();
		pApp->m_Tool.SetTexture();
	}
}


void SToolForm::OnBnClickedButton5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString selFileName;
	CFileDialog dlg(
		TRUE, L"dds", NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		  L"dds Files(*.dds)|*.dds| jpg Files(*.jpg)|*.jpg| All Files(*.*)|*.*|", this
	);

	if (dlg.DoModal() == IDOK)
	{
		selFileName = dlg.GetPathName();
		//selFileName += dlg.GetFileName();		
		//AfxMessageBox(selFileName);
		CMapToolApp* pApp = (CMapToolApp*)AfxGetApp();
		pApp->m_Tool.m_strAlphaTex0 = selFileName.GetBuffer();
		pApp->m_Tool.SetTexture();
	}
}


void SToolForm::OnBnClickedButton8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString selFileName;
	CFileDialog dlg(
		TRUE, L"dds", NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		L"dds Files(*.dds)|*.dds| jpg Files(*.jpg)|*.jpg| All Files(*.*)|*.*|", this
	);

	if (dlg.DoModal() == IDOK)
	{
		selFileName = dlg.GetPathName();
		//selFileName += dlg.GetFileName();		
		//AfxMessageBox(selFileName);
		CMapToolApp* pApp = (CMapToolApp*)AfxGetApp();
		pApp->m_Tool.m_strBaseTexture = selFileName.GetBuffer();
		pApp->m_Tool.SetTexture();
	}
}


void SToolForm::OnBnClickedRadio5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// Up

	UpdateData(TRUE);

	CMapToolApp* pApp = (CMapToolApp*)AfxGetApp();
	pApp->m_Tool.m_dwHeightMode = QUADTREEMAP_HEIGHT_UP;
}


void SToolForm::OnBnClickedRadio6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// Down
	CMapToolApp* pApp = (CMapToolApp*)AfxGetApp();
	pApp->m_Tool.m_dwHeightMode = QUADTREEMAP_HEIGHT_DOWN;
}


void SToolForm::OnBnClickedRadio7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// flat
	CMapToolApp* pApp = (CMapToolApp*)AfxGetApp();
	pApp->m_Tool.m_dwHeightMode = QUADTREEMAP_HEIGHT_FLAT;
}
