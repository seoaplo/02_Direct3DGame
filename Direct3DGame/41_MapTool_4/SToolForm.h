#pragma once



// SToolForm 보기

class SToolForm : public CFormView
{
	DECLARE_DYNCREATE(SToolForm)

public:
	SToolForm();   // 표준 생성자입니다.
	virtual ~SToolForm();
	static SToolForm*  CreateOne(CWnd* pParent);
	// 대화 상자 데이터입니다.

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SToolForm};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnUserChanged();
	afx_msg void OnBnClickedButton1();
	// Map의 Coll
	int MapColl;
	// Map의 Row
	int MapRow;
	// Node 당 Cell의 갯수
	int CellNumSize;
	float CellDistance;
	// 높이 설정 범위
	float m_fDistance;
	// 높이 설정 기준값
	float m_fHeightValue;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedButton4();
	// // SmoothSize
	DWORD m_dwSmooth;
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnBnClickedRadio6();
	afx_msg void OnBnClickedRadio7();
};


