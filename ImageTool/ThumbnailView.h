#pragma once

#include "MagnifyView.h"

// CThumbnailView

class CThumbnailView : public CImageWnd
{
	DECLARE_DYNAMIC(CThumbnailView)

public:
	CThumbnailView();
	virtual ~CThumbnailView();

    virtual void OnRenderItem(ID2D1RenderTarget *, LayoutCell &);
    void SetBitmap(ComPtr<ID2D1Bitmap> pBitmap);
    void NotifyJumpToOrigin(const CPoint &point);
    void DrawBoundingRect(const CD2DRectF &rect);

    CMagnifyView *m_pMagnifyView = nullptr;

protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


