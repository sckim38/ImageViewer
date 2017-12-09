#pragma once

template<typename RenderFunc>
ComPtr<ID2D1Bitmap> CreateCompatibleD2DBitmap(ID2D1RenderTarget *pRT, CSize size, RenderFunc func)
{
    // Create a compatible render target.
    ComPtr<ID2D1BitmapRenderTarget> cRT;
    HRESULT hr = pRT->CreateCompatibleRenderTarget(D2D1::SizeF(size), cRT.GetAddressOf());
    if (SUCCEEDED(hr))
    {
        cRT->BeginDraw();
        {
            func(cRT.Get());
        }
        HRESULT hr = cRT->EndDraw();
        if (SUCCEEDED(hr))
        {
            // Retrieve the bitmap from the render target.
            ComPtr<ID2D1Bitmap> pBitmap;
            cRT->GetBitmap(pBitmap.GetAddressOf());
            return pBitmap;
        }
    }
    return nullptr;
}
