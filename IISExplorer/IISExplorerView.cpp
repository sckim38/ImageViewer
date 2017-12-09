
// IISExplorerView.cpp : CIISExplorerView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "IISExplorer.h"
#endif

#include "IISExplorerDoc.h"
#include "IISExplorerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIISExplorerView

IMPLEMENT_DYNCREATE(CIISExplorerView, CListView)

BEGIN_MESSAGE_MAP(CIISExplorerView, CListView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CIISExplorerView ����/�Ҹ�

CIISExplorerView::CIISExplorerView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CIISExplorerView::~CIISExplorerView()
{
}

BOOL CIISExplorerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CListView::PreCreateWindow(cs);
}

void CIISExplorerView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	// TODO: GetListCtrl()�� ȣ���Ͽ� �ش� list ��Ʈ���� ���� �׼��������ν�
	//  ListView�� �׸����� ä�� �� �ֽ��ϴ�.
}

void CIISExplorerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CIISExplorerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CIISExplorerView ����

#ifdef _DEBUG
void CIISExplorerView::AssertValid() const
{
	CListView::AssertValid();
}

void CIISExplorerView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CIISExplorerDoc* CIISExplorerView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIISExplorerDoc)));
	return (CIISExplorerDoc*)m_pDocument;
}
#endif //_DEBUG


// CIISExplorerView �޽��� ó����
