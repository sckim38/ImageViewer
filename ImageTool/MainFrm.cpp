// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ImageTool.h"
#include "MainFrm.h"
#include "ImageToolView.h"
#include "SelectMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE(                                                         )
	ON_COMMAND(           ID_VIEW_DIRECTORY_BAR, OnViewDirectoryBar       )
	ON_UPDATE_COMMAND_UI( ID_VIEW_DIRECTORY_BAR, OnUpdateViewDirectoryBar )
	ON_COMMAND(           ID_VIEW_PREVIEW_BAR,   OnViewPreviewBar         )
	ON_UPDATE_COMMAND_UI( ID_VIEW_PREVIEW_BAR,   OnUpdateViewPreviewBar   )
	ON_WM_CLOSE(                                                          )
	//}}AFX_MSG_MAP
    ON_WM_MOVE()
    ON_WM_SIZE()
    ON_COMMAND(ID_DRAW_LINE, &CMainFrame::OnDrawLine)
    ON_COMMAND(ID_DRAW_RECTANGLE, &CMainFrame::OnDrawRectangle)
    ON_COMMAND(ID_DRAW_ELLIPSE, &CMainFrame::OnDrawEllipse)
    ON_COMMAND(ID_DRAW_CIRCLE, &CMainFrame::OnDrawCircle)
    ON_UPDATE_COMMAND_UI(ID_DRAW_LINE, &CMainFrame::OnUpdateDrawLine)
    ON_UPDATE_COMMAND_UI(ID_DRAW_RECTANGLE, &CMainFrame::OnUpdateDrawRectangle)
    ON_UPDATE_COMMAND_UI(ID_DRAW_ELLIPSE, &CMainFrame::OnUpdateDrawEllipse)
    ON_COMMAND(IDC_BTN_FILEPATH, &CMainFrame::OnBtnFilepath)
    ON_WM_DESTROY()
    ON_COMMAND(ID_FILE_RESTART, &CMainFrame::OnFileRestart)
    ON_COMMAND(ID_MAP_SELECTUNIT, &CMainFrame::OnMapSelectunit)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction
CMainFrame::CMainFrame()
{
	m_bShowPreviewBar = m_bShowDirectoryBar = TRUE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( CFrameWndEx::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}

    if (!m_wndThumbnailView.CreateEx(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        nullptr,
        _T("Thumbnail View"),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CRect{ POINT{ 1020, 100 }, SIZE{ 300, 300 } },
        nullptr, 0))
        return -1;


    if (!m_wndMagnifyView.CreateEx(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        nullptr,
        _T("Magnify View"),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CRect(POINT{ 1020, 410 }, SIZE{ 300, 300 }),
        nullptr,
        0))
        return -1;

    m_wndMagnifyView.m_pThumbnailView = &m_wndThumbnailView;
    m_wndThumbnailView.m_pMagnifyView = &m_wndMagnifyView;

	if( !m_wndToolBar.CreateEx( this,
		                        TBSTYLE_FLAT,
								WS_CHILD      | 
								WS_VISIBLE    |
								CBRS_TOP      |
								CBRS_GRIPPER  | 
								CBRS_TOOLTIPS |
								CBRS_FLYBY    | 
								CBRS_SIZE_DYNAMIC )
		|| !m_wndToolBar.LoadToolBar( IDR_MAINFRAME ) 
	  )
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}

	if( !m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators( indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1; 
	}

    if (!CreateZoomDlg())
    {
        TRACE0("Failed to create zoom dialog\n");
        return -1;      // 만들지 못했습니다.
    }

	m_wndToolBar.EnableDocking( CBRS_ALIGN_ANY );
	EnableDocking( CBRS_ALIGN_ANY );
    DockPane(&m_wndToolBar);

    CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
    CDockingManager::SetDockingMode(DT_SMART);

	return 0;
}

BOOL CMainFrame::PreCreateWindow( CREATESTRUCT& cs )
{
	cs.style &= ~FWS_ADDTOTITLE;
	cs.cx = 480;
	cs.cy = 740;
	cs.x  = 530;
	cs.y  = 10;

	if( !CFrameWndEx::PreCreateWindow( cs ) )
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CMainFrame::CreateZoomDlg()
{
    if (!m_ZoomDlg.Create(CZoomDlg::IDD, this))
    {
        TRACE0("Failed to create zoom control dialog\n");
        return FALSE;
    }
    m_ZoomDlg.ShowWindow(SW_SHOW);

    return TRUE;
}

void CMainFrame::ShowZoomDlg(int nCmdShow)
{
    if (m_ZoomDlg)
        m_ZoomDlg.ShowWindow(nCmdShow);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnViewDirectoryBar() 
{
	CImageToolView* pView = (CImageToolView*)GetActiveView();

	if( m_bShowDirectoryBar )
	{
		::ShowWindow( pView->m_foldersDlg.m_hWnd, SW_HIDE );			
		m_bShowDirectoryBar = FALSE;

		RecalcLayout( TRUE );
	}
	else
	{
		::ShowWindow( pView->m_foldersDlg.m_hWnd, SW_SHOW );
		RecalcLayout( TRUE );
		m_bShowDirectoryBar = TRUE;
	}
}

void CMainFrame::OnUpdateViewDirectoryBar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_bShowDirectoryBar );
}

