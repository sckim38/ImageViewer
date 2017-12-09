// MagnifyView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ImageTool.h"
#include "MagnifyView.h"
#include "ThumbnailView.h"

// CMagnifyView

IMPLEMENT_DYNAMIC(CMagnifyView, CImageWnd)

CMagnifyView::CMagnifyView()
{
}

CMagnifyView::~CMagnifyView()
{
}

void CMagnifyView::OnRenderItem(ID2D1RenderTarget *pRT, LayoutCell &item)
{
    pRT->Clear(m_data.background_color);
    pRT->SetTransform(item.matrix_);
    if (item.m_pBitmap)
    {
        pRT->DrawBitmap(item.m_pBitmap.Get(), nullptr, 1.F, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
    }
    pRT->SetTransform(D2D1::Matrix3x2F::Identity());

}

void CMagnifyView::NotifyChangeBoundingRect()
{
    if (m_pThumbnailView)
    {
        CD2DRectF rect = GetPageLayout();
        m_pThumbnailView->DrawBoundingRect(rect);
    }
}

CD2DRectF CMagnifyView::GetPageLayout()
{
    return m_layout.m_items[0][0].GetPageLayout();
}

void CMagnifyView::JumpToOrigin(const CD2DPointF &pt)
{
    CRect rect;
    GetClientRect(&rect);

    m_layout.m_items[0][0].JumpToOrigin(pt, rect);
    Render(&m_layout.m_items[0][0]);
}

void CMagnifyView::SetBitmap(ComPtr<ID2D1Bitmap> pBitmap)
{
    m_layout.m_items[0][0].m_pBitmap = pBitmap;
    //CImageView::SetBitmap(pBitmap);
    NotifyChangeBoundingRect();
}

BEGIN_MESSAGE_MAP(CMagnifyView, CImageWnd)
    ON_WM_SIZE()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()



// CMagnifyView 메시지 처리기입니다.

void CMagnifyView::OnSize(UINT nType, int cx, int cy)
{
    CImageWnd::OnSize(nType, cx, cy);

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
    NotifyChangeBoundingRect();
}



void CMagnifyView::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

    CImageWnd::OnMouseMove(nFlags, point);
    NotifyChangeBoundingRect();
}


BOOL CMagnifyView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

    BOOL res = CImageWnd::OnMouseWheel(nFlags, zDelta, pt);
    NotifyChangeBoundingRect();
    return res;
}

