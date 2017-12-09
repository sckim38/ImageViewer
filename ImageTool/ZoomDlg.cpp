// ZoomDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ImageTool.h"
#include "ZoomDlg.h"
#include "afxdialogex.h"


// CZoomDlg ��ȭ �����Դϴ�.

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


// CZoomDlg �޽��� ó�����Դϴ�.


BOOL CZoomDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

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
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CZoomDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
    PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));

    CDialogEx::OnLButtonDown(nFlags, point);
}

LRESULT CZoomDlg::OnNcHitTest(CPoint point)
{
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    AfxGetMainWnd()->PostMessageW(WM_COMMAND, IDC_BTN_FILEPATH);
}


void CZoomDlg::OnDrawLine()
{
    AfxGetMainWnd()->PostMessageW(WM_COMMAND, ID_DRAW_LINE);
    // TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void CZoomDlg::OnDrawRectangle()
{
    AfxGetMainWnd()->PostMessageW(WM_COMMAND, ID_DRAW_RECTANGLE);
    // TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void CZoomDlg::OnDrawEllipse()
{
    AfxGetMainWnd()->PostMessageW(WM_COMMAND, ID_DRAW_ELLIPSE);
    // TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}

void CZoomDlg::OnDrawCircle()
{
    AfxGetMainWnd()->PostMessageW(WM_COMMAND, ID_DRAW_CIRCLE);
    // TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}
