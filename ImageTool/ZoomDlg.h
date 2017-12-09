#pragma once
#include "afxwin.h"


// CZoomDlg 대화 상자입니다.

class CZoomDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CZoomDlg)

public:
	CZoomDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CZoomDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_ZOOM };

    CMFCToolBar m_ToolBar;
    CMFCButton m_button;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMove(int x, int y);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void OnClickedBtnFilepath();
    afx_msg void OnDrawLine();
    afx_msg void OnDrawRectangle();
    afx_msg void OnDrawEllipse();
    afx_msg void OnDrawCircle();
};
