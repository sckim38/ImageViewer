#pragma once

#include <gdiplus.h>
#pragma comment(lib, "gdiplus")

#include "WicImageFactory2.h"

struct GdiplusHelper
{
    static auto CreateGdiplusBitmap(
        IWICBitmapSource *source
        )->std::shared_ptr<Gdiplus::Bitmap>
    {
        HRESULT hr = S_OK;

        // Check BitmapSource format
        //WICPixelFormatGUID pixelFormat;
        //hr = (*this)->GetPixelFormat(&pixelFormat);
        //ATLENSURE_SUCCEEDED(hr);

        //hr = (pixelFormat == GUID_WICPixelFormat32bppBGR) ? S_OK : E_FAIL;
        //ATLENSURE_SUCCEEDED(hr);

        auto size = D2D1::SizeU(source);// ->GetSize();

        UINT cbStride = 0;
        // Size of a scan line represented in bytes: 4 bytes each pixel
        hr = UIntMult(size.width, sizeof(Gdiplus::ARGB), &cbStride);
        ATLENSURE_SUCCEEDED(hr);

        UINT cbBufferSize = 0;
        // Size of the image, represented in bytes
        hr = UIntMult(cbStride, size.height, &cbBufferSize);
        ATLENSURE_SUCCEEDED(hr);

        // Make sure to free previously allocated buffer and bitmap
        BYTE *pbBuffer = new BYTE[cbBufferSize];

        hr = (pbBuffer) ? S_OK : E_FAIL;
        ATLENSURE_SUCCEEDED(hr);

        WICRect rc = { 0, 0, size.width, size.height };

        // Extract the image into the GDI+ Bitmap
        hr = source->CopyPixels(
            &rc,
            cbStride,
            cbBufferSize,
            pbBuffer
            );
        ATLENSURE_SUCCEEDED(hr);

        Gdiplus::Bitmap *pGdiPlusBitmap = new Gdiplus::Bitmap(
            size.width,
            size.height,
            cbStride,
            PixelFormat32bppRGB,
            pbBuffer
            );

        hr = pGdiPlusBitmap ? S_OK : E_FAIL;
        ATLENSURE_SUCCEEDED(hr);

        if (pGdiPlusBitmap->GetLastStatus() != Gdiplus::Ok)
        {
            delete pGdiPlusBitmap;
            pGdiPlusBitmap = nullptr;
            hr = E_FAIL;
        }
        ATLENSURE_SUCCEEDED(hr);

        return std::shared_ptr<Gdiplus::Bitmap>(pGdiPlusBitmap);
    }
    static std::shared_ptr<Gdiplus::Bitmap> LoadGdiplusBitmap(
        LPCWSTR lpszFilename,
        UINT uiWidth,
        UINT uiHeight)
    {
        auto source = CWICImagingFactory2::LoadBitmapFromFile(lpszFilename, uiWidth, uiHeight);

        return GdiplusHelper::CreateGdiplusBitmap(source.Get());
    }

    //static std::shared_ptr<Gdiplus::Bitmap> LoadGdiplusBitmapThumbnail(
    //    LPCWSTR lpszFilename,
    //    UINT uiWidth,
    //    UINT uiHeight)
    //{
    //    auto factory = GetFactory();
    //    auto stream = factory.CreateStream();
    //    stream.InitializeFromFilename(lpszFilename);
    //    auto decoder = factory.CreateDecoderFromStream(stream);
    //    auto frame = decoder.GetFrame(0);

    //    BitmapSource source;
    //    HRESULT hr = E_FAIL;
    //    //hr = frame->GetThumbnail(source.GetAddressOf());
    //    if (FAILED(hr))
    //    {
    //        hr = decoder->GetPreview(source.GetAddressOf());
    //        if (FAILED(hr))
    //        {
    //            if (uiWidth || uiHeight)
    //            {
    //                source = GetFactory().Scale(frame, uiWidth, uiHeight);
    //            }
    //            else
    //            {
    //                source = frame;
    //            }

    //            source = GetFactory().FormatConvert(source, GUID_WICPixelFormat32bppBGR);
    //        }
    //    }
    //    return GdiplusHelper::CreateGdiplusBitmap(source);
    //}
};

