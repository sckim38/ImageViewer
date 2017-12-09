#pragma once

#include <Windows.h>
#include <algorithm>
#include <cassert>

#include <Unknwn.h>
#include <wincodecsdk.h>

#include <d2d1_1.h>
#include <d3d11_1.h>
#include <DWrite.h>
#include <WinCodec.h>

#include <wrl.h>
#include <ppl.h>

using namespace D2D1;
using Microsoft::WRL::ComPtr;

#pragma comment(lib, "d2d1")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "dwrite")

#include <gdiplus.h>

#pragma comment(lib, "gdiplus")

//#include "singleton.hpp"

// Usefule macros
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

template<COINIT flags = COINIT::COINIT_MULTITHREADED>
class CoInitializer
{
    HRESULT hr_;
public:
    // COINIT::COINIT_APARTMENTTHREADED
    // COINIT::COINIT_MULTITHREADED
    CoInitializer()
    {
        hr_ = CoInitializeEx(nullptr, flags);
    }
    ~CoInitializer()
    {
        if (SUCCEEDED(hr_))
            CoUninitialize();
    }
    operator HRESULT()
    {
        return hr_;
    }
};

template<typename T>
class Factory;

template<typename T, template<typename> class Creator = Factory>
class FactoryHelper
{
//private:
//    Factory() = delete;
//    ~Factory() = delete;
public:
    template<typename ... Args>
    static ComPtr<T> GetFactory(Args... args...)
    {
        if (!factory_)
        {
            concurrency::critical_section::scoped_lock lock(cs_);
            if (!factory_)
            {
                //if (destroyed_)
                //    throw std::exception("dead reference detected!!!");
                ATLENSURE_SUCCEEDED(Creator<T>::Create(&factory_, args...));
            }
        }
        return factory_;
    }
    //static void destroy()
    //{
    //    concurrency::critical_section::scoped_lock lock(cs_);
    //    if (destroyed_)
    //        throw std::exception("dead reference detected!!!");
    //    instance_.reset();
    //    destroyed_ = true;
    //}

private:
    static concurrency::critical_section cs_;
    static ComPtr<T> factory_;
    //static bool destroyed_;
};
template<typename T, template <typename> class Creator> concurrency::critical_section FactoryHelper<T, Creator>::cs_;
template<typename T, template <typename> class Creator> ComPtr<T> FactoryHelper<T, Creator>::factory_;
//template<typename T> bool FactoryHelper<T>::destroyed_ = false;

template<>
class Factory<ID2D1Factory1> : public FactoryHelper<ID2D1Factory1>
{
public:
    static HRESULT Create(ComPtr<ID2D1Factory1>* pID2D1Factory)
    {
        D2D1_FACTORY_OPTIONS options = {};
#if defined(DEBUG) || defined(_DEBUG)
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
        return D2D1CreateFactory(
            D2D1_FACTORY_TYPE_MULTI_THREADED,
            options,
            pID2D1Factory->GetAddressOf());
    }
};

template<>
class Factory<IWICImagingFactory2> : public FactoryHelper<IWICImagingFactory2>
{
public:
    static HRESULT Create(ComPtr<IWICImagingFactory2>* pIWICImagingFactory)
    {
        return CoCreateInstance(
            CLSID_WICImagingFactory2,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(pIWICImagingFactory->GetAddressOf()));
    }
};

template<>
class Factory<IDWriteFactory> : public FactoryHelper<IDWriteFactory>
{
public:
    static HRESULT Create(ComPtr<IDWriteFactory>* pIDWriteFactory)
    {
        return DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(pIDWriteFactory->Get()),
            reinterpret_cast<IUnknown**>(pIDWriteFactory->GetAddressOf()));
    }
};

