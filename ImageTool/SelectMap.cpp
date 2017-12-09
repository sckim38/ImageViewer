// SelectMap.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ImageTool.h"
#include "SelectMap.h"

// CSelectMap ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CSelectMap, CDialogResize)

CSelectMap::CSelectMap(CWnd* pParent /*=NULL*/)
	: CDialogResize(CSelectMap::IDD, pParent)
{
    m_wndUnitMap.pThis = this;
}

CSelectMap::~CSelectMap()
{
}

void CSelectMap::DoDataExchange(CDataExchange* pDX)
{
	CDialogResize::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CController)
    //DDX_Control(pDX, IDC_UNIT_MAP, m_wndUnitMap);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectMap, CDialogResize)
    ON_BN_CLICKED(IDOK, &CSelectMap::OnBnClickedOk)
    ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
    ON_BN_CLICKED(IDC_SELECT_MAP, &CSelectMap::OnBnClickedSelectMap)
    ON_BN_CLICKED(IDC_DISABLE_PCB, &CSelectMap::OnBnClickedDisablePcb)
    ON_BN_CLICKED(IDC_ENABLE_PCB, &CSelectMap::OnBnClickedEnablePcb)
END_MESSAGE_MAP()


// CSelectMap �޽��� ó�����Դϴ�.


void CSelectMap::OnBnClickedOk()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    CDialogResize::OnOK();
}


BOOL CSelectMap::OnInitDialog()
{
    __super::OnInitDialog();

    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
    CRect rect;
    CWnd *pWnd = GetDlgItem(IDC_UNIT_MAP);
    pWnd->GetClientRect(rect);
    pWnd->MapWindowPoints(this, &rect);

    m_wndUnitMap.m_layout = GridLayout(1, 1);
    m_wndUnitMap.m_data.background_color = D2D1::ColorF(D2D1::ColorF::WhiteSmoke);
    m_wndUnitMap.Create(nullptr, nullptr, WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rect, this, (UINT)-1);

    m_data = {
        {
            { IDC_UNIT_MAP, DLSZ_SIZE_X | DLSZ_SIZE_Y }
        },
        {
            { IDC_SELECT_MAP, DLSZ_MOVE_X | DLSZ_MOVE_Y }
        },
        {
            { IDC_DISABLE_PCB, DLSZ_MOVE_X | DLSZ_MOVE_Y }
        },
        {
            { IDC_ENABLE_PCB, DLSZ_MOVE_X | DLSZ_MOVE_Y }
        },
        {
            { IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y },
        },
        {
            { IDCANCEL, DLSZ_MOVE_X | DLSZ_MOVE_Y },
        }
    };

    DlgResize_Init();

    SetSize(1050, 850);

    InitUnitMap(86, 68);
    //SetWindowPos(NULL, 0, 0, m_sizeMinTrack.cx, m_sizeMinTrack.cy, SWP_NOMOVE | SWP_NOZORDER);
    //MoveWindow(0, 0, m_sizeMinTrack.cx, m_sizeMinTrack.cy);
    //CenterWindow();

    return TRUE;  // return TRUE unless you set the focus to a control
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CSelectMap::OnSize(UINT nType, int cx, int cy)
{
    //ATLTRACE(L"CSelectMap::OnSize(%d, %d, %d); [%d, %d], [%d, %d]\n", nType, cx, cy, m_sizeDialog.cx, m_sizeDialog.cy, m_sizeMinTrack.cx, m_sizeMinTrack.cy);
    __super::OnSize(nType, cx, cy);
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
    if (GetSafeHwnd()){
        CWnd *pWnd = GetDlgItem(IDC_UNIT_MAP);
        if (pWnd){
            CRect rect;
            pWnd->GetClientRect(rect);
            pWnd->MapWindowPoints(this, &rect);
            if (m_wndUnitMap.GetSafeHwnd())
            {
                m_wndUnitMap.MoveWindow(rect);
            }
        }
    }
}


void CSelectMap::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
    __super::OnGetMinMaxInfo(lpMMI);
}


void CSelectMap::OnBnClickedSelectMap()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    DrawShapeMode::instance().c_drawShape = DrawShape::Polygon;
}


void CSelectMap::OnBnClickedDisablePcb()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    if (DrawShapeMode::instance().c_drawShape == DrawShape::Selection)
    {
        auto cell = m_wndUnitMap.m_layout.m_items[0][0];

        if (cell.draw_object_list_.size())
        {
            auto pShape = cell.draw_object_list_[0];
            for (auto &row : m_pcb_map)
            {
                for (auto &col : row)
                {
                    auto center = D2D1::Point2F(col.rect.CenterPoint());
                    if (pShape->HitTestFill(center))
                    {
                        col.enable = false;
                    }
                }
            }
            m_wndUnitMap.Invalidate();
        }
    }
}


void CSelectMap::OnBnClickedEnablePcb()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    if (DrawShapeMode::instance().c_drawShape == DrawShape::Selection)
    {
        auto cell = m_wndUnitMap.m_layout.m_items[0][0];

        if (cell.draw_object_list_.size())
        {
            auto pShape = cell.draw_object_list_[0];
            for (auto &row : m_pcb_map)
            {
                for (auto &col : row)
                {
                    auto center = D2D1::Point2F(col.rect.CenterPoint());
                    if (pShape->HitTestFill(center))
                    {
                        col.enable = true;
                    }
                }
            }
            m_wndUnitMap.Invalidate();
        }
    }
}