void CMainFrame::OnViewPreviewBar() 
{
	CImageToolView* pView = (CImageToolView*)GetActiveView();

	if( m_bShowPreviewBar )
	{
		::ShowWindow( pView->m_previewDlg.m_hWnd, SW_HIDE );			
		m_bShowPreviewBar = FALSE;

		RecalcLayout( TRUE );
	}
	else
	{
		::ShowWindow( pView->m_previewDlg.m_hWnd, SW_SHOW );
		RecalcLayout( TRUE );
		m_bShowPreviewBar = TRUE;
	}
}

void CMainFrame::OnUpdateViewPreviewBar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_bShowPreviewBar );	
}

void CMainFrame::OnClose() 
{
	CImageToolView* pView = (CImageToolView*)GetActiveView();

    CFrameWndEx::OnClose();
}


void CMainFrame::RecalcDialog()
{
    if (!GetActiveView())
        return;

    CRect rc;
    GetActiveView()->GetWindowRect(rc);
    if (m_ZoomDlg.GetSafeHwnd())
        m_ZoomDlg.SetWindowPos(nullptr, rc.left + 10, rc.top + 10, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}


void CMainFrame::OnMove(int x, int y)
{
    CFrameWndEx::OnMove(x, y);

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
    RecalcDialog();
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
    CFrameWndEx::OnSize(nType, cx, cy);

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
    RecalcDialog();
}


void CMainFrame::OnDrawLine()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
    //TRACE(_T("abstractFactoryTest();\n"));
    //abstractFactoryTest();
    //AfxMessageBox(_T("CMainFrame::OnDrawLine()"));
    CImageToolView* pView = (CImageToolView*)GetActiveView();
    pView->m_previewDlg.m_wndCanvas.SetDrawObject(DrawShape::Line);
}


void CMainFrame::OnUpdateDrawLine(CCmdUI *pCmdUI)
{
    // TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}


void CMainFrame::OnDrawRectangle()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
    CImageToolView* pView = (CImageToolView*)GetActiveView();
    pView->m_previewDlg.m_wndCanvas.SetDrawObject(DrawShape::Rectangle);
}


void CMainFrame::OnUpdateDrawRectangle(CCmdUI *pCmdUI)
{
    // TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}


void CMainFrame::OnDrawEllipse()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
    CImageToolView* pView = (CImageToolView*)GetActiveView();
    pView->m_previewDlg.m_wndCanvas.SetDrawObject(DrawShape::Ellipse);
}


void CMainFrame::OnUpdateDrawEllipse(CCmdUI *pCmdUI)
{
    // TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}

void CMainFrame::OnDrawCircle()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
    CImageToolView* pView = (CImageToolView*)GetActiveView();
    pView->m_previewDlg.m_wndCanvas.SetDrawObject(DrawShape::Circle);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    if (pMsg->message == WM_KEYDOWN)
    {
        CImageToolView* pView = (CImageToolView*)GetActiveView();
        if (pMsg->hwnd != pView->m_previewDlg.m_hWnd)
        {
            pView->m_previewDlg.PostMessageW(pMsg->message, pMsg->wParam, pMsg->lParam);
            return TRUE;
        }
    }

    return CFrameWndEx::PreTranslateMessage(pMsg);
}


void CMainFrame::OnBtnFilepath()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
    CFolderPickerDialog dlg;
    CString strPathName;

    dlg.m_ofn.lpstrTitle = _T("Put your title here");
    dlg.m_ofn.lpstrInitialDir = _T("C:\\");
    if (dlg.DoModal() == IDOK) {
        strPathName = dlg.GetPathName();   // Use this to get the selected folder name 
        AfxMessageBox(strPathName);
    }
}


void CMainFrame::OnDestroy()
{
    CFrameWndEx::OnDestroy();

    m_wndThumbnailView.DestroyWindow();
    m_wndMagnifyView.DestroyWindow();
    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CMainFrame::OnFileRestart()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
    theApp.RestartInstance();
}


void CMainFrame::OnMapSelectunit()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
    CSelectMap dlg;
    if (dlg.DoModal() == IDOK)
    {

    }
}
