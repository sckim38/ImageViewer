// ImageView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ImageTool.h"
#include "ImageView.h"

// CImageView

IMPLEMENT_DYNAMIC(CImageView, CImageWnd)

CImageView::CImageView()
{
}

CImageView::~CImageView()
{
}

void CImageView::OnRenderItem(ID2D1RenderTarget *pRT, LayoutCell &item)
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

BEGIN_MESSAGE_MAP(CImageView, CImageWnd)
END_MESSAGE_MAP()

// CImageView 메시지 처리기입니다.