template<>
class Factory<ID3D11Device> : public FactoryHelper<ID3D11Device>
{
public:
    static HRESULT Create(ComPtr<ID3D11Device>* pD3Device)
    {
        auto hr = Create(D3D_DRIVER_TYPE_HARDWARE, pD3Device->GetAddressOf());
        if (DXGI_ERROR_UNSUPPORTED == hr)
            hr = Create(D3D_DRIVER_TYPE_WARP, pD3Device->GetAddressOf());
        return hr;
    }
private:
    static HRESULT Create(D3D_DRIVER_TYPE const type, ID3D11Device **pD3Device)
    {
        UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
        flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        return D3D11CreateDevice(nullptr,
            type,
            nullptr,
            flags,
            nullptr, 0,
            D3D11_SDK_VERSION,
            pD3Device,
            nullptr,
            nullptr);
    }
};

template<>
class Factory<IDXGIDevice> : public FactoryHelper<IDXGIDevice>
{
public:
    static HRESULT Create(ComPtr<IDXGIDevice>* pDXGIDevice)
    {
        return Factory<ID3D11Device>::GetFactory().As(pDXGIDevice);
    }
};

template<>
class Factory<IDXGIAdapter> : public FactoryHelper<IDXGIAdapter>
{
public:
    static HRESULT Create(ComPtr<IDXGIAdapter>* pDXGIAdapter)
    {
        return Factory<IDXGIDevice>::GetFactory()->GetAdapter(pDXGIAdapter->GetAddressOf());
    }
};

template<>
class Factory<IDXGIFactory2> : public FactoryHelper<IDXGIFactory2>
{
public:
    static HRESULT Create(ComPtr<IDXGIFactory2>* pDXGIFactory2)
    {
        return Factory<IDXGIAdapter>::GetFactory()->GetParent(
            __uuidof(*pDXGIFactory2),
            reinterpret_cast<void **>(pDXGIFactory2->GetAddressOf()));
    }
};


template<>
class Factory<ID2D1Device> : public FactoryHelper<ID2D1Device>
{
public:
    static HRESULT Create(ComPtr<ID2D1Device>* pD2Device)
    {
        return Factory<ID2D1Factory1>::GetFactory()->CreateDevice(
            Factory<IDXGIDevice>::GetFactory().Get(),
            pD2Device->GetAddressOf());
    }
};

template<>
class Factory<IDXGISwapChain1> : public FactoryHelper<IDXGISwapChain1>
{
public:
    static HRESULT Create(
        ComPtr<IDXGISwapChain1>* pDXGISwapChain1,
        HWND hWnd)
    {
        DXGI_SWAP_CHAIN_DESC1 props = {};
        props.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        props.SampleDesc.Count = 1;
        props.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        props.BufferCount = 2;

        return Factory<IDXGIFactory2>::GetFactory()->CreateSwapChainForHwnd(
            Factory<ID3D11Device>::GetFactory().Get(),
            hWnd, &props, nullptr, nullptr,
            pDXGISwapChain1->GetAddressOf());
    }
};

template<>
class Factory<ID2D1DeviceContext> : public FactoryHelper<ID2D1DeviceContext>
{
public:
    static HRESULT Create(ComPtr<ID2D1DeviceContext>* pD2D1DeviceContext)
    {
        return Factory<ID2D1Device>::GetFactory()->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            pD2D1DeviceContext->GetAddressOf());
    }
};

using WIC = Factory<IWICImagingFactory2>;

