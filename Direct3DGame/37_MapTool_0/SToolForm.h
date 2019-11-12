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
	afx_msg void OnEnChangeEdit5();
};


