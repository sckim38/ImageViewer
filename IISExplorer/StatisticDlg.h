#pragma once
#include "afxwin.h"


// CStatisticDlg ��ȭ �����Դϴ�.

class CStatisticDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStatisticDlg)

public:
	CStatisticDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CStatisticDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
    CEdit m_statisticText;
    virtual BOOL OnInitDialog();
};
