#pragma once

static const FLOAT resol = 0.026458333F;
// image resolution * length
// (1 / (96 dpi)) * 2.54 = 0.026458333 cm

struct LengthOfPoints
{
    static FLOAT Length(D2D1_RECT_F rect)
    {
        FLOAT w = std::abs(rect.right - rect.left);
        FLOAT h = std::abs(rect.bottom - rect.top);
        FLOAT l = sqrtf(pow(w, 2) + pow(h, 2));

        return l;
    }
    static FLOAT Length(D2D1_POINT_2F point1, D2D1_POINT_2F point2)
    {
        FLOAT w = std::abs(point2.x - point1.x);
        FLOAT h = std::abs(point2.y - point1.y);
        FLOAT l = sqrtf(pow(w, 2) + pow(h, 2));

        return l;
    }
    /**
    * Determines the angle of a straight line drawn between point one and two. The number returned, which is a float in degrees, tells us how much we have to rotate a horizontal line clockwise for it to match the line between the two points.
    * If you prefer to deal with angles using radians instead of degrees, just change the last line to: "return Math.Atan2(yDiff, xDiff);"
    */
    static float GetAngleOfLineBetweenTwoPoints(D2D1_POINT_2F p1, D2D1_POINT_2F p2)
    {
        float xDiff = p2.x - p1.x;
        float yDiff = p2.y - p1.y;
        float angle = float(std::atan2(yDiff, xDiff) * (180 / M_PI));
        if (angle >= 90.F)
            angle -= 180.F;
        if (angle < -90.F)
            angle += 180.F;
        return angle;
    }
    static void DrawLength(ID2D1RenderTarget *pRT, ID2D1SolidColorBrush *pBrush, D2D1_POINT_2F point1, D2D1_POINT_2F point2)
    {
        FLOAT angle = GetAngleOfLineBetweenTwoPoints(point1, point2);
        WCHAR buf[BUFSIZ];
        swprintf_s(buf, L"%.3f(um) %.2f", Length(point1, point2) * resol, angle);

        CTextFormat text_format;
        text_format.CreateTextFormat();

        auto size = text_format.GetTextLayoutSize(buf);
        auto center = D2D1::Center(point1, point2);

        auto corner = D2D1::Point2F(center.x - size.width / 2.F, center.y - size.height / 2.F);
        auto rect = D2D1::RectF(corner.x, corner.y, corner.x + size.width, corner.y + size.height);

        D2D1_MATRIX_3X2_F matrix;
        pRT->GetTransform(&matrix);

        auto matrix2 = D2D1::Matrix3x2F::Rotation(angle, center);
        pRT->SetTransform(matrix2 * matrix); // rotate, transition

        pRT->DrawText(buf, (UINT32)wcslen(buf), text_format.Get(), rect, pBrush);

        pRT->SetTransform(matrix);
    }
    static void DrawLength(ID2D1RenderTarget *pRT, ID2D1SolidColorBrush *pBrush, D2D1_RECT_F rect)
    {
        DrawLength(pRT, pBrush, D2D1::Point2F(rect.left, rect.top), D2D1::Point2F(rect.right, rect.bottom));
    }
};

class DrawObject
{
    // Constructors
public:
    virtual ~DrawObject() {}

    // Attributes
    D2D1_RECT_F rect_ = D2D1_RECT_F{};
    ComPtr<ID2D1Geometry> geometry_;
    ComPtr<ID2D1Bitmap> bitmap_;

