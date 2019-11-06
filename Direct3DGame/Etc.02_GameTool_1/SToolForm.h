#pragma once



// SToolForm 보기

class SToolForm : public CFormView
{
	DECLARE_DYNCREATE(SToolForm)

protected:
	SToolForm();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~SToolForm();

public:
	SUIForm();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TUIForm };
#endif

protected:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual void OnInitialUpdate();     // 생성된 후 처음입니다.

	DECLARE_MESSAGE_MAP()
};


