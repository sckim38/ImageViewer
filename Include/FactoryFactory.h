#pragma once

#include <atlbase.h>
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

using Microsoft::WRL::ComPtr;

#pragma comment(lib, "d2d1")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "dwrite")

struct CCoInitialize
{
    CCoInitialize() try
    {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        ATLENSURE_SUCCEEDED(hr);
    }
    catch (...)
    {
    }
    ~CCoInitialize()
    {
        CoUninitialize();
    }
};

class DWriteFactory
{
public:
    IDWriteFactory *operator->() { return m_pDwriteFactory.Get(); }
    IDWriteFactory *Get() { return m_pDwriteFactory.Get(); }
private:
    static ComPtr<IDWriteFactory> CreateFactory(DWRITE_FACTORY_TYPE factoryType = DWRITE_FACTORY_TYPE_SHARED)
    {
        ComPtr<IDWriteFactory> pDwriteFactory;

        HRESULT hr = DWriteCreateFactory(factoryType,
            __uuidof(pDwriteFactory.Get()),
            reinterpret_cast<IUnknown **>(pDwriteFactory.GetAddressOf()));
        ATLENSURE_SUCCEEDED(hr);

        return pDwriteFactory;
    }
    ComPtr<IDWriteFactory> m_pDwriteFactory = CreateFactory();
};

class D2D1Device
{
public:
    ComPtr<ID2D1DeviceContext> CreateDeviceContext()
    {
        ComPtr<ID2D1DeviceContext> pD2D1DeviceContext;
        ATLENSURE_SUCCEEDED(m_pD2D1Device->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            pD2D1DeviceContext.GetAddressOf()));
        return pD2D1DeviceContext;
    }
    ID2D1Device *operator->() { return m_pD2D1Device.Get(); }
    ID2D1Device *Get() { return m_pD2D1Device.Get(); }

    ComPtr<ID2D1Device> m_pD2D1Device;
};

class D2D1Factory1
{
public:
    D2D1Device CreateDevice(const ComPtr<ID3D11Device> &pD3Device)
    {
        ComPtr<IDXGIDevice> pDXGIDevice;
        ATLENSURE_SUCCEEDED(pD3Device.As(&pDXGIDevice));

        ComPtr<ID2D1Device> pD2Device;
        ATLENSURE_SUCCEEDED(m_pD2D1Factory1->CreateDevice(pDXGIDevice.Get(), pD2Device.GetAddressOf()));

        return D2D1Device{ pD2Device };
    }
    ComPtr<ID2D1HwndRenderTarget> CreateHwndRenderTarget(HWND hWnd)
    {
        RECT rect;
        assert(::GetClientRect(hWnd, &rect));
        auto size = D2D1::SizeU(rect.right, rect.bottom);
        ComPtr<ID2D1HwndRenderTarget> pRenderTarget;
        ATLENSURE_SUCCEEDED(Get()->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(hWnd, size),
            pRenderTarget.GetAddressOf()));
        return pRenderTarget;
    }

    ID2D1Factory1 *operator->() { return m_pD2D1Factory1.Get(); }
    ID2D1Factory1 *Get() { return m_pD2D1Factory1.Get(); }
private:
    static ComPtr<ID2D1Factory1> CreateFactory()
    {
        ComPtr<ID2D1Factory1> pFactory1;
        D2D1_FACTORY_OPTIONS options = {};
#if defined(DEBUG) || defined(_DEBUG)
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#else
        options.debugLevel = D2D1_DEBUG_LEVEL_NONE;
#endif
        HRESULT hr = D2D1CreateFactory(
            D2D1_FACTORY_TYPE_MULTI_THREADED,
            options,
            pFactory1.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        return pFactory1;
    }
    ComPtr<ID2D1Factory1> m_pD2D1Factory1 = CreateFactory();
};

class WICImagingFactory2
{
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

class DXGIFactory2
{
public:
    ComPtr<IDXGISwapChain1> CreateSwapChainForHwnd(const ComPtr<ID3D11Device> &pD3Device, HWND hWnd)
    {
        DXGI_SWAP_CHAIN_DESC1 props = {};
        props.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        props.SampleDesc.Count = 1;
        props.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        props.BufferCount = 2;

        ComPtr<IDXGISwapChain1> pDXGISwapChain1;
        m_pDXGIFactory2->CreateSwapChainForHwnd(pD3Device.Get(),
            hWnd,
            &props,
            nullptr,
            nullptr,
            pDXGISwapChain1.GetAddressOf());
        return pDXGISwapChain1;
    }

