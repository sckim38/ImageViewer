#pragma once
#include "afxwin.h"


// CZoomDlg ��ȭ �����Դϴ�.

class CZoomDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CZoomDlg)

public:
	CZoomDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CZoomDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_ZOOM };

    CMFCToolBar m_ToolBar;
    CMFCButton m_button;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
