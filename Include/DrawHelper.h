#pragma once

struct WicBitmap
{
    void Load(LPCWSTR lpszFilename)
    {
        CWICImagingFactory2 wicFactory;

        auto decoder = wicFactory.CreateDecoderFromFilename(lpszFilename);
        m_pBitmapFrame = wicFactory.GetFrame(decoder.Get());
    }
    void DrawBitmap(
        ID2D1RenderTarget *pParentTarget,
        Layout::Matrix matrix,
        D2D1_RECT_F rect)
    {
        if (m_pBitmapFrame)
        {
            CWICImagingFactory2 wicFactory;
            auto source = wicFactory.CreateTransformBitmapSource(m_pBitmapFrame.Get(), matrix, rect);

            // Create a Direct2D bitmap from the WIC bitmap.
            HRESULT hr = pParentTarget->CreateBitmapFromWicBitmap(
                source.Get(),
                nullptr,
                m_pBitmap.GetAddressOf());
            ATLENSURE_SUCCEEDED(hr);
        }
        if (m_pBitmap)
        {
            pParentTarget->DrawBitmap(
                m_pBitmap.Get(),
                rect,
                1.0F,
                D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
                );
        }
    }

    ComPtr<IWICBitmapFrameDecode> m_pBitmapFrame;
    ComPtr<ID2D1Bitmap> m_pBitmap;
};
inline void DrawBitmap(
    ID2D1RenderTarget *pParentTarget,
    IWICBitmapFrameDecode *pBitmapSource,
    Layout::Matrix matrix,
    D2D1_RECT_F rect)
{
    assert(pParentTarget);
    assert(pBitmapSource);

    ComPtr<ID2D1Bitmap> pBitmap;

    CWICImagingFactory2 wicFactory;
    auto source = wicFactory.CreateTransformBitmapSource(pBitmapSource, matrix, rect);

    // Create a Direct2D bitmap from the WIC bitmap.
    HRESULT hr = pParentTarget->CreateBitmapFromWicBitmap(
        source.Get(),
        nullptr,
        pBitmap.GetAddressOf());
    ATLENSURE_SUCCEEDED(hr);

    pParentTarget->DrawBitmap(
        pBitmap.Get(),
        rect,
        1.0F,
        D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
        );
}