    ComPtr<IDXGIFactory2> m_pDXGIFactory2;
};

class D3D11Device
{
public:
    DXGIFactory2 GetDXGIFactory2()
    {
        ComPtr<IDXGIDevice> pDXGIDevice;
        ATLENSURE_SUCCEEDED(m_pDevice.As(&pDXGIDevice));

        ComPtr<IDXGIAdapter> pDXGIAdapter;
        ATLENSURE_SUCCEEDED(pDXGIDevice->GetAdapter(pDXGIAdapter.GetAddressOf()));

        ComPtr<IDXGIFactory2> pDXGIFactory2;
        ATLENSURE_SUCCEEDED(pDXGIAdapter->GetParent(__uuidof(pDXGIFactory2),
            reinterpret_cast<void **>(pDXGIFactory2.GetAddressOf())));

        return DXGIFactory2{ pDXGIFactory2 };
    }
    ID3D11Device *operator->() { return m_pDevice.Get(); }
    ID3D11Device *Get() { return m_pDevice.Get(); }

private:
    static HRESULT CreateDevice(D3D_DRIVER_TYPE const type, ID3D11Device **pD3Device)
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
    static ComPtr<ID3D11Device> CreateDevice()
    {
        ComPtr<ID3D11Device> pDevice;
        HRESULT hr = CreateDevice(D3D_DRIVER_TYPE_HARDWARE, pDevice.GetAddressOf());
        if (DXGI_ERROR_UNSUPPORTED == hr)
            hr = CreateDevice(D3D_DRIVER_TYPE_WARP, pDevice.GetAddressOf());
        ATLENSURE_SUCCEEDED(hr);

        return pDevice;
    }
    ComPtr<ID3D11Device> m_pDevice = CreateDevice();
};


class D2D1RenderTarget
{
public:

public:
    virtual ~D2D1RenderTarget() {}
    virtual void Draw() = 0;

    virtual void CreateDeviceResources() {}
    virtual void ReleaseDeviceResources() {}

    LRESULT Resize(D2D1_SIZE_U size)
    {
        if (m_target)
        {
            if (S_OK != m_target->Resize(size))
            {
                m_target.Reset();
            }
        }
        return S_OK;
    }

    void Render(HWND hWnd)
    {
        if (!m_target)
        {
            m_target = m_factory.CreateHwndRenderTarget(hWnd);
            CreateDeviceResources();
        }
        if (!(D2D1_WINDOW_STATE_OCCLUDED & m_target->CheckWindowState()))
        {
            m_target->BeginDraw();
            Draw();
            if (D2DERR_RECREATE_TARGET == m_target->EndDraw())
            {
                m_target.Reset();
                ReleaseDeviceResources();
            }
        }
    }
protected:
    D2D1Factory1 m_factory;
    ComPtr<ID2D1HwndRenderTarget> m_target;
};

class D2D1DeviceContext
{
    FLOAT m_dpi = 96.0F;
    D3D_FEATURE_LEVEL m_featureLevel = D3D_FEATURE_LEVEL_10_1;

public:
    virtual ~D2D1DeviceContext() {}

    virtual void Draw() = 0;

    void CreateDevice(HWND hWnd)
    {
        m_swapChain = m_dxgifactory.CreateSwapChainForHwnd(m_d3d11device.Get(), hWnd);
        m_target = m_d2d1device.CreateDeviceContext();
    }

    void CreateDeviceSwapChainBitmap()
    {
        ComPtr<IDXGISurface> surface;
        ATLENSURE_SUCCEEDED(m_swapChain->GetBuffer(0, // buffer index
            __uuidof(surface),
            reinterpret_cast<void **>(surface.GetAddressOf())));

        auto props = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));

        ComPtr<ID2D1Bitmap1> pBitmap1;
        ATLENSURE_SUCCEEDED(m_target->CreateBitmapFromDxgiSurface(surface.Get(),
            props,
            pBitmap1.GetAddressOf()));
        m_target->SetTarget(pBitmap1.Get());
    }

    void Render(HWND hWnd)
    {
        if (!m_target)
        {
            CreateDevice(hWnd);
            CreateDeviceSwapChainBitmap();

            FLOAT dpiX, dpiY;
            m_factory->GetDesktopDpi(&dpiX, &dpiY);
            m_target->SetDpi(dpiX, dpiY);
        }

        m_target->BeginDraw();
        Draw();
        m_target->EndDraw();

        HRESULT hr = m_swapChain->Present(1, 0);

        if (S_OK != hr && DXGI_STATUS_OCCLUDED != hr)
        {
            ReleaseDevice();
        }
    }
    LRESULT Resize(D2D1_SIZE_U size)
    {
        if (m_target)
        {
            ResizeSwapChainBitmap();
        }
        return S_OK;
    }
    void ResizeSwapChainBitmap()
    {
        m_target->SetTarget(nullptr);
        if (S_OK == m_swapChain->ResizeBuffers(0,
            0, 0,
            DXGI_FORMAT_UNKNOWN,
            0))
        {
            CreateDeviceSwapChainBitmap();
        }
        else
        {
            ReleaseDevice();
        }
    }
    void ReleaseDevice()
    {
        m_target.Reset();
        m_swapChain.Reset();
    }

protected:
    D2D1Factory1 m_factory;
    D3D11Device m_d3d11device;
    D2D1Device m_d2d1device = m_factory.CreateDevice(m_d3d11device.Get());
    DXGIFactory2 m_dxgifactory = m_d3d11device.GetDXGIFactory2();

    ComPtr<ID2D1DeviceContext> m_target;
    ComPtr<IDXGISwapChain1> m_swapChain;

};
