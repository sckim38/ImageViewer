#pragma once

#include "D2DXRenderTarget.h"

namespace WIC
{
    struct Stream : Wic::Stream
    {
        void InitializeFromResource(LPCWSTR Name, LPCWSTR Type)
        {
            assert(Name);
            assert(Type);

            // Locate the resource.
            HRSRC imageResHandle = ::FindResourceW(HINST_THISCOMPONENT, Name, Type);
            HRESULT hr = imageResHandle ? S_OK : E_FAIL;
            HR(hr);

            // Load the resource.
            HGLOBAL imageResDataHandle = ::LoadResource(HINST_THISCOMPONENT, imageResHandle);
            hr = imageResDataHandle ? S_OK : E_FAIL;
            HR(hr);

            // Lock it to get a system memory pointer.
            void *imageFile = ::LockResource(imageResDataHandle);
            hr = imageFile ? S_OK : E_FAIL;
            HR(hr);

            // Calculate the size.
            ULONG imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);
            hr = imageFileSize ? S_OK : E_FAIL;
            HR(hr);

            // Initialize the stream with the memory pointer and size.
            InitializeFromMemory(reinterpret_cast<unsigned char*>(imageFile), imageFileSize);
        }
    };
    using BitmapSource = Wic::BitmapSource;

    struct Clipper : BitmapSource
    {
        KENNYKERR_DEFINE_CLASS(Clipper, BitmapSource, IWICBitmapClipper)
    };
    struct Scaler : BitmapSource
    {
        KENNYKERR_DEFINE_CLASS(Scaler, BitmapSource, IWICBitmapScaler);
    };
    struct AdjustSize
    {
        static SizeU Resize(SizeU size, UINT uiWidth, UINT uiHeight)
        {
            assert(uiWidth != 0 && uiHeight != 0);

            if (uiWidth == 0)
            {
                FLOAT scalar = static_cast<FLOAT>(uiHeight) / static_cast<FLOAT>(size.Height);
                uiWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(size.Width));
            }
            else if (uiHeight == 0)
            {
                FLOAT scalar = static_cast<FLOAT>(uiWidth) / static_cast<FLOAT>(size.Width);
                uiHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(size.Height));
            }
            else
            {
                float nPercentW = ((FLOAT)uiWidth / (FLOAT)size.Width);
                float nPercentH = ((FLOAT)uiHeight / (FLOAT)size.Height);

                float nPercent = (nPercentH < nPercentW) ? nPercentH : nPercentW;

                uiWidth = (UINT)(size.Width * nPercent);
                uiHeight = (UINT)(size.Height * nPercent);
            }

            return SizeU(uiWidth, uiHeight);
        }
    };
    struct Factory2 : Wic::Factory2
    {
        KENNYKERR_DEFINE_CLASS(Factory2, Wic::Factory2, IWICImagingFactory2);

        Scaler CreateScaler() const
        {
            Scaler scaler;
            HR((*this)->CreateBitmapScaler(scaler.GetAddressOf()));
            return scaler;
        }
        Clipper CreateClipper() const
        {
            Clipper clipper;
            HR((*this)->CreateBitmapClipper(clipper.GetAddressOf()));
            return clipper;
        }
        Wic::Bitmap CreateBitmapFromHBITMAP(HBITMAP hBitmap, HPALETTE hPalette = nullptr,
            WICBitmapAlphaChannelOption = WICBitmapAlphaChannelOption::WICBitmapUseAlpha)
        {
            HRESULT hr = nullptr == hBitmap ? E_FAIL : S_OK;
            HR(hr);

            Wic::Bitmap bitmap;

            hr = (*this)->CreateBitmapFromHBITMAP(hBitmap, nullptr, WICBitmapUseAlpha, bitmap.GetAddressOf());
            HR(hr);

            DeleteObject(hBitmap);
            return bitmap;
        }
        Scaler Scale(BitmapSource &source, UINT uiWidth, UINT uiHeight,
            WICBitmapInterpolationMode mode = WICBitmapInterpolationMode::WICBitmapInterpolationModeNearestNeighbor)
        {
            auto size = AdjustSize::Resize(source.GetSize(), uiWidth, uiHeight);

            Scaler scaler;
            HR((*this)->CreateBitmapScaler(scaler.GetAddressOf()));
            HR(scaler->Initialize(source.Get(), size.Width, size.Height, mode));
            return scaler;
        }
        Clipper Clip(BitmapSource &source, const WICRect &rect)
        {
            Clipper clipper;
            HR((*this)->CreateBitmapClipper(clipper.GetAddressOf()));
            HRESULT hr = clipper->Initialize(source.Get(), &rect);
            HR(hr);
            return clipper;
        }
        BitmapSource FormatConvert(const BitmapSource &source,
            GUID const & format = GUID_WICPixelFormat32bppPBGRA,
            Wic::BitmapDitherType dither = Wic::BitmapDitherType::None,
            double alphaThresholdPercent = 0.0,
            Wic::BitmapPaletteType paletteTranslate = Wic::BitmapPaletteType::MedianCut)
        {
            auto converter = CreateFormatConverter();
            //converter.Initialize(source, GUID_WICPixelFormat32bppBGR);
            converter.Initialize(source, format, dither, alphaThresholdPercent, paletteTranslate);
            return converter;
        }
    };
    inline auto CreateFactory() -> Factory2
    {
        Factory2 result;

        HR(CoCreateInstance(CLSID_WICImagingFactory,
            result.GetAddressOf()));

        return result;
    }
    struct WicFactory
    {
        Factory2 factory2 = CreateFactory();
    };
    inline Factory2 GetFactory()
    {
        return unique_singleton<WicFactory>::instance().factory2;
    }
    struct BitmapLoader
    {
        static ComPtr<ID2D1Bitmap> LoadBitmap(ID2D1RenderTarget *pTarget, LPCWSTR lpszFilename)
        {
            ComPtr<ID2D1Bitmap> pBitmap;

            auto stream = WIC::GetFactory().CreateStream();
            HR(stream.InitializeFromFilename(lpszFilename));

            auto source = WIC::BitmapLoader::LoadBitmapSource(stream, 0, 0);
            pTarget->CreateBitmapFromWicBitmap(source.Get(), nullptr, pBitmap.GetAddressOf());

            return pBitmap;
        }
        static BitmapSource LoadBitmapSource(const Wic::Stream &stream, UINT uiWidth, UINT uiHeight)
        {
            auto decoder = GetFactory().CreateDecoderFromStream(stream);
            auto frame = decoder.GetFrame();

            BitmapSource source;
            if (uiWidth || uiHeight)
            {
                source = GetFactory().Scale(frame, uiWidth, uiHeight);
            }
            else
            {
                source = frame;
            }
            return GetFactory().FormatConvert(source);// , GUID_WICPixelFormat32bppBGR);
        }

        static BitmapSource LoadBitmapSource(HBITMAP hBitmap, UINT uiWidth, UINT uiHeight)
        {
            HRESULT hr = nullptr == hBitmap ? E_FAIL : S_OK;
            HR(hr);

            Wic::Bitmap bitmap = GetFactory().CreateBitmapFromHBITMAP(hBitmap);

            BitmapSource source;
            if (uiWidth || uiHeight)
            {
                source = GetFactory().Scale(bitmap, uiWidth, uiHeight);
            }
            else
            {
                source = bitmap;
            }

            return GetFactory().FormatConvert(source, GUID_WICPixelFormat32bppBGR);
        }
    };
}



