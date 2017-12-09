// PreviewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageTool.h"
#include "PreviewDlg.h"
#include "MainFrm.h"
#include "ImageToolDoc.h"
//#include "memdc.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>  

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG  

/////////////////////////////////////////////////////////////////////////////
// CPreviewDlg
CPreviewDlg::CPreviewDlg()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
}

CPreviewDlg::~CPreviewDlg()
{
}

BEGIN_MESSAGE_MAP(CPreviewDlg, CDialog)
	//{{AFX_MSG_MAP(CPreviewDlg)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreviewDlg message handlers
int CPreviewDlg::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if( CDialog::OnCreate( lpCreateStruct ) == -1 )
	{
		return -1;
	}
	
    CRect rect; GetClientRect(&rect);

    m_wndCanvas.m_layout = GridLayout(2, 2);
    return m_wndCanvas.CreateEx(0, 0, _T("ImageView"), WS_CHILD | WS_VISIBLE, rect, this, (UINT)-1);
}

void CPreviewDlg::OnSize( UINT nType, int cx, int cy ) 
{
	CDialog::OnSize(nType, cx, cy);
	CRect rc; GetClientRect( rc );
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if( pFrame == NULL )
	{
		return;
	}

    m_wndCanvas.MoveWindow(rc);
}



BOOL CPreviewDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

    return TRUE;// CDialog::OnEraseBkgnd(pDC);
}

