#pragma once
#include "afxwin.h"


// CStatisticDlg 대화 상자입니다.

class CStatisticDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStatisticDlg)

public:
	CStatisticDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CStatisticDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
    CEdit m_statisticText;
    virtual BOOL OnInitDialog();
};
