#if !defined(AFX_PreviewDlg_H__426DCEB6_0A8E_4609_B433_2675C303E9DA__INCLUDED_)
#define AFX_PreviewDlg_H__426DCEB6_0A8E_4609_B433_2675C303E9DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreviewDlg.h : header file
//

#include "ImageView.h"

/////////////////////////////////////////////////////////////////////////////
// CPreviewDlg window

class CPreviewDlg : public CDialog
{
// Construction
public:
	CPreviewDlg();

    CImageView m_wndCanvas;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreviewDlg)
	//}}AFX_VIRTUAL

	virtual ~CPreviewDlg();

protected:

	//{{AFX_MSG(CPreviewDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_PreviewDlg_H__426DCEB6_0A8E_4609_B433_2675C303E9DA__INCLUDED_)
