#pragma once

enum class DrawShape : BYTE
{
    Selection, Line, Ellipse, Rectangle, Polygon, Circle
};

enum class SelectMode : BYTE
{
    None, NetSelect, Move, Size
};

struct AFX_EXT_CLASS DrawShapeMode : unique_singleton<DrawShapeMode>
{
    DrawShape c_drawShape = DrawShape::Selection;
};

struct RenderData
{
    void Create(ComPtr<ID2D1HwndRenderTarget> pRenderTarget)
    {
        assert(pRenderTarget);
        m_pRenderTarget = pRenderTarget;

        HRESULT hr = m_pRenderTarget->CreateSolidColorBrush(fill_color, m_pFillBrush.ReleaseAndGetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        hr = m_pRenderTarget->CreateSolidColorBrush(line_color, m_pLineBrush.ReleaseAndGetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        hr = m_pRenderTarget->CreateSolidColorBrush(select_line_color, m_pSelLineBrush.ReleaseAndGetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        hr = m_pRenderTarget->CreateSolidColorBrush(text_color, m_pTextBrush.ReleaseAndGetAddressOf());
        ATLENSURE_SUCCEEDED(hr);
    }

    void DrawCustom(ID2D1RenderTarget *pRT)
    {
        if (m_draw_chain)
            m_draw_chain(pRT, *this);
    }
    std::function<void(ID2D1RenderTarget *, RenderData &)> m_draw_chain;

    ComPtr<ID2D1HwndRenderTarget> m_pRenderTarget;
    D2D1_COLOR_F background_color = D2D1::ColorF(D2D1::ColorF::CadetBlue);
    D2D1_COLOR_F fill_color = D2D1::ColorF(D2D1::ColorF::WhiteSmoke, 0.3F);
    D2D1_COLOR_F line_color = D2D1::ColorF(D2D1::ColorF::Blue);
    D2D1_COLOR_F select_line_color = D2D1::ColorF(D2D1::ColorF::Red);
    D2D1_COLOR_F text_color = D2D1::ColorF(D2D1::ColorF::Black);
    ComPtr<ID2D1SolidColorBrush> m_pFillBrush;
    ComPtr<ID2D1SolidColorBrush> m_pLineBrush;
    ComPtr<ID2D1SolidColorBrush> m_pSelLineBrush;
    ComPtr<ID2D1SolidColorBrush> m_pTextBrush;
    FLOAT strok_width = 2.F;
};

class LayoutCell
{
public:
    auto GetSafeHwnd()->HWND
    {
        return AfxGetThreadState()->m_lastSentMsg.hwnd;
    }
    D2D1_POINT_2F InvertTransform(CPoint point)
    {
        point -= block_.TopLeft();
        return Layout::MatrixInvertor<D2D1_POINT_2F>::InvertTransform(matrix_, point);
    }
    bool HitTest(POINT point)
    {
        return block_.PtInRect(point) == TRUE;
    }
    bool HitTest(D2D1_POINT_2F point)
    {
        for (auto ptr : reverse(draw_object_list_))
        {
            if (ptr->HitTestStroke(point))
            {
                select_object_ = ptr;
                return true;
            }
        }
        for (auto ptr : reverse(draw_object_list_))
        {
            if (ptr->HitTestFill(point))
            {
                select_object_ = ptr;
                return true;
            }
        }
        return false;
    }
    void AdjustOffset()
    {
        if (m_pBitmap)
        {
            matrix_.AdjustOffset(m_pBitmap.Get(), block_);
        }
    }
    void AdjustScale()
    {
        if (m_pBitmap)
        {
            matrix_.AdjustScale(m_pBitmap.Get(), block_);
        }
    }

    void SetLayout(INT x, INT y, INT cx, INT cy)
    {
        block_.SetRect(x, y, x + cx, y + cy);
        AdjustOffset();
    }
    template<typename T>
    void JumpToOrigin(D2D1_POINT_2F point, T rect)
    {
        matrix_.JumpToOrigin(point, rect);
    }
    D2D1_RECT_F GetPageLayout()
    {
        return Layout::MatrixInvertor<D2D1_RECT_F>::InvertTransform(matrix_, block_);
    }
    //void Render(RenderData &data)
    //{
    //    auto pBitmap = CreateCompatibleD2DBitmap(data.m_pRenderTarget.Get(), block_.Size(), [this, &data](ID2D1RenderTarget *pRT){
    //        pRT->Clear(data.background_color);
    //        pRT->SetTransform(matrix_);
    //        if (m_pBitmap)
    //        {
    //            pRT->DrawBitmap(m_pBitmap.Get(), nullptr);
    //        }
    //        RenderDrawObject(pRT, data);
    //        data.DrawCustom(pRT);
    //        pRT->SetTransform(D2D1::Matrix3x2F::Identity());
    //    });
    //    if (pBitmap)
    //    {
    //        data.m_pRenderTarget->DrawBitmap(
    //            pBitmap.Get(),
    //            D2D1::RectF(block_));
    //    }
    //}
    void RenderDrawObject(ID2D1RenderTarget *pRT, RenderData &data)
    {
        for (auto &obj : draw_object_list_)
        {
            auto pBrush = (select_object_ == obj) ? data.m_pSelLineBrush : data.m_pLineBrush;

            obj->DrawGeometry(
                pRT,
                data.m_pFillBrush,
                pBrush,
                data.strok_width);
            obj->DrawLength(
                pRT,
                data.m_pTextBrush
                );
        }
    }
    void ScaleAt(FLOAT factor, CPoint point)
    {
        point -= block_.TopLeft();
        matrix_.ScaleAt(factor, point);
        AdjustOffset();
    }
    void SetBitmap(ComPtr<ID2D1Bitmap> pBitmap)
    {
        matrix_ = Layout::Matrix();
        m_pBitmap.Swap(pBitmap);
    }
public:
    bool lbutton_down = false;
    CPoint last_point;

    D2D1_POINT_2F c_down = D2D1_POINT_2F{};

    CRect block_;
    Layout::Matrix matrix_;
    ComPtr<ID2D1Bitmap> m_pBitmap;

    DrawObjectPtr select_object_;
    std::vector<DrawObjectPtr> draw_object_list_;
};

class DrawTool
{
    // Constructors
public:
    virtual void OnLButtonDown(LayoutCell *pCell, UINT nFlags, D2D1_POINT_2F point)
    {
        ::SetCapture(pCell->GetSafeHwnd());
        pCell->c_down = point;
    }
    virtual void OnLButtonDblClk(LayoutCell* /*pCell*/, UINT /*nFlags*/, D2D1_POINT_2F /*point*/) {}
    virtual void OnLButtonUp(LayoutCell* pCell, UINT /*nFlags*/, D2D1_POINT_2F point)
    {
        ReleaseCapture();
        if (point == pCell->c_down)
            DrawShapeMode::instance().c_drawShape = DrawShape::Selection;
    }
    virtual void OnMouseMove(LayoutCell* pCell, UINT /*nFlags*/, D2D1_POINT_2F point)
    {
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
    }
    virtual void OnCancel(LayoutCell* pCell)
    {
        DrawShapeMode::instance().c_drawShape = DrawShape::Selection;
    }

    static DrawTool* FindTool();
};

class SelectTool : public DrawTool, public unique_singleton<SelectTool>
{
    // Constructors
public:

    SelectMode select_mode = SelectMode::None;
    int resize_handle = 0;
    D2D1_POINT_2F last_point;

    // Implementation
    void OnLButtonDown(LayoutCell* pCell, UINT nFlags, D2D1_POINT_2F point) override
    {
        select_mode = SelectMode::None;

        // Check for resizing(only allowed on single selections)
        if (pCell->select_object_)
        {
            resize_handle = pCell->select_object_->HitTest(point);
            if (resize_handle != 0)
                select_mode = SelectMode::Size;
        }

        // See if the click was on an object, select and start move if so
        if (select_mode == SelectMode::None)
        {
            if (pCell->HitTest(point))
                select_mode = SelectMode::Move;
        }

        // Click on background, start a drag 
        if (select_mode == SelectMode::None)
        {
            pCell->select_object_.reset();
            select_mode = SelectMode::NetSelect;
        }

        last_point = point;

        if (DrawShapeMode::instance().c_drawShape == DrawShape::Selection)
            DrawTool::OnLButtonDown(pCell, nFlags, point);
    }
    void OnMouseMove(LayoutCell* pCell, UINT nFlags, D2D1_POINT_2F point) override
    {
        if (::GetCapture() != pCell->GetSafeHwnd())
        {
            if (DrawShapeMode::instance().c_drawShape == DrawShape::Selection && pCell->select_object_)
            {
                int nHandle = pCell->select_object_->HitTest(point);
                if (nHandle != 0)
                {
                    SetCursor(pCell->select_object_->GetHandleCursor(nHandle));
                    return; // bypass CDrawTool
                }
            }
            if (DrawShapeMode::instance().c_drawShape == DrawShape::Selection)
                DrawTool::OnMouseMove(pCell, nFlags, point);
            return;
        }

        FLOAT dx = point.x - last_point.x;
        FLOAT dy = point.y - last_point.y;
        if (select_mode == SelectMode::NetSelect)
        {
            DrawTool::OnMouseMove(pCell, nFlags, point);
            return;
        }

        if (pCell->select_object_)
        {
            if (select_mode == SelectMode::Move)
            {
                auto rect = D2D1::RectF(
                    pCell->select_object_->rect_.left + dx,
                    pCell->select_object_->rect_.top + dy,
                    pCell->select_object_->rect_.right + dx,
                    pCell->select_object_->rect_.bottom + dy);
                pCell->select_object_->MoveTo(rect);
                pCell->select_object_->CreateGeometry();
            }
            else if (resize_handle != 0)
            {
                if (pCell->select_object_->MoveHandleTo(resize_handle, point))
                    pCell->select_object_->CreateGeometry();
            }
        }

        last_point = point;

        if (select_mode == SelectMode::Size && DrawShapeMode::instance().c_drawShape == DrawShape::Selection)
        {
            SetCursor(pCell->select_object_->GetHandleCursor(resize_handle));
            return; // bypass CDrawTool
        }

        if (DrawShapeMode::instance().c_drawShape == DrawShape::Selection)
            DrawTool::OnMouseMove(pCell, nFlags, point);
    }
};

template<D2D1::shapes shape, typename Enable = void>
class TDrawTool;

template<D2D1::shapes shape>
class TDrawTool<shape, typename std::enable_if_t<shape == D2D1::shapes::line || shape == D2D1::shapes::ellipse || shape == D2D1::shapes::rectangle>>
    : public DrawTool, public unique_singleton<TDrawTool<shape>>
{
    using DrawType = typename std::_If < shape == D2D1::shapes::line, DrawLine,
                     typename std::_If < shape == D2D1::shapes::ellipse, DrawEllipse, DrawRectangle >::type>::type;
    // Constructors
public:
    std::shared_ptr<DrawType> CreateObject(D2D1_RECT_F rect)
    {
        return std::make_shared<DrawType>(rect);
    }
    // Implementation
    void OnLButtonDown(LayoutCell* pCell, UINT nFlags, D2D1_POINT_2F point) override
    {
        DrawTool::OnLButtonDown(pCell, nFlags, point);

        pCell->select_object_ = CreateObject(D2D1::RectF(point));
        pCell->draw_object_list_.push_back(pCell->select_object_);

        SelectTool::instance().select_mode = SelectMode::Size;
        SelectTool::instance().resize_handle = 1;
        SelectTool::instance().last_point = point;
    }
    void OnLButtonUp(LayoutCell* pCell, UINT nFlags, D2D1_POINT_2F point) override
    {
        if (point == pCell->c_down)
        {
            // Don't Create empty objects...
            pCell->draw_object_list_.pop_back();
            SelectTool::instance().OnLButtonDown(pCell, nFlags, point); // try a select!
        }
        SelectTool::instance().OnLButtonUp(pCell, nFlags, point);
        DrawShapeMode::instance().c_drawShape = DrawShape::Selection;
    }
    void OnMouseMove(LayoutCell* pCell, UINT nFlags, D2D1_POINT_2F point) override
    {
        SetCursor(LoadCursor(nullptr, IDC_CROSS));
        SelectTool::instance().OnMouseMove(pCell, nFlags, point);
    }
};

using DrawPolygonPtr = std::shared_ptr<DrawPolygon>;

template<D2D1::shapes shape>
class TDrawTool<shape, typename std::enable_if_t<shape == D2D1::shapes::polygon || shape == D2D1::shapes::circle>>
    : public DrawTool, public unique_singleton<TDrawTool<shape>>
{
    using DrawType = typename std::_If < shape == D2D1::shapes::polygon, DrawPolygon, DrawCircle>::type;

    // Constructors
public:

    DrawPolygonPtr draw_polygon_;

    BOOL IsFirstPoint(D2D1_MATRIX_3X2_F matrix)
    {
        BOOL contains = FALSE;
        auto &point_list = draw_polygon_->point_list_;

        D2D1_ELLIPSE ellipse = D2D1::Ellipse(point_list[0], 2.F, 2.F);
        ComPtr<ID2D1EllipseGeometry> pGeometry;
        D2D1::GetFactory()->CreateEllipseGeometry(ellipse, pGeometry.GetAddressOf());
        pGeometry->StrokeContainsPoint(*point_list.rbegin(), 2.F, nullptr, matrix, &contains);

        return contains;
    }
    std::shared_ptr<DrawType> CreateObject(D2D1_RECT_F rect)
    {
        return std::make_shared<DrawType>(rect);
    }

    // Implementation
    void OnLButtonDown(LayoutCell* pCell, UINT nFlags, D2D1_POINT_2F point) override
    {
        DrawTool::OnLButtonDown(pCell, nFlags, point);

        if (!draw_polygon_) // Create polygon & add point
        {
            ::SetCapture(pCell->GetSafeHwnd());

            draw_polygon_ = CreateObject(D2D1::RectF(point));

            pCell->draw_object_list_.push_back(draw_polygon_);
            pCell->select_object_ = draw_polygon_;

            draw_polygon_->AddPoint(point);
        }
        else
        {
            if (pCell->select_object_ != draw_polygon_)
            {
                // cell is not object to draw polygon
                return;
            }

            if (IsFirstPoint(pCell->matrix_)) // if current point equal first point
            {
                // Stop when the first point is repeated...
                ReleaseCapture();

                draw_polygon_->point_list_.pop_back();
                if (draw_polygon_->point_list_.size() < 3)  // invalid polygon
                {
                    pCell->draw_object_list_.pop_back();    // erase last object
                    pCell->select_object_.reset();          // reset select object
                }
                else // complete
                {
                    //pCell->select_object_->CreateGeometry();
                }
                draw_polygon_.reset();
                DrawShapeMode::instance().c_drawShape = DrawShape::Selection;
                return;
            }
            if (shape == D2D1::shapes::circle) // circumcircle
            {
                if (draw_polygon_->point_list_.size() == 3)
                {
                    ReleaseCapture();
                    draw_polygon_.reset();
                    DrawShapeMode::instance().c_drawShape = DrawShape::Selection;
                    return;
                }
            }
        }

        draw_polygon_->AddPoint(point);
        pCell->select_object_->CreateGeometry();

        SelectTool::instance().select_mode = SelectMode::Size;
        SelectTool::instance().resize_handle = draw_polygon_->GetHandleCount();
        SelectTool::instance().last_point = point;
    }

    void OnLButtonUp(LayoutCell* /*pCell*/, UINT /*nFlags*/, D2D1_POINT_2F /*point*/) override
    {
        // Don't release capture yet!
    }

    void OnMouseMove(LayoutCell* pCell, UINT nFlags, D2D1_POINT_2F point) override
    {
        if (draw_polygon_ && (nFlags & MK_LBUTTON) != 0) // left button pressed and move
        {
            //draw_polygon_->AddPoint(point);
            //SelectTool::instance().resize_handle = draw_polygon_->GetHandleCount();
            //SelectTool::instance().last_point = point;
            //pCell->select_object_->CreateGeometry();
        }
        else
        {
            SetCursor(LoadCursor(nullptr, IDC_CROSS));
            SelectTool::instance().OnMouseMove(pCell, nFlags, point);
        }
    }

    void OnLButtonDblClk(LayoutCell* pCell, UINT, D2D1_POINT_2F) override
    {
        ReleaseCapture();

        auto nPoints = draw_polygon_->point_list_.size();
        if (nPoints > 3)
        {
            // Remove the last point if it's the same as the next to last...
            draw_polygon_->point_list_.pop_back();
            draw_polygon_->CreateGeometry();
        }
        else
        {
            pCell->draw_object_list_.pop_back();
            pCell->select_object_.reset();
        }
        draw_polygon_.reset();
        DrawShapeMode::instance().c_drawShape = DrawShape::Selection;
    }

    void OnCancel(LayoutCell* pCell) override
    {
        DrawTool::OnCancel(pCell);
        draw_polygon_.reset();
    }
};

using DrawLineTool = TDrawTool<D2D1::shapes::line>;
using DrawEllipseTool = TDrawTool<D2D1::shapes::ellipse>;
using DrawRectangleTool = TDrawTool<D2D1::shapes::rectangle>;
using DrawPolygonTool = TDrawTool<D2D1::shapes::polygon>;
using DrawCircleTool = TDrawTool<D2D1::shapes::circle>;

inline
DrawTool* DrawTool::FindTool()
{
    switch (DrawShapeMode::instance().c_drawShape)
    {
    case DrawShape::Selection:
        return &SelectTool::instance();
    case DrawShape::Line:
        return &DrawLineTool::instance();
    case DrawShape::Ellipse:
        return &DrawEllipseTool::instance();
    case DrawShape::Rectangle:
        return &DrawRectangleTool::instance();
    case DrawShape::Polygon:
        return &DrawPolygonTool::instance();
    case DrawShape::Circle:
        return &DrawCircleTool::instance();
    }
    throw;
}

inline void LButtonDownHandler(LayoutCell *pCell, UINT nFlags, CPoint pt)
{
    assert(pCell);
    pCell->lbutton_down = true;
    pCell->last_point = pt;

    auto point = pCell->InvertTransform(pt);
    auto old = DrawShapeMode::instance().c_drawShape;
    DrawTool* pTool = DrawTool::FindTool();
    pTool->OnLButtonDown(pCell, nFlags, point);
    if (old != DrawShapeMode::instance().c_drawShape)
        ClipCursor(nullptr);
}

inline void LButtonUpHandler(LayoutCell *pCell, UINT nFlags, CPoint pt)
{
    assert(pCell);
    pCell->lbutton_down = false;

    auto point = pCell->InvertTransform(pt);
    auto old = DrawShapeMode::instance().c_drawShape;
    DrawTool* pTool = DrawTool::FindTool();
    pTool->OnLButtonUp(pCell, nFlags, point);
    if (old != DrawShapeMode::instance().c_drawShape)
        ClipCursor(nullptr);
}

inline void LButtonDblClkHandler(LayoutCell *pCell, UINT nFlags, CPoint pt)
{
    assert(pCell);
    auto point = pCell->InvertTransform(pt);
    auto old = DrawShapeMode::instance().c_drawShape;
    DrawTool* pTool = DrawTool::FindTool();
    pTool->OnLButtonDblClk(pCell, nFlags, point);
    if (old != DrawShapeMode::instance().c_drawShape)
        ClipCursor(nullptr);
}

inline void MouseMoveHandler(LayoutCell *pCell, UINT nFlags, CPoint pt)
{
    assert(pCell);
    auto point = pCell->InvertTransform(pt);
    DrawTool* pTool = DrawTool::FindTool();
    pTool->OnMouseMove(pCell, nFlags, point);

    if (DrawShapeMode::instance().c_drawShape == DrawShape::Selection || (nFlags & MK_LBUTTON) != 0)
    {
        if (SelectTool::instance().select_mode == SelectMode::NetSelect)
        {
            if (pCell->lbutton_down && pCell->last_point != pt)
            {
                CSize delta = pt - pCell->last_point;

                pCell->matrix_.Offset(delta.cx, delta.cy);
                pCell->AdjustOffset();

                pCell->last_point = pt;
            }
        }
    }
}