    virtual auto CreateGeometry()->void = 0;
    virtual auto DrawGeometry(
        ID2D1RenderTarget *pRenderTarget,
        ComPtr<ID2D1SolidColorBrush> fill_brush,
        ComPtr<ID2D1SolidColorBrush> line_brush,
        FLOAT strok_width = 2.F)->void
    {
        if (!geometry_)
            return;

        pRenderTarget->FillGeometry(geometry_.Get(), fill_brush.Get());
        pRenderTarget->DrawGeometry(geometry_.Get(), line_brush.Get(), strok_width);
    }
    virtual auto DrawLength(
        ID2D1RenderTarget *pRenderTarget,
        ComPtr<ID2D1SolidColorBrush> text_brush)->void
    {
        FLOAT left = (std::min)(rect_.left, rect_.right);
        FLOAT bottom = (std::max)(rect_.top, rect_.bottom);
        LengthOfPoints::DrawLength(pRenderTarget, text_brush.Get(), D2D1::Point2F(left, rect_.top), D2D1::Point2F(left, rect_.bottom));
        LengthOfPoints::DrawLength(pRenderTarget, text_brush.Get(), D2D1::Point2F(rect_.left, bottom), D2D1::Point2F(rect_.right, bottom));
    }
    auto HitTest(D2D1_POINT_2F point, FLOAT strok_width = 2.F)->int
    {
        auto matrix = D2D1::Matrix3x2F::Identity();
        BOOL contains = FALSE;
        HRESULT hr = S_OK;

        int nHandleCount = GetHandleCount();
        for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
        {
            // GetHandleRect returns in logical coords
            auto rc = GetHandleRect(nHandle);
            ComPtr<ID2D1RectangleGeometry> pGeometry;
            D2D1::GetFactory()->CreateRectangleGeometry(rc, pGeometry.GetAddressOf());
            hr = pGeometry->StrokeContainsPoint(point, strok_width, nullptr, matrix, &contains);
            ATLENSURE_SUCCEEDED(hr);
            if (contains)
                return nHandle;
        }
        return 0;
    }
    auto HitTestStroke(D2D1_POINT_2F point, FLOAT strok_width = 2.F)->BOOL
    {
        BOOL contains = FALSE;

        HRESULT hr = geometry_->StrokeContainsPoint(point, strok_width, nullptr, D2D1::Matrix3x2F::Identity(), &contains);
        ATLENSURE_SUCCEEDED(hr);

        return contains;
    }
    auto HitTestFill(D2D1_POINT_2F point)->BOOL
    {
        BOOL contains = FALSE;

        HRESULT hr = geometry_->FillContainsPoint(point, D2D1::Matrix3x2F::Identity(), &contains);
        ATLENSURE_SUCCEEDED(hr);

        return contains;
    }
    virtual auto GetHandleCount()->int
    {
        return 8;
    }
    // returns logical coords of center of handle
    virtual auto GetHandle(int nHandle)->D2D1_POINT_2F
    {
        // this gets the center regardless of left/right and top/bottom ordering
        FLOAT xCenter = rect_.left + (rect_.right - rect_.left) / 2;
        FLOAT yCenter = rect_.top + (rect_.bottom - rect_.top) / 2;

        switch (nHandle)
        {
        case 1:
            return{ rect_.left, rect_.top };
        case 2:
            return{ xCenter, rect_.top };
        case 3:
            return{ rect_.right, rect_.top };
        case 4:
            return{ rect_.right, yCenter };
        case 5:
            return{ rect_.right, rect_.bottom };
        case 6:
            return{ xCenter, rect_.bottom };
        case 7:
            return{ rect_.left, rect_.bottom };
        case 8:
            return{ rect_.left, yCenter };
        default:
            assert(false);
        }
        return{ 0 };
    }
    virtual auto GetHandleRect(int nHandleID)->D2D1_RECT_F
    {
        auto point = GetHandle(nHandleID);
        return D2D1_RECT_F{ point.x - 3, point.y - 3, point.x + 3, point.y + 3 };
    }
    virtual auto GetHandleCursor(int nHandle)->HCURSOR
    {
        LPCTSTR id;
        switch (nHandle)
        {
        default:
            assert(false);

        case 1:
        case 5:
            id = IDC_SIZENWSE;
            break;

        case 2:
        case 6:
            id = IDC_SIZENS;
            break;

        case 3:
        case 7:
            id = IDC_SIZENESW;
            break;

        case 4:
        case 8:
            id = IDC_SIZEWE;
            break;
        }

        return LoadCursor(nullptr, id);
    }
    // Operations
    virtual auto MoveTo(const D2D1_SIZE_F& size)->void
    {
        rect_.left += size.width;
        rect_.right += size.width;
        rect_.top += size.height;
        rect_.bottom += size.height;
    }
    virtual auto MoveTo(const D2D1_RECT_F& rect)->void
    {
        if (rect != rect_)
            rect_ = rect;
    }

