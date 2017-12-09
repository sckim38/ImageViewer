
// IISExplorerView.h : CIISExplorerView Ŭ������ �������̽�
//

#pragma once


class CIISExplorerView : public CListView
{
protected: // serialization������ ��������ϴ�.
	CIISExplorerView();
	DECLARE_DYNCREATE(CIISExplorerView)

// Ư���Դϴ�.
public:
	CIISExplorerDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ���� �� ó�� ȣ��Ǿ����ϴ�.

// �����Դϴ�.
public:
	virtual ~CIISExplorerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // IISExplorerView.cpp�� ����� ����
inline CIISExplorerDoc* CIISExplorerView::GetDocument() const
   { return reinterpret_cast<CIISExplorerDoc*>(m_pDocument); }
#endif

