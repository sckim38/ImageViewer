#pragma once

// CImageView

class CImageView : public CImageWnd
{
	DECLARE_DYNAMIC(CImageView)

public:
	CImageView();
	virtual ~CImageView();

    virtual void OnRenderItem(ID2D1RenderTarget *, LayoutCell &);

protected:
	DECLARE_MESSAGE_MAP()
public:
};


