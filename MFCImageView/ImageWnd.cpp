// ImageWnd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ImageWnd.h"


//DrawShape DrawShapeMode::c_drawShape = DrawShape::Selection;

// CImageWnd

IMPLEMENT_DYNAMIC(CImageWnd, CWnd)

CImageWnd::CImageWnd()
{
}

CImageWnd::~CImageWnd()
{
}

bool CImageWnd::LoadBitmap(LPCWSTR lpszFilename)
{
    CD2DBitmap bitmap(&m_RenderTarget, lpszFilename);
    HRESULT hr = bitmap.Create(&m_RenderTarget);

    if (SUCCEEDED(hr))
    {
        for (auto &items : m_layout.m_items)
            for (auto &item : items)
                item.SetBitmap(bitmap.Get());

        return true;
    }
    return false;
}

bool CImageWnd::LoadWicBitmap(LPCWSTR lpszFilename)
{
    auto source = CWICImagingFactory2::LoadBitmapFromFile(lpszFilename);

    ComPtr<ID2D1Bitmap> pBitmap;
    HRESULT hr = m_RenderTarget.GetRenderTarget()->CreateBitmapFromWicBitmap(source.Get(), pBitmap.GetAddressOf());

    if (SUCCEEDED(hr))
    {
        for (auto &items : m_layout.m_items)
            for (auto &item : items)
                item.SetBitmap(pBitmap);

        return true;
    }
    return false;
}

bool CImageWnd::SetWicBitmap(ComPtr<IWICBitmapSource> pSource)
{
    ComPtr<ID2D1Bitmap> pBitmap;
    HRESULT hr = m_RenderTarget.GetRenderTarget()->CreateBitmapFromWicBitmap(pSource.Get(), pBitmap.GetAddressOf());

    if (SUCCEEDED(hr))
    {
        for (auto &items : m_layout.m_items)
            for (auto &item : items)
                item.SetBitmap(pBitmap);

        return true;
    }
    return false;
}

void CImageWnd::FitScaleToBitmap()
{
    for (auto &items : m_layout.m_items)
        for (auto &item : items)
            item.AdjustScale();
}

void CImageWnd::Render(LayoutCell* pCell)
{
    assert(pCell);

    m_RenderTarget.BeginDraw();
    //pCell->Render(m_data);
    RenderItem(*pCell);
    m_RenderTarget.EndDraw();
}

void CImageWnd::RenderItem(LayoutCell &item)
{
    auto pBitmap = CreateCompatibleD2DBitmap(m_data.m_pRenderTarget.Get(), item.block_.Size(), [this, &item](ID2D1RenderTarget *pRT){
        OnRenderItem(pRT, item);
    });
    if (pBitmap)
    {
        m_data.m_pRenderTarget->DrawBitmap(
            pBitmap.Get(),
            D2D1::RectF(item.block_),
            1.F,
            D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
    }
}

BEGIN_MESSAGE_MAP(CImageWnd, CWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()

    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEWHEEL()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()

// CImageWnd 메시지 처리기입니다.

BOOL CImageWnd::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
        ::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), NULL);

    return CWnd::PreCreateWindow(cs);
}

int CImageWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    HRESULT hr = m_RenderTarget.Create(m_hWnd);
    if (SUCCEEDED(hr))
    {
        m_data.Create(m_RenderTarget.GetHwndRenderTarget());
        return 0;
    }
    return -1;
}

void CImageWnd::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    m_layout.RecalcLayout(cx, cy);

    if (m_RenderTarget.IsValid())
    {
        m_RenderTarget.Resize(CD2DSizeU(cx, cy));
    }
}

BOOL CImageWnd::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}

void CImageWnd::OnPaint()
{
    CPaintDC dc(this);
    // 그리기 메시지에 대해서는 CWnd::OnPaint()을(를) 호출하지 마십시오.
    if (m_RenderTarget.IsValid())
    {
        // initiate drawing on render m_RenderTarget
        m_RenderTarget.BeginDraw();

        m_RenderTarget.Clear(m_data.background_color);
        
        for (auto &items : m_layout.m_items)
            for (auto &item : items)
            {
                RenderItem(item);
            }

        // ends drawing operations 
        HRESULT hr = m_RenderTarget.EndDraw();
        // if the render m_RenderTarget has been lost, then recreate it
        if (D2DERR_RECREATE_TARGET == hr)
        {
            m_RenderTarget.ReCreate(m_hWnd);
            m_data.Create(m_RenderTarget.GetHwndRenderTarget());
        }
    }
}

void CImageWnd::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if (m_pCurCell)
    {
        MouseMoveHandler(m_pCurCell, nFlags, point);
        Render(m_pCurCell);
    }
    else
    {
        auto pSel = m_layout.HitTest(point);
        if (!pSel)
            return;

        MouseMoveHandler(pSel, nFlags, point);
        Render(pSel);
    }

    CWnd::OnMouseMove(nFlags, point);
}

BOOL CImageWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
    if (zDelta)
    {
        ScreenToClient(&point);
        auto pCell = m_layout.HitTest(point);
        if (pCell)
        {
            FLOAT factor = (FLOAT)zDelta / 1200.F;
            pCell->ScaleAt(factor, point);

            Render(pCell);
        }
    }

    return CWnd::OnMouseWheel(nFlags, zDelta, point);
}

void CImageWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (!m_pCurCell)
    {
        m_pCurCell = m_layout.HitTest(point);
    }
    if (m_pCurCell)
    {
        LButtonDownHandler(m_pCurCell, nFlags, point);
    }

    CWnd::OnLButtonDown(nFlags, point);
}

void CImageWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_pCurCell)
    {
        LButtonUpHandler(m_pCurCell, nFlags, point);
        Render(m_pCurCell);
        if (DrawShapeMode::instance().c_drawShape == DrawShape::Selection)
            m_pCurCell = nullptr;
    }

    CWnd::OnLButtonUp(nFlags, point);
}

void CImageWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    if (!m_pCurCell)
    {
        m_pCurCell = m_layout.HitTest(point);
    }
    if (m_pCurCell)
    {
        LButtonDblClkHandler(m_pCurCell, nFlags, point);
        Render(m_pCurCell);
        if (DrawShapeMode::instance().c_drawShape == DrawShape::Selection)
            m_pCurCell = nullptr;
    }

    CWnd::OnLButtonDblClk(nFlags, point);
}

int CImageWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    SetFocus();
    return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}
