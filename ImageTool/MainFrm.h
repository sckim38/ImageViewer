// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__6FF8C1FF_7D6C_4761_9D4F_F73B92B46A48__INCLUDED_)
#define AFX_MAINFRM_H__6FF8C1FF_7D6C_4761_9D4F_F73B92B46A48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ZoomDlg.h"
#include "ThumbnailView.h"

class CMainFrame : public CFrameWndEx
{

protected:
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

    BOOL CreateZoomDlg();
    void ShowZoomDlg(int nCmdShow);
    void RecalcDialog();

    CZoomDlg				m_ZoomDlg;
    CThumbnailView          m_wndThumbnailView;
    CMagnifyView            m_wndMagnifyView;

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CStatusBar      m_wndStatusBar;
	CMFCToolBar     m_wndToolBar;
	BOOL            m_bShowPreviewBar,
		            m_bShowDirectoryBar;

	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewDirectoryBar();
	afx_msg void OnUpdateViewDirectoryBar(CCmdUI* pCmdUI);
	afx_msg void OnViewPreviewBar();
	afx_msg void OnUpdateViewPreviewBar(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnMove(int x, int y);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDrawLine();
    afx_msg void OnUpdateDrawLine(CCmdUI *pCmdUI);
    afx_msg void OnDrawRectangle();
    afx_msg void OnUpdateDrawRectangle(CCmdUI *pCmdUI);
    afx_msg void OnDrawEllipse();
    afx_msg void OnUpdateDrawEllipse(CCmdUI *pCmdUI);
    afx_msg void OnDrawCircle();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBtnFilepath();
    afx_msg void OnDestroy();
    afx_msg void OnFileRestart();
    afx_msg void OnMapSelectunit();
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_MAINFRM_H__6FF8C1FF_7D6C_4761_9D4F_F73B92B46A48__INCLUDED_)
