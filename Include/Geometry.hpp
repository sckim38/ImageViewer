#pragma once

class DrawCircle;
namespace D2D1
{
    inline ID2D1Factory *GetFactory()
    {
        return AfxGetD2DState()->GetDirect2dFactory();
    }
    enum class shapes { line, ellipse, rectangle, polygon, circle };

    template<shapes type>
    class Geometry;

    template<>
    class Geometry<shapes::line>
    {
    public:
        static auto Create(const D2D1_RECT_F &rect)->ComPtr<ID2D1Geometry>
        {
            ComPtr<ID2D1PathGeometry> pGeometry;
            HRESULT hr = D2D1::GetFactory()->CreatePathGeometry(pGeometry.GetAddressOf());
            ATLENSURE_SUCCEEDED(hr);

            ComPtr<ID2D1GeometrySink> pSink;
            hr = pGeometry->Open(pSink.GetAddressOf());
            ATLENSURE_SUCCEEDED(hr);

            pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

            pSink->BeginFigure(
                D2D1::Point2F(rect.left, rect.top),
                D2D1_FIGURE_BEGIN_FILLED);
            pSink->AddLine(D2D1::Point2F(rect.right, rect.bottom));

            if (rect.top < rect.bottom)
                pSink->AddLine(D2D1::Point2F(rect.left, rect.bottom));
            else
                pSink->AddLine(D2D1::Point2F(rect.right, rect.top));

            pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

            hr = pSink->Close();
            ATLENSURE_SUCCEEDED(hr);

            return pGeometry;
        }
    };
    template<>
    class Geometry<shapes::ellipse>
    {
    public:
        static auto Create(const D2D1_RECT_F &rect)->ComPtr<ID2D1Geometry>
        {
            auto ellipse = D2D1::Ellipse(
                D2D1::Center(rect),
                D2D1::SizeF(rect).width / 2.F,
                D2D1::SizeF(rect).height / 2.F);

            ComPtr<ID2D1EllipseGeometry> pGeometry;
            auto hr = D2D1::GetFactory()->CreateEllipseGeometry(ellipse, pGeometry.GetAddressOf());
            ATLENSURE_SUCCEEDED(hr);

            return pGeometry;
        }
    };
    template<>
    class Geometry<shapes::rectangle>
    {
    public:
        static auto Create(const D2D1_RECT_F &rect)->ComPtr<ID2D1Geometry>
        {
            ComPtr<ID2D1RectangleGeometry> pGeometry;
            auto hr = D2D1::GetFactory()->CreateRectangleGeometry(rect, pGeometry.GetAddressOf());
            ATLENSURE_SUCCEEDED(hr);

            return pGeometry;
        }
    };
    template<>
    class Geometry<shapes::polygon>
    {
    public:
        static auto Create(std::vector<D2D1_POINT_2F> polygon)->ComPtr<ID2D1Geometry>
        {
            ComPtr<ID2D1PathGeometry> pGeometry;
            auto hr = D2D1::GetFactory()->CreatePathGeometry(pGeometry.GetAddressOf());
            ATLENSURE_SUCCEEDED(hr);

            ComPtr<ID2D1GeometrySink> pSink;
            hr = pGeometry->Open(pSink.GetAddressOf());
            ATLENSURE_SUCCEEDED(hr);

            pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

            auto it = std::begin(polygon);
            pSink->BeginFigure(*it, D2D1_FIGURE_BEGIN_FILLED);
            it++;
            for (; it != std::end(polygon); it++)
                pSink->AddLine(*it);
            pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

            hr = pSink->Close();
            ATLENSURE_SUCCEEDED(hr);

            return pGeometry;
        }
    };
    template<>
    class Geometry<shapes::circle>
    {
        friend class DrawCircle;
        static void circumcenter(const D2D1_POINT_2F &p1, const D2D1_POINT_2F &p2, const D2D1_POINT_2F &p3, D2D1_POINT_2F &center, FLOAT &radius)
        {
            float ab = (p1.x * p1.x) + (p1.y * p1.y);
            float cd = (p2.x * p2.x) + (p2.y * p2.y);
            float ef = (p3.x * p3.x) + (p3.y * p3.y);

            center.x = (ab * (p3.y - p2.y) + cd * (p1.y - p3.y) + ef * (p2.y - p1.y)) / (p1.x * (p3.y - p2.y) + p2.x * (p1.y - p3.y) + p3.x * (p2.y - p1.y)) / 2.f;
            center.y = (ab * (p3.x - p2.x) + cd * (p1.x - p3.x) + ef * (p2.x - p1.x)) / (p1.y * (p3.x - p2.x) + p2.y * (p1.x - p3.x) + p3.y * (p2.x - p1.x)) / 2.f;
            radius = sqrtf(((p1.x - center.x) * (p1.x - center.x)) + ((p1.y - center.y) * (p1.y - center.y)));
        }
    public:
        static auto Create(const D2D1_POINT_2F &p1, const D2D1_POINT_2F &p2, const D2D1_POINT_2F &p3)->ComPtr<ID2D1Geometry>
        {
            D2D1_POINT_2F center;
            FLOAT radius;
            circumcenter(p1, p2, p3, center, radius);

            auto ellipse = D2D1::Ellipse(
                center,
                radius,
                radius);

            ComPtr<ID2D1EllipseGeometry> pGeometry;
            auto hr = D2D1::GetFactory()->CreateEllipseGeometry(ellipse, pGeometry.GetAddressOf());
            ATLENSURE_SUCCEEDED(hr);

            return pGeometry;
        }
    };
}
