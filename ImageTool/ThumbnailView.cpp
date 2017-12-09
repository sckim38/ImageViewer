// ThumbnailView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ImageTool.h"
#include "ThumbnailView.h"


// CThumbnailView

IMPLEMENT_DYNAMIC(CThumbnailView, CImageWnd)

CThumbnailView::CThumbnailView()
{
}

CThumbnailView::~CThumbnailView()
{
}

BEGIN_MESSAGE_MAP(CThumbnailView, CImageWnd)
    ON_WM_SIZE()
    //ON_WM_PAINT()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CThumbnailView �޽��� ó�����Դϴ�.

void CThumbnailView::OnRenderItem(ID2D1RenderTarget *pRT, LayoutCell &item)
{
    pRT->Clear(m_data.background_color);
    pRT->SetTransform(item.matrix_);
    if (item.m_pBitmap)
    {
        pRT->DrawBitmap(item.m_pBitmap.Get(), nullptr);
    }
    item.RenderDrawObject(pRT, m_data);
    pRT->SetTransform(D2D1::Matrix3x2F::Identity());

}

void CThumbnailView::SetBitmap(ComPtr<ID2D1Bitmap> pBitmap)
{
    m_layout.m_items[0][0].m_pBitmap = pBitmap;
    m_layout.m_items[0][0].AdjustScale();
}

void CThumbnailView::NotifyJumpToOrigin(const CPoint &pt)
{
    if (m_pMagnifyView)
    {
        D2D1_POINT_2F point = m_layout.m_items[0][0].InvertTransform(pt);
        m_pMagnifyView->JumpToOrigin(point);

        CD2DRectF rect = m_pMagnifyView->GetPageLayout();
        DrawBoundingRect(rect);
    }
}

void CThumbnailView::DrawBoundingRect(const CD2DRectF &rect)
{
    //m_layout.m_items[0][0].SetBoundingRect(D2D1::ColorF(D2D1::ColorF::Yellow), rect);
    Render(&m_layout.m_items[0][0]);
}

void CThumbnailView::OnSize(UINT nType, int cx, int cy)
{
    CImageWnd::OnSize(nType, cx, cy);

    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
    m_layout.m_items[0][0].SetLayout(0, 0, cx, cy);
    m_layout.m_items[0][0].AdjustScale();
}

void trace(LPCWSTR title, const CD2DRectF &rect)
{
    TRACE(_T("%s : %.3f, %.3f, %.3f, %.3f\n"), title, rect.left, rect.top, rect.right, rect.bottom);
}

void CThumbnailView::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
    // �׸��� �޽����� ���ؼ��� CWnd::OnPaint()��(��) ȣ������ ���ʽÿ�.

    Render(&m_layout.m_items[0][0]);
}



void CThumbnailView::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
    if ((nFlags & MK_LBUTTON) != 0)
    {
        NotifyJumpToOrigin(point);
    }

    CWnd::OnMouseMove(nFlags, point);
}


void CThumbnailView::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
    NotifyJumpToOrigin(point);

    CWnd::OnLButtonDown(nFlags, point);
}