class PictureHelper
{
public:
    static HBITMAP CreateHBITMAP(IWICBitmapSource *source)
    {
        // Check BitmapSource format
        WICPixelFormatGUID pixelFormat;
        HRESULT hr = source->GetPixelFormat(&pixelFormat);
        ATLENSURE_SUCCEEDED(hr);

        hr = (pixelFormat == GUID_WICPixelFormat32bppBGR) ? S_OK : E_FAIL;
        ATLENSURE_SUCCEEDED(hr);

        UINT width, height;
        hr = source->GetSize(&width, &height);
        ATLENSURE_SUCCEEDED(hr);

        // Create a DIB section based on Bitmap Info
        // BITMAPINFO Struct must first be setup before a DIB can be created.
        // Note that the height is negative for top-down bitmaps
        BITMAPINFO bminfo{};
        //ZeroMemory(&bminfo, sizeof(bminfo));
        bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bminfo.bmiHeader.biWidth = width;
        bminfo.bmiHeader.biHeight = -(LONG)height;
        bminfo.bmiHeader.biPlanes = 1;
        bminfo.bmiHeader.biBitCount = 32;
        bminfo.bmiHeader.biCompression = BI_RGB;

        // Get a DC for the full screen
        HDC hdcScreen = GetDC(NULL);

        hr = hdcScreen ? S_OK : E_FAIL;
        ATLENSURE_SUCCEEDED(hr);

        void *pvImageBits = nullptr;
        HBITMAP hDIBBitmap = CreateDIBSection(
            hdcScreen,
            &bminfo,
            DIB_RGB_COLORS,
            &pvImageBits,
            NULL,
            0);

        ReleaseDC(NULL, hdcScreen);

        hr = hDIBBitmap ? S_OK : E_FAIL;
        ATLENSURE_SUCCEEDED(hr);

        UINT cbStride = 0;
        // Size of a scan line represented in bytes: 4 bytes each pixel
        hr = UIntMult(width, sizeof(DWORD), &cbStride);
        ATLENSURE_SUCCEEDED(hr);

        UINT cbImage = 0;
        // Size of the image, represented in bytes
        hr = UIntMult(cbStride, height, &cbImage);
        ATLENSURE_SUCCEEDED(hr);

        // Extract the image into the HBITMAP    
        hr = source->CopyPixels(
            NULL,
            cbStride,
            cbImage,
            reinterpret_cast<BYTE *> (pvImageBits));

        // Copy the bitmap to the target device context
        //::SetDIBitsToDevice(hDC, x, y, cx, cy, 0, 0, 0, cy, pvImageBits, &bminfo, DIB_RGB_COLORS);

        // Image Extraction failed, clear allocated memory
        if (FAILED(hr))
        {
            DeleteObject(hDIBBitmap);
            hDIBBitmap = NULL;
        }
        ATLENSURE_SUCCEEDED(hr);

        return hDIBBitmap;
    }
    static HBITMAP CreateHBITMAP(IPictureDisp *pPictureDisp)
    {
        ComPtr<IPicture> pPicture;
        pPictureDisp->QueryInterface(__uuidof(IPicture), (void**)pPicture.GetAddressOf());

        // IPicture -> D2D1Bitmap
        HBITMAP hBitmap;
        HRESULT hr = pPicture->get_Handle((OLE_HANDLE*)&hBitmap);
        ATLENSURE_SUCCEEDED(hr);

        return hBitmap;
    }
    static ComPtr<IPictureDisp> CreatePicture(HBITMAP hBitmap)
    {
        // Create the IPicture from the bitmap handle
        ComPtr<IPicture> pPicture;
        PICTDESC pictureDesc = {};

        pictureDesc.cbSizeofstruct = sizeof(pictureDesc);
        pictureDesc.picType = PICTYPE_BITMAP;
        pictureDesc.bmp.hbitmap = hBitmap;
        pictureDesc.bmp.hpal = 0;

        HRESULT hr = ::OleCreatePictureIndirect(&pictureDesc, IID_IPicture, FALSE,
            reinterpret_cast<void **>(pPicture.GetAddressOf()));
        ATLENSURE_SUCCEEDED(hr);

        ComPtr<IPictureDisp> pPictureDisp;
        hr = pPicture.As(&pPictureDisp);
        ATLENSURE_SUCCEEDED(hr);

        return pPictureDisp;
    }
};


