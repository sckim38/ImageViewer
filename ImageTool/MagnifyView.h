#pragma once

// CMagnifyView
class CThumbnailView;

class CMagnifyView : public CImageWnd
{
	DECLARE_DYNAMIC(CMagnifyView)

public:
	CMagnifyView();
	virtual ~CMagnifyView();

    //BOOL Create(const RECT &rect, CWnd *pWnd);
    virtual void OnRenderItem(ID2D1RenderTarget *, LayoutCell &);

    void NotifyChangeBoundingRect();
    CD2DRectF GetPageLayout();
    void JumpToOrigin(const CD2DPointF &pt);
    void SetBitmap(ComPtr<ID2D1Bitmap> pBitmap);

    CThumbnailView *m_pThumbnailView = nullptr;

protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnPaint();
};


