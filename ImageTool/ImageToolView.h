// ImageToolView.h : interface of the CImageToolView class
//

#include "FoldersDlg.h"
#include "PreviewDlg.h"

#if !defined(AFX_IMAGETOOLVIEW_H__562766B1_E825_48B8_A821_EDF664032AF4__INCLUDED_)
#define AFX_IMAGETOOLVIEW_H__562766B1_E825_48B8_A821_EDF664032AF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CImageToolDoc;

class CImageToolView : public CListView
{
protected: // create from serialization only
	CImageToolView();
	DECLARE_DYNCREATE(CImageToolView)

public:
	CImageToolDoc *GetDocument();
	CFoldersDlg   m_foldersDlg;
	CPreviewDlg   m_previewDlg;
	CImageList    m_ImageListThumb;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageToolView)
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

public:
    void LoadThumbnail2(int nIndex, const CString &path);

	void LoadThumbnailImages();
	virtual ~CImageToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DWORD m_dwThreadID;

	//{{AFX_MSG(CImageToolView)
	//}}AFX_MSG
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnDestroy();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

#ifndef _DEBUG  // debug version in ImageToolView.cpp
inline CImageToolDoc* CImageToolView::GetDocument()
   { return (CImageToolDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_IMAGETOOLVIEW_H__562766B1_E825_48B8_A821_EDF664032AF4__INCLUDED_)