    enum TrackerState { normal, selected, active };
    virtual auto DrawTracker(ID2D1RenderTarget *pRenderTarget, TrackerState state = selected)->void
    {
        switch (state)
        {
        case normal:
            break;

        case selected:
        case active:
            for (int nHandle = 1; nHandle <= GetHandleCount(); nHandle += 1)
            {
                auto handle = GetHandle(nHandle);
                //pDC->PatBlt(handle.x - 3, handle.y - 3, 7, 7, DSTINVERT);
            }
            break;
        }
    }

    // rect must be in logical coordinates
    virtual auto MoveHandleTo(int nHandle, D2D1_POINT_2F point)->bool
    {
        ATLTRACE(L"DrawObject::MoveHandle() - nHandle %d, point %f, %f\n", nHandle, point.x, point.y);
        switch (nHandle)
        {
        default:
            assert(false);
            break;
        case 1:
            rect_.left = point.x;
            rect_.top = point.y;
            break;
        case 2:
            rect_.top = point.y;
            break;
        case 3:
            rect_.right = point.x;
            rect_.top = point.y;
            break;
        case 4:
            rect_.right = point.x;
            break;
        case 5:
            rect_.right = point.x;
            rect_.bottom = point.y;
            break;
        case 6:
            rect_.bottom = point.y;
            break;
        case 7:
            rect_.left = point.x;
            rect_.bottom = point.y;
            break;
        case 8:
            rect_.left = point.x;
            break;
        }
        return true;
    }
};
using DrawObjectPtr = std::shared_ptr<DrawObject>;

/////////////////////////////////////////////////////////////////////////////
class DrawLine : public DrawObject
{
public:
    DrawLine(const D2D1_RECT_F &rect)
    {
        rect_ = rect;
    }

    // Implementation
public:
    auto CreateGeometry()->void override
    {
        geometry_ = D2D1::Geometry<D2D1::shapes::line>::Create(rect_);
    }
    auto DrawLength(
        ID2D1RenderTarget *pRenderTarget,
        ComPtr<ID2D1SolidColorBrush> text_brush)->void override
    {
        LengthOfPoints::DrawLength(pRenderTarget, text_brush.Get(), rect_);
    }

    auto GetHandleCount()->int override
    {
        return 2;
    }
    // returns center of handle in logical coordinates
    auto GetHandle(int nHandle)->D2D1_POINT_2F override
    {
        switch (nHandle)
        {
        default:
            assert(false);
        case 1:
            return{ rect_.left, rect_.top };
        case 2:
            return{ rect_.right, rect_.bottom };
        }
        return{ 0 };
    }

    auto GetHandleCursor(int nHandle)->HCURSOR override
    {
        return LoadCursor(nullptr, IDC_SIZENWSE);
    }

    // point is in logical coordinates
    auto MoveHandleTo(int nHandle, D2D1_POINT_2F point)->bool override
    {
        ATLTRACE(L"DrawLine::MoveHandle() - nHandle %d, point %f, %f\n", nHandle, point.x, point.y);
        switch (nHandle)
        {
        default:
            assert(false);
        case 1:
            rect_.left = point.x;
            rect_.top = point.y;
            break;
        case 2:
            rect_.right = point.x;
            rect_.bottom = point.y;
            break;
        }
        return true;
    }
};

/////////////////////////////////////////////////////////////////////////////
class DrawEllipse : public DrawObject
{
public:
    DrawEllipse(const D2D1_RECT_F &rect)
    {
        rect_ = rect;
    }

    // Implementation
public:
    auto CreateGeometry()->void override
    {
        geometry_ = D2D1::Geometry<D2D1::shapes::ellipse>::Create(rect_);
    }
};

/////////////////////////////////////////////////////////////////////////////
class DrawRectangle : public DrawObject
{
public:
    DrawRectangle(const D2D1_RECT_F &rect)
    {
        rect_ = rect;
    }
    // Implementation
public:
    auto CreateGeometry()->void override
    {
        geometry_ = D2D1::Geometry<D2D1::shapes::rectangle>::Create(rect_);
    }
};

/////////////////////////////////////////////////////////////////////////////
class DrawPolygon : public DrawObject
{
public:
    std::vector<D2D1_POINT_2F> point_list_;

