#pragma once

// Usefule macros
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class CCoInitializer
{
public:
    explicit CCoInitializer(DWORD dwCoInit = COINIT_APARTMENTTHREADED)
        : _coinitialized(false)
    {
        // Initialize the COM library on the current thread.
        HRESULT hr = CoInitializeEx(nullptr, dwCoInit);
        if (FAILED(hr))
            throw hr;
        _coinitialized = true;
    }
    ~CCoInitializer()
    {
        // Free the COM library.
        if (_coinitialized)
            CoUninitialize();
    }
private:
    // Flags whether COM was properly initialized.
    bool _coinitialized;

    // Hide copy constructor and assignment operator.
    CCoInitializer(const CCoInitializer&);
    CCoInitializer& operator=(const CCoInitializer&);
};

class CWICImagingFactory2
{
public:

    // CreateDecoderFromResource(MAKEINTRESOURCE(IDB_), L"PNG");
    ComPtr<IWICBitmapDecoder> CreateDecoderFromResource(
        LPCWSTR Name,
        LPCWSTR Type,
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
        hr = m_pWICImagingFactory2->CreateStream(pIWICStream.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        // Initialize the stream with the memory pointer and size.
        hr = pIWICStream->InitializeFromMemory(
            reinterpret_cast<unsigned char*>(imageFile),
            imageFileSize);
        ATLENSURE_SUCCEEDED(hr);

        // Create a decoder for the stream.
        ComPtr<IWICBitmapDecoder> pDecoder;
        hr = m_pWICImagingFactory2->CreateDecoderFromStream(
            pIWICStream.Get(),
            nullptr,
            WICDecodeMetadataCacheOnLoad,
            pDecoder.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        return pDecoder;
    }
    ComPtr<IWICBitmap> CreateBitmapFromHBITMAP(
        HBITMAP hBitmap)
    {
        HRESULT hr = nullptr == hBitmap ? E_FAIL : S_OK;
        ATLENSURE_SUCCEEDED(hr);

        ComPtr<IWICBitmap> pBitmap;
        hr = m_pWICImagingFactory2->CreateBitmapFromHBITMAP(
            hBitmap,
            nullptr,
            WICBitmapUseAlpha,
            pBitmap.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        DeleteObject(hBitmap);
        return pBitmap;
    }

    ComPtr<IWICBitmapDecoder> CreateDecoderFromFilename(
        LPCWSTR lpszFilename,
        WICDecodeOptions metadataOptions = WICDecodeMetadataCacheOnLoad)
    {
        ComPtr<IWICBitmapDecoder> pDecoder;

        HRESULT hr = m_pWICImagingFactory2->CreateDecoderFromFilename(
            lpszFilename,
            nullptr,
            GENERIC_READ,
            metadataOptions,
            pDecoder.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);
        return pDecoder;
    }
    ComPtr<IWICBitmapFrameDecode> GetFrame(IWICBitmapDecoder *pDecoder, UINT index = 0)
    {
        ComPtr<IWICBitmapFrameDecode> pFrame;
        HRESULT hr = pDecoder->GetFrame(index, pFrame.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        return pFrame;
    }

    ComPtr<IWICBitmapScaler> CreateBitmapScale(
        IWICBitmapSource *pISource,
        UINT uiWidth,
        UINT uiHeight,
        WICBitmapInterpolationMode mode = WICBitmapInterpolationModeNearestNeighbor)
    {
        assert(uiWidth != 0 && uiHeight != 0);

        ComPtr<IWICBitmapScaler> pScaler;

        HRESULT hr = m_pWICImagingFactory2->CreateBitmapScaler(pScaler.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        // If a new width or height was specified, create an
        // IWICBitmapScaler and use it to resize the image.
        UINT originalWidth, originalHeight;
        hr = pISource->GetSize(&originalWidth, &originalHeight);
        ATLENSURE_SUCCEEDED(hr);

        hr = pScaler->Initialize(
            pISource,
            uiWidth,
            uiHeight,
            mode);
        ATLENSURE_SUCCEEDED(hr);

        return pScaler;
    }
    ComPtr<IWICBitmapClipper> CreateBitmapClip(
        IWICBitmapSource *pISource,
        const WICRect &rect)
    {
        ComPtr<IWICBitmapClipper> pClipper;
        HRESULT hr = m_pWICImagingFactory2->CreateBitmapClipper(pClipper.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);
        hr = pClipper->Initialize(pISource, &rect);
        return pClipper;
    }

    ComPtr<IWICFormatConverter> CreateBitmapFormatConvert(
        IWICBitmapSource *pISource,
        REFWICPixelFormatGUID dstFormat = GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherType dither = WICBitmapDitherTypeNone,
        IWICPalette *pIPalette = nullptr,
        double alphaThresholdPercent = 0.f,
        WICBitmapPaletteType paletteTranslate = WICBitmapPaletteTypeCustom)
    {
        ComPtr<IWICFormatConverter> pConverter;

        HRESULT hr = m_pWICImagingFactory2->CreateFormatConverter(pConverter.GetAddressOf());
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
    ComPtr<IWICBitmapSource> CreateTransformBitmapSource(
        IWICBitmapFrameDecode *pFrame,
        D2D1::Matrix3x2F matrix,
        D2D1_RECT_F window_rect)
    {
        WICRect wicRect = Layout::CalcClipRect(matrix, pFrame, window_rect);

        ComPtr<IWICBitmapClipper> pClipper = CreateBitmapClip(pFrame, wicRect);

        UINT width = (UINT)(wicRect.Width * matrix._11);
        UINT height = (UINT)(wicRect.Height * matrix._22);

        ComPtr<IWICBitmapScaler> pScaler = CreateBitmapScale(
            pClipper.Get(),
            width,
            height,
            WICBitmapInterpolationMode::WICBitmapInterpolationModeNearestNeighbor);

        return CreateBitmapFormatConvert(pScaler.Get());
    }
    ComPtr<IWICBitmapSource> CreateBitmapFlipRotate(IWICBitmapSource *pSource, WICBitmapTransformOptions options)
    {
        ComPtr<IWICBitmapFlipRotator> pFlipRotator;
        HRESULT hr = m_pWICImagingFactory2->CreateBitmapFlipRotator(pFlipRotator.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        hr = pFlipRotator->Initialize(pSource, options);
        ATLENSURE_SUCCEEDED(hr);

        return pFlipRotator;
    }

    // Creates a Direct2D bitmap
    static ComPtr<IWICBitmapSource> LoadBitmapFromFile(
        LPCWSTR lpszFilename,
        UINT uiWidth = 0,
        UINT uiHeight = 0)
    {
        CWICImagingFactory2 factory;
        ComPtr<IWICBitmapDecoder> pDecoder = factory.CreateDecoderFromFilename(lpszFilename);
        ComPtr<IWICBitmapFrameDecode> pFrame;
        HRESULT hr = pDecoder->GetFrame(0, pFrame.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        ComPtr<IWICBitmapSource> pSource;
        if (uiWidth || uiHeight)
        {
            ComPtr<IWICBitmapSource> pScaler;
            pScaler = factory.CreateBitmapScale(pFrame.Get(), uiWidth, uiHeight);
            pSource = factory.CreateBitmapFormatConvert(pScaler.Get());
        }
        else
        {
            pSource = factory.CreateBitmapFormatConvert(pFrame.Get());
        }
        return pSource;
    }
public:
    IWICImagingFactory2 *operator->() { return m_pWICImagingFactory2.Get(); }
    IWICImagingFactory2 *Get() { return m_pWICImagingFactory2.Get(); }
private:
    static ComPtr<IWICImagingFactory2> CreateFactory()
    {
        ComPtr<IWICImagingFactory2> pFactory2;
        HRESULT hr = CoCreateInstance(
            CLSID_WICImagingFactory2,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(pFactory2.GetAddressOf()));
        ATLENSURE_SUCCEEDED(hr);

        return pFactory2;
    }

    ComPtr<IWICImagingFactory2> m_pWICImagingFactory2 = CreateFactory();
};

inline Concurrency::task<ComPtr<IWICBitmapSource>> LoadBitmapFromFile(
    std::wstring filename,
    UINT uiWidth = 0,
    UINT uiHeight = 0)
{
    return Concurrency::create_task([filename, uiWidth, uiHeight]{
        CCoInitializer coinit(COINIT_MULTITHREADED);
        return CWICImagingFactory2::LoadBitmapFromFile(filename.data(), uiWidth, uiHeight);
    });
}

