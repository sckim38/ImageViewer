#pragma once

#if !defined(_AFXEXT)
#pragma comment(lib, "MFCImageView")
#endif

#define _USE_MATH_DEFINES

#include <algorithm>
#include <cassert>
#include <exception>
#include <memory>
#include <vector>

#include <atlbase.h>
#include <atltrace.h>
#include <atltypes.h>
#include <tchar.h>
#include <Windows.h>
#include <ppltasks.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

#include <d2d1helper.h>
//#include <d2d1_1helper.h>
#include <wincodecsdk.h>
#include <WinCodec.h>

#pragma comment(lib, "d2d1")

#include "is_any.hpp"
#include "reverse_wrapper.hpp"
#include "singleton.hpp"

#include "LayoutMatrix.h"
#include "WicImageFactory2.h"
#include "DrawHelper.h"
#include "Geometry.hpp"
#include "TextFormat.h"
#include "DrawObject.h"
#include "CompatibleRenderTarget.h"
#include "LayoutCell.h"
#include "Layout.h"

// CImageWnd

class AFX_EXT_CLASS CImageWnd : public CWnd
{
	DECLARE_DYNAMIC(CImageWnd)

public:
	CImageWnd();
	virtual ~CImageWnd();

    bool LoadBitmap(LPCWSTR);
    bool LoadWicBitmap(LPCWSTR);

    bool SetWicBitmap(ComPtr<IWICBitmapSource>);
    void SetDrawObject(DrawShape drawObj)
    {
        DrawShapeMode::instance().c_drawShape = drawObj;// DrawShape::Line;
        CRect rect;
        GetWindowRect(rect);
        ClipCursor(rect);
    }

    void FitScaleToBitmap();
    void Render(LayoutCell* pItem);
    void RenderItem(LayoutCell& pItem);
    virtual void OnRenderItem(ID2D1RenderTarget *, LayoutCell &) = 0;

    LayoutCell *m_pCurCell = nullptr;
    GridLayout m_layout;
    RenderData m_data;
    CHwndRenderTarget m_RenderTarget;

protected:
	DECLARE_MESSAGE_MAP()
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};