    DrawPolygon(const D2D1_RECT_F &rect)
    {
        rect_ = rect;
    }
    // Implementation
public:
    auto CreateGeometry()->void override
    {
        geometry_ = D2D1::Geometry<D2D1::shapes::polygon>::Create(point_list_);
    }
    auto DrawLength(
        ID2D1RenderTarget *pRenderTarget,
        ComPtr<ID2D1SolidColorBrush> text_brush)->void override
    {
    }
    // Operations
    // point is in logical coordinates
    auto AddPoint(D2D1_POINT_2F point)->bool
    {
        point_list_.push_back(point);
        return RecalcBounds();
    }
    auto PopLastPoint()->bool
    {
        point_list_.pop_back();
        return RecalcBounds();
    }

    auto RecalcBounds()->bool
    {
        auto bounds = D2D1::RectF();
        if (geometry_)
            geometry_->GetBounds(D2D1::Matrix3x2F::Identity(), &bounds);
        if (bounds != rect_)
        {
            rect_ = bounds;
            return true;
        }
        return false;
    }
    // Implementation
public:
    // position must be in logical coordinates
    auto MoveTo(const D2D1_RECT_F& rect)->void override
    {
        if (rect == rect_)
            return;

        D2D1_SIZE_F size = { rect.left - rect_.left, rect.top - rect_.top };
        for (auto &pt : point_list_)
        {
            pt.x += size.width;
            pt.y += size.height;
        }

        rect_ = rect;
    }
    auto MoveTo(const D2D1_SIZE_F& size)->void override
    {
        __super::MoveTo(size);
        for (auto &pt : point_list_)
        {
            pt.x += size.width;
            pt.y += size.height;
        }
    }
    auto GetHandleCount()->int override
    {
        return (int)point_list_.size();
    }
    auto GetHandle(int nHandle)->D2D1_POINT_2F override
    {
        assert(nHandle >= 1 && (UINT)nHandle <= point_list_.size());
        return point_list_[nHandle - 1];
    }
    auto GetHandleCursor(int)->HCURSOR override
    {
        return LoadCursor(nullptr, IDC_SIZEALL);
    }
    // point is in logical coordinates
    auto MoveHandleTo(int nHandle, D2D1_POINT_2F point)->bool override
    {
        ATLTRACE(L"DrawPolygon::MoveHandle() - nHandle %d, point %f, %f\n", nHandle, point.x, point.y);
        assert(nHandle >= 1 && (UINT)nHandle <= point_list_.size());
        if (point_list_[nHandle - 1] == point)
            return false;

        point_list_[nHandle - 1] = point;
        RecalcBounds();
        return true;
    }
};


class DrawCircle : public DrawPolygon
{
public:
    DrawCircle(const D2D1_RECT_F &rect)
        : DrawPolygon(rect)
    {
    }
    // Implementation
public:
    auto DrawGeometry(
        ID2D1RenderTarget *pRenderTarget,
        ComPtr<ID2D1SolidColorBrush> fill_brush,
        ComPtr<ID2D1SolidColorBrush> line_brush,
        FLOAT strok_width = 2.F)->void override
    {
        DrawPolygon::DrawGeometry(pRenderTarget, fill_brush, line_brush, strok_width);

        if (point_list_.size() == 3)
        {
            ComPtr<ID2D1Geometry> pGeometry;
            pGeometry = D2D1::Geometry<D2D1::shapes::circle>::Create(point_list_[0], point_list_[1], point_list_[2]);
            pRenderTarget->DrawGeometry(pGeometry.Get(), line_brush.Get(), strok_width);
        }
    }
    auto DrawLength(
        ID2D1RenderTarget *pRenderTarget,
        ComPtr<ID2D1SolidColorBrush> text_brush)->void override
    {
        if (point_list_.size() == 3)
        {
            D2D1_POINT_2F center{};
            FLOAT radius = 0.F;
            D2D1::Geometry<D2D1::shapes::circle>::circumcenter(point_list_[0], point_list_[1], point_list_[2], center, radius);
            if (radius < 0.F)
                return;

            LengthOfPoints::DrawLength(
                pRenderTarget,
                text_brush.Get(),
                D2D1::Point2F(center.x - radius, center.y + radius),
                D2D1::Point2F(center.x + radius, center.y + radius));
        }
    }
};
