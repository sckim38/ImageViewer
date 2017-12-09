
// IISExplorerView.cpp : CIISExplorerView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
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

// CIISExplorerView 생성/소멸

CIISExplorerView::CIISExplorerView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CIISExplorerView::~CIISExplorerView()
{
}

BOOL CIISExplorerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CListView::PreCreateWindow(cs);
}

void CIISExplorerView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	// TODO: GetListCtrl()을 호출하여 해당 list 컨트롤을 직접 액세스함으로써
	//  ListView를 항목으로 채울 수 있습니다.
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


// CIISExplorerView 진단

#ifdef _DEBUG
void CIISExplorerView::AssertValid() const
{
	CListView::AssertValid();
}

void CIISExplorerView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CIISExplorerDoc* CIISExplorerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIISExplorerDoc)));
	return (CIISExplorerDoc*)m_pDocument;
}
#endif //_DEBUG


// CIISExplorerView 메시지 처리기