class WICHelper
{
public:
    static ComPtr<IWICFormatConverter> GetWICFormatConverter(
        IWICBitmapSource *pISource,
        REFWICPixelFormatGUID dstFormat = GUID_WICPixelFormat32bppBGR, //GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherType dither = WICBitmapDitherTypeNone,
        IWICPalette *pIPalette = nullptr,
        double alphaThresholdPercent = 0.f,
        WICBitmapPaletteType paletteTranslate = WICBitmapPaletteTypeCustom)
    {
        ComPtr<IWICFormatConverter> pConverter;

        HRESULT hr = WIC::GetFactory()->CreateFormatConverter(pConverter.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = pConverter->Initialize(
            pISource,                       // Input bitmap to convert
            dstFormat,                      // Destination pixel format
            dither,                         // Specified dither patterm
            pIPalette,                      // Specify a particular palette 
            alphaThresholdPercent,          // Alpha threshold
            paletteTranslate                // Palette translation type
            );
        ATLENSURE_SUCCEEDED(hr);

        return pConverter;
    }
    static void GetSize(
        IWICBitmapDecoder *pIDecoder,
        FLOAT scalar,
        UINT &uiWidth,
        UINT &uiHeight)
    {
        ComPtr<IWICBitmapFrameDecode> pFrame;
        HRESULT hr = pIDecoder->GetFrame(0, pFrame.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        UINT originalWidth, originalHeight;
        hr = pFrame->GetSize(&originalWidth, &originalHeight);
        ATLENSURE_SUCCEEDED(hr);

        uiWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
        uiHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
    }
    static ComPtr<IWICBitmapScaler> GetWICBitmapScaler(
        IWICBitmapSource *pISource,
        UINT uiWidth,
        UINT uiHeight,
        WICBitmapInterpolationMode mode = WICBitmapInterpolationModeNearestNeighbor)
    {
        assert(uiWidth != 0 || uiHeight != 0);

        ComPtr<IWICBitmapScaler> pScaler;

        HRESULT hr = WIC::GetFactory()->CreateBitmapScaler(pScaler.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        // If a new width or height was specified, create an
        // IWICBitmapScaler and use it to resize the image.
        UINT originalWidth, originalHeight;
        hr = pISource->GetSize(&originalWidth, &originalHeight);
        ATLENSURE_SUCCEEDED(hr);

        if (uiWidth == 0)
        {
            FLOAT scalar = static_cast<FLOAT>(uiHeight) / static_cast<FLOAT>(originalHeight);
            uiWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
        }
        else if (uiHeight == 0)
        {
            FLOAT scalar = static_cast<FLOAT>(uiWidth) / static_cast<FLOAT>(originalWidth);
            uiHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
        }

        hr = pScaler->Initialize(
            pISource,
            uiWidth,
            uiHeight,
            mode);
        ATLENSURE_SUCCEEDED(hr);

        return pScaler;
    }
    static ComPtr<IWICBitmapDecoder> GetWICBitmapDecoder(
        LPCWSTR lpszFilename,
        WICDecodeOptions metadataOptions = WICDecodeMetadataCacheOnLoad)
    {
        ComPtr<IWICBitmapDecoder> pDecoder;

        HRESULT hr = WIC::GetFactory()->CreateDecoderFromFilename(
            lpszFilename,
            nullptr,
            GENERIC_READ,
            metadataOptions,
            pDecoder.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);
        return pDecoder;
    }
    static ComPtr<IWICBitmapDecoder> GetWICBitmapDecoder(
        LPCWSTR Name, LPCWSTR Type,
        WICDecodeOptions metadataOptions = WICDecodeMetadataCacheOnLoad)
    {
        // Locate the resource.
        HRSRC imageResHandle = ::FindResourceW(HINST_THISCOMPONENT, Name, Type);
        HRESULT hr = imageResHandle ? S_OK : E_FAIL;
        ATLENSURE_SUCCEEDED(hr);

        // Load the resource.
        HGLOBAL imageResDataHandle = ::LoadResource(HINST_THISCOMPONENT, imageResHandle);
        hr = imageResDataHandle ? S_OK : E_FAIL;
        ATLENSURE_SUCCEEDED(hr);

        // Lock it to get a system memory pointer.
        void *imageFile = ::LockResource(imageResDataHandle);
        hr = imageFile ? S_OK : E_FAIL;
        ATLENSURE_SUCCEEDED(hr);

        // Calculate the size.
        ULONG imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);
        hr = imageFileSize ? S_OK : E_FAIL;
        ATLENSURE_SUCCEEDED(hr);

        // Create a WIC stream to map onto the memory.
        ComPtr<IWICStream> pIWICStream;
        hr = WIC::GetFactory()->CreateStream(pIWICStream.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        // Initialize the stream with the memory pointer and size.
        hr = pIWICStream->InitializeFromMemory(
            reinterpret_cast<unsigned char*>(imageFile),
            imageFileSize);
        ATLENSURE_SUCCEEDED(hr);

        // Create a decoder for the stream.
        ComPtr<IWICBitmapDecoder> pDecoder;
        hr = WIC::GetFactory()->CreateDecoderFromStream(
            pIWICStream.Get(),
            nullptr,
            WICDecodeMetadataCacheOnLoad,
            pDecoder.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        return pDecoder;
    }
    static ComPtr<IWICBitmap> GetWICBitmap(HBITMAP hBitmap)
    {
        HRESULT hr = nullptr == hBitmap ? E_FAIL : S_OK;
        ATLENSURE_SUCCEEDED(hr);

        ComPtr<IWICBitmap> pBitmap;
        hr = WIC::GetFactory()->CreateBitmapFromHBITMAP(
            hBitmap,
            nullptr,
            WICBitmapUseAlpha,
            pBitmap.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        DeleteObject(hBitmap);
        return pBitmap;
    }
    static ComPtr<IWICBitmapSource> GetWICBitmapSource(
        IWICBitmapDecoder *pIDecoder,
        UINT uiWidth,
        UINT uiHeight)
    {
        ComPtr<IWICBitmapFrameDecode> pFrame;
        HRESULT hr = pIDecoder->GetFrame(0, pFrame.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        ComPtr<IWICBitmapSource> pSource = pFrame;
        if (uiWidth || uiHeight)
            pSource = GetWICBitmapScaler(pSource.Get(), uiWidth, uiHeight);
        return GetWICFormatConverter(pSource.Get());// , GUID_WICPixelFormat32bppPBGRA);
    }

    static ComPtr<ID2D1Bitmap> GetD2D1Bitmap(
        ID2D1RenderTarget *pID2D1RenderTarget,
        IWICBitmapSource *pBitmap)
    {
        ComPtr<ID2D1Bitmap> pD2DBitmap;
        HRESULT hr = pID2D1RenderTarget->CreateBitmapFromWicBitmap(
            pBitmap,
            nullptr,
            pD2DBitmap.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        return pD2DBitmap;
    }
    static Gdiplus::Bitmap *GetGdiplusBitmap(IWICBitmapSource *pBitmap)
    {
        HRESULT hr = S_OK;

        UINT width = 0;
        UINT height = 0;

        // Check BitmapSource format
        WICPixelFormatGUID pixelFormat;
        hr = pBitmap->GetPixelFormat(&pixelFormat);
        ATLENSURE_SUCCEEDED(hr);

        hr = (pixelFormat == GUID_WICPixelFormat32bppBGR) ? S_OK : E_FAIL;
        ATLENSURE_SUCCEEDED(hr);

        hr = pBitmap->GetSize(&width, &height);
        ATLENSURE_SUCCEEDED(hr);

        UINT cbStride = 0;
        // Size of a scan line represented in bytes: 4 bytes each pixel
        hr = UIntMult(width, sizeof(Gdiplus::ARGB), &cbStride);
        ATLENSURE_SUCCEEDED(hr);

        UINT cbBufferSize = 0;
        // Size of the image, represented in bytes
        hr = UIntMult(cbStride, height, &cbBufferSize);
        ATLENSURE_SUCCEEDED(hr);

        // Make sure to free previously allocated buffer and bitmap
        BYTE *pbBuffer = new BYTE[cbBufferSize];

        hr = (pbBuffer) ? S_OK : E_FAIL;
        ATLENSURE_SUCCEEDED(hr);

        WICRect rc = { 0, 0, width, height };

        // Extract the image into the GDI+ Bitmap
        hr = pBitmap->CopyPixels(
            &rc,
            cbStride,
            cbBufferSize,
            pbBuffer
            );
        ATLENSURE_SUCCEEDED(hr);

        Gdiplus::Bitmap *pGdiPlusBitmap = new Gdiplus::Bitmap(
                width,
                height,
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

        return pGdiPlusBitmap;
    }

    // Creates a Direct2D bitmap from the specified file name.
    static ComPtr<ID2D1Bitmap> LoadD2D1Bitmap(
        ID2D1RenderTarget *pID2D1RenderTarget,
        LPCWSTR lpszFilename,
        UINT uiWidth = 0,
        UINT uiHeight = 0)
    {
        ComPtr<IWICBitmapDecoder> pDecoder = GetWICBitmapDecoder(lpszFilename);
        ComPtr<IWICBitmapSource> pSource = GetWICBitmapSource(pDecoder.Get(), uiWidth, uiHeight);
        // Create a Direct2D bitmap from the WIC bitmap.
        return GetD2D1Bitmap(pID2D1RenderTarget, pSource.Get());
    }
    // Creates a Direct2D bitmap from the specified file name.
    static ComPtr<ID2D1Bitmap> LoadD2D1Bitmap(
        ID2D1RenderTarget *pID2D1RenderTarget,
        LPCWSTR lpszFilename,
        FLOAT scalar)
    {
        ComPtr<IWICBitmapDecoder> pDecoder = GetWICBitmapDecoder(lpszFilename);

        UINT uiWidth = 0;
        UINT uiHeight = 0;

        WICHelper::GetSize(pDecoder.Get(), scalar, uiWidth, uiHeight);

        ComPtr<IWICBitmapSource> pSource = GetWICBitmapSource(pDecoder.Get(), uiWidth, uiHeight);
        // Create a Direct2D bitmap from the WIC bitmap.
        return GetD2D1Bitmap(pID2D1RenderTarget, pSource.Get());
    }

    static ComPtr<ID2D1Bitmap> LoadD2D1Bitmap(
        ID2D1RenderTarget *pID2D1RenderTarget,
        HBITMAP hBitmap,
        UINT uiWidth,
        UINT uiHeight)
    {
        ComPtr<IWICBitmapSource> pSource = GetWICBitmap(hBitmap);

        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        if (uiWidth || uiHeight)
            pSource = GetWICBitmapScaler(pSource.Get(), uiWidth, uiHeight);
        pSource = GetWICFormatConverter(pSource.Get());

        // Create a Direct2D bitmap from the WIC bitmap.
        return GetD2D1Bitmap(pID2D1RenderTarget, pSource.Get());
    }
    // Creates a Direct2D bitmap from the specified resource name and type.
    static ComPtr<ID2D1Bitmap> LoadD2D1Bitmap(
        ID2D1RenderTarget *pID2D1RenderTarget,
        LPCWSTR Name, LPCWSTR Type,
        UINT uiWidth,
        UINT uiHeight)
    {
        ComPtr<IWICBitmapDecoder> pDecoder = GetWICBitmapDecoder(Name, Type);
        ComPtr<IWICBitmapSource> pSource = GetWICBitmapSource(pDecoder.Get(), uiWidth, uiHeight);

        // Create a Direct2D bitmap from the WIC bitmap.
        return GetD2D1Bitmap(pID2D1RenderTarget, pSource.Get());
    }
    static Gdiplus::Bitmap *LoadGdiplusBitmap(
        LPCWSTR lpszFilename,
        UINT uiWidth,
        UINT uiHeight)
    {
        ComPtr<IWICBitmapDecoder> pDecoder = GetWICBitmapDecoder(lpszFilename);
        ComPtr<IWICBitmapSource> pSource = GetWICBitmapSource(pDecoder.Get(), uiWidth, uiHeight);

        // Create a Direct2D bitmap from the WIC bitmap.
        return GetGdiplusBitmap(pSource.Get());
    }
    static Gdiplus::Bitmap *LoadGdiplusBitmap(
        LPCWSTR lpszFilename,
        FLOAT scalar)
    {
        ComPtr<IWICBitmapDecoder> pDecoder = WICHelper::GetWICBitmapDecoder(lpszFilename);

        UINT uiWidth = 0;
        UINT uiHeight = 0;

        WICHelper::GetSize(pDecoder.Get(), scalar, uiWidth, uiHeight);
        ComPtr<IWICBitmapSource> pSource = WICHelper::GetWICBitmapSource(pDecoder.Get(), uiWidth, uiHeight);

        // Create a Direct2D bitmap from the WIC bitmap.
        return WICHelper::GetGdiplusBitmap(pSource.Get());
    }
    static Gdiplus::Bitmap *LoadGdiplusBitmapThumbnail(LPCWSTR lpszFilename, FLOAT scalar=0.05F)
    {
        ComPtr<IWICBitmapDecoder> pDecoder = WICHelper::GetWICBitmapDecoder(lpszFilename);

        ComPtr<IWICBitmapSource> pSource;
        HRESULT hr = pDecoder->GetThumbnail(pSource.GetAddressOf());
        if (FAILED(hr))
        {
            hr = pDecoder->GetPreview(pSource.GetAddressOf());
            if (FAILED(hr))
            {
                UINT uiWidth = 0;
                UINT uiHeight = 0;
                WICHelper::GetSize(pDecoder.Get(), scalar, uiWidth, uiHeight);
                pSource = WICHelper::GetWICBitmapSource(pDecoder.Get(), uiWidth, uiHeight);
            }
        }
        //ATLENSURE_SUCCEEDED(hr);

        // Create a Direct2D bitmap from the WIC bitmap.
        return WICHelper::GetGdiplusBitmap(pSource.Get());
    }

};

class PictureHelper
{
public:
    static HBITMAP CreateDIBSectionFromWicBitmap(IWICBitmapSource *pBitmap)
    {
        // Check BitmapSource format
        WICPixelFormatGUID pixelFormat;
        HRESULT hr = pBitmap->GetPixelFormat(&pixelFormat);
        ATLENSURE_SUCCEEDED(hr);

        hr = (pixelFormat == GUID_WICPixelFormat32bppPBGRA) ? S_OK : E_FAIL;
        ATLENSURE_SUCCEEDED(hr);

        UINT width, height;
        hr = pBitmap->GetSize(&width, &height);
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
        hr = pBitmap->CopyPixels(
            NULL,
            cbStride,
            cbImage,
            reinterpret_cast<BYTE *> (pvImageBits));

        // Image Extraction failed, clear allocated memory
        if (FAILED(hr))
        {
            DeleteObject(hDIBBitmap);
            hDIBBitmap = NULL;
        }
        ATLENSURE_SUCCEEDED(hr);

        return hDIBBitmap;
    }
    static HBITMAP GetHBITMAPFromPicture(IPictureDisp *pPictureDisp)
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
    static ComPtr<IPictureDisp> LoadPictureFromResource(LPCWSTR Name, LPCWSTR Type)
    {
        // Load a bitmap
        ComPtr<IWICBitmapDecoder> pDecoder = WICHelper::GetWICBitmapDecoder(Name, Type);
        ComPtr<IWICBitmapFrameDecode> pFrame;
        HRESULT hr = pDecoder->GetFrame(0, pFrame.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        ComPtr<IWICBitmapSource> pSource = WICHelper::GetWICFormatConverter(pFrame.Get());
        HBITMAP hBitmap = PictureHelper::CreateDIBSectionFromWicBitmap(pSource.Get());

        // Create the IPictureDisp from the bitmap handle
        return PictureHelper::CreatePicture(hBitmap);
    }
    static ComPtr<IPictureDisp> LoadPictureFromFile(LPCWSTR lpszFilename)
    {
        // Load a bitmap
        ComPtr<IWICBitmapDecoder> pDecoder = WICHelper::GetWICBitmapDecoder(lpszFilename);
        ComPtr<IWICBitmapFrameDecode> pFrame;
        HRESULT hr = pDecoder->GetFrame(0, pFrame.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        ComPtr<IWICBitmapSource> pSource = WICHelper::GetWICFormatConverter(pFrame.Get());
        HBITMAP hBitmap = PictureHelper::CreateDIBSectionFromWicBitmap(pSource.Get());

        // Create the IPictureDisp from the bitmap handle
        return PictureHelper::CreatePicture(hBitmap);
    }
};


namespace D2D1
{
    D2D1_SIZE_F operator+(D2D1_POINT_2F first, D2D1_POINT_2F second)
    {
        return D2D1_SIZE_F{ first.x + second.x, first.y + second.y };
    }
    D2D1_SIZE_F operator-(D2D1_POINT_2F first, D2D1_POINT_2F second)
    {
        return D2D1_SIZE_F{ first.x - second.x, first.y - second.y };
    }
    D2D1_SIZE_F &operator+=(D2D1_SIZE_F &first, D2D1_SIZE_F second)
    {
        first.width += second.width;
        first.height += second.height;
        return first;
    }
    D2D1_SIZE_F &operator-=(D2D1_SIZE_F &first, D2D1_SIZE_F second)
    {
        first.width -= second.width;
        first.height -= second.height;
        return first;
    }
    bool operator==(D2D1_POINT_2F first, D2D1_POINT_2F second)
    {
        return (first.x == second.x && first.y == second.y);
    }
    bool operator!=(D2D1_POINT_2F first, D2D1_POINT_2F second)
    {
        return !(first == second);
    }

    D2D1_RECT_F operator+(D2D1_RECT_F rect, D2D1_SIZE_F delta)
    {
        rect.left += delta.width;
        rect.right += delta.width;
        rect.top += delta.height;
        rect.bottom += delta.height;
        return rect;
    }
    bool operator==(D2D1_RECT_F first, D2D1_RECT_F second)
    {
        return (first.left == second.left
            && first.top == second.top
            && first.right == second.right
            && first.bottom == second.bottom);
    }
    bool operator!=(D2D1_RECT_F first, D2D1_RECT_F second)
    {
        return !(first == second);
    }

    template<typename T>
    D2D1_POINT_2F Point2F(T x, T y)
    {
        return Point2<FLOAT>(static_cast<FLOAT>(x), static_cast<FLOAT>(y));
    }
    template<typename PointT>
    D2D1_POINT_2F Point2F(PointT point)
    {
        return Point2<FLOAT>(static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y));
    }
    template<typename RectT>
    D2D1_SIZE_F SizeF(RectT rect)
    {
        return Size<FLOAT>(static_cast<FLOAT>(rect.right - rect.left), static_cast<FLOAT>(rect.bottom - rect.top));
    }
    template<typename RectT>
    D2D1_RECT_F RectF(RectT rect)
    {
        return Rect<FLOAT>(
            static_cast<FLOAT>(rect.left),
            static_cast<FLOAT>(rect.top),
            static_cast<FLOAT>(rect.right),
            static_cast<FLOAT>(rect.bottom));
    }
    template<typename PointT>
    D2D1_RECT_F RectF(PointT point1, PointT point2)
    {
        return Rect<FLOAT>(
            static_cast<FLOAT>(point1.x),
            static_cast<FLOAT>(point1.y),
            static_cast<FLOAT>(point2.x),
            static_cast<FLOAT>(point2.y));
    }
    template<typename PointT, typename SizeT>
    D2D1_RECT_F RectF(PointT point, SizeT size)
    {
        return Rect<FLOAT>(
            static_cast<FLOAT>(point.x),
            static_cast<FLOAT>(point.y),
            static_cast<FLOAT>(point.x + size.width),
            static_cast<FLOAT>(point.y + size.height));
    }
}

namespace D2D1
{
    static auto LinearGradientBrush(
        ID2D1RenderTarget *pRenderTarget,
        D2D1_COLOR_F BorderColor = D2D1::ColorF(D2D1::ColorF::Blue),
        D2D1_COLOR_F CenterColor = D2D1::ColorF(D2D1::ColorF::Indigo)) -> ComPtr<ID2D1LinearGradientBrush>
    {
        D2D1_GRADIENT_STOP gradientStops[2];

        gradientStops[0] = D2D1_GRADIENT_STOP{ 0.F, BorderColor };
        gradientStops[1] = D2D1_GRADIENT_STOP{ 1.F, CenterColor };

        ComPtr<ID2D1GradientStopCollection> pGradientStops;

        HRESULT hr = pRenderTarget->CreateGradientStopCollection(
            gradientStops,
            ARRAYSIZE(gradientStops),
            D2D1_GAMMA_2_2,
            D2D1_EXTEND_MODE_CLAMP,
            pGradientStops.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        auto size = pRenderTarget->GetSize();
        auto brushProperties =
            D2D1::LinearGradientBrushProperties(D2D1::Point2F(), D2D1::Point2F(size.width, size.height));

        ComPtr<ID2D1LinearGradientBrush> pLinearGradientBrush;
        hr = pRenderTarget->CreateLinearGradientBrush(
            brushProperties,
            pGradientStops.Get(),
            pLinearGradientBrush.GetAddressOf()
            );
        ATLENSURE_SUCCEEDED(hr);

        return pLinearGradientBrush;
    }
    static auto GridPatternBrush(
        ID2D1RenderTarget *pRenderTarget,
        D2D1_SIZE_F size = D2D1::SizeF(10.F, 10.F),
        FLOAT thickness = (1.F),
        D2D1_COLOR_F color = D2D1::ColorF(0.93f, 0.94f, 0.96f)) -> ComPtr<ID2D1BitmapBrush>
    {
        // Create a compatible render target.
        ComPtr<ID2D1BitmapRenderTarget> compatibleRenderTarget;
        HRESULT hr = pRenderTarget->CreateCompatibleRenderTarget(
            size,
            compatibleRenderTarget.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        // Draw a pattern.
        ComPtr<ID2D1SolidColorBrush> gridBrush;
        compatibleRenderTarget->CreateSolidColorBrush(color, gridBrush.GetAddressOf());

        compatibleRenderTarget->BeginDraw();
        compatibleRenderTarget->FillRectangle(D2D1::RectF(0.F, 0.F, size.width, thickness), gridBrush.Get());
        compatibleRenderTarget->FillRectangle(D2D1::RectF(0.F, thickness, thickness, size.height), gridBrush.Get());
        compatibleRenderTarget->EndDraw();

        // Choose the tiling mode for the bitmap brush.
        auto brushProperties =
            D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP);

        // Create the bitmap brush.
        ComPtr<ID2D1Bitmap> pBitmap;
        // Retrieve the bitmap from the render target.
        compatibleRenderTarget->GetBitmap(pBitmap.GetAddressOf());

        ComPtr<ID2D1BitmapBrush> pGridPatternBitmapBrush;
        pRenderTarget->CreateBitmapBrush(pBitmap.Get(), brushProperties, pGridPatternBitmapBrush.GetAddressOf());

        return pGridPatternBitmapBrush;
    }
    template<typename RenderFunc>
    static ComPtr<ID2D1Bitmap> CompatibleRenderBitmap(ID2D1RenderTarget *pRenderTarget, RenderFunc render)
    {
        // Create a compatible render target.
        ComPtr<ID2D1BitmapRenderTarget> compatibleRenderTarget;
        HRESULT hr = pRenderTarget->CreateCompatibleRenderTarget(compatibleRenderTarget.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        compatibleRenderTarget->BeginDraw();
        render(compatibleRenderTarget.Get());
        compatibleRenderTarget->EndDraw();

        // Create the bitmap brush.
        ComPtr<ID2D1Bitmap> pBitmap;

        // Retrieve the bitmap from the render target.
        compatibleRenderTarget->GetBitmap(pBitmap.GetAddressOf());
        return pBitmap;
    }
};



