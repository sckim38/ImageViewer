// ZoomDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ImageTool.h"
#include "ZoomDlg.h"
#include "afxdialogex.h"


// CZoomDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CZoomDlg, CDialogEx)

CZoomDlg::CZoomDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CZoomDlg::IDD, pParent)
{

}

CZoomDlg::~CZoomDlg()
{
}

void CZoomDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BTN_FILEPATH, m_button);
}


BEGIN_MESSAGE_MAP(CZoomDlg, CDialogEx)
    ON_WM_LBUTTONDOWN()
    ON_WM_NCHITTEST()
    ON_BN_CLICKED(IDC_BTN_FILEPATH, &CZoomDlg::OnClickedBtnFilepath)
    ON_COMMAND(ID_DRAW_LINE, &CZoomDlg::OnDrawLine)
    ON_COMMAND(ID_DRAW_RECTANGLE, &CZoomDlg::OnDrawRectangle)
    ON_COMMAND(ID_DRAW_ELLIPSE, &CZoomDlg::OnDrawEllipse)
    ON_COMMAND(ID_DRAW_CIRCLE, &CZoomDlg::OnDrawCircle)
END_MESSAGE_MAP()


// CZoomDlg 메시지 처리기입니다.


BOOL CZoomDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.

    //ModifyStyle(WS_CAPTION, WS_CLIPCHILDREN);
    //SetBackgroundColor(RGB(1, 11, 21));
    //ModifyStyleEx(0, WS_EX_LAYERED);
    //SetLayeredWindowAttributes(RGB(1, 11, 21), 255, LWA_COLORKEY | LWA_ALPHA);


    if (m_ToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, 100)) // && m_ToolBar.LoadToolBar (IDB_DRAWSMALL, 0, 0, TRUE /* Locked */))
    {
        BOOL res = m_ToolBar.LoadBitmap(IDB_HOME_SMALL);
        m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_ANY));

        CMFCToolBarButton Line(ID_DRAW_LINE, 8, _T("Draw Line"), TRUE);
        CMFCToolBarButton Rectangle(ID_DRAW_RECTANGLE, 9, _T("Draw Rectangle"), TRUE);
        CMFCToolBarButton Ellipse(ID_DRAW_ELLIPSE, 11, _T("Draw Ellipse"), TRUE);
        CMFCToolBarButton Circle(ID_DRAW_CIRCLE, 12, _T("Draw Circle"), TRUE);

        //Line.m_bImage = TRUE;
        //Rectangle.m_bImage = TRUE;
        //Ellipse.m_bImage = TRUE;

        m_ToolBar.InsertButton(Line);
        m_ToolBar.InsertButton(Rectangle);
        m_ToolBar.InsertButton(Ellipse);
        m_ToolBar.InsertButton(Circle);

        CSize   sizeToolBar = m_ToolBar.CalcFixedLayout(FALSE, TRUE);
        m_ToolBar.SetWindowPos(NULL, 0 + 15, 0, sizeToolBar.cx, sizeToolBar.cy,
            SWP_NOACTIVATE | SWP_NOZORDER);

        CRect rect;
        GetWindowRect(rect);
        SetWindowPos(NULL, rect.left, rect.top, sizeToolBar.cx + 30, sizeToolBar.cy+50,
            SWP_NOACTIVATE | SWP_NOZORDER);

        m_button.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
        m_button.m_bTransparent = TRUE;

        m_button.SetImage(AfxGetApp()->LoadIconW(IDI_ARROW_UP));
        m_button.SetWindowText(L"");
        m_button.SizeToContent();
    }
    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CZoomDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));

    CDialogEx::OnLButtonDown(nFlags, point);
}

LRESULT CZoomDlg::OnNcHitTest(CPoint point)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    CRect rect;
    GetClientRect(rect);
    ClientToScreen(rect);

    if (rect.PtInRect(point))
    {
        return HTCAPTION;
    }
    return CDialogEx::OnNcHitTest(point);
}


void CZoomDlg::OnClickedBtnFilepath()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    AfxGetMainWnd()->PostMessageW(WM_COMMAND, IDC_BTN_FILEPATH);
}


void CZoomDlg::OnDrawLine()
{
    AfxGetMainWnd()->PostMessageW(WM_COMMAND, ID_DRAW_LINE);
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CZoomDlg::OnDrawRectangle()
{
    AfxGetMainWnd()->PostMessageW(WM_COMMAND, ID_DRAW_RECTANGLE);
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CZoomDlg::OnDrawEllipse()
{
    AfxGetMainWnd()->PostMessageW(WM_COMMAND, ID_DRAW_ELLIPSE);
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CZoomDlg::OnDrawCircle()
{
    AfxGetMainWnd()->PostMessageW(WM_COMMAND, ID_DRAW_CIRCLE);
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
}
