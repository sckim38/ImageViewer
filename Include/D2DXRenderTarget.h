#pragma once

#include <cassert>

#include <Unknwn.h>
#include <wincodecsdk.h>

//#include "D2DXHelper.h"
#include "singleton.hpp"

#include "dx.h"
using namespace KennyKerr;
using namespace KennyKerr::Direct2D;
using Microsoft::WRL::ComPtr;

// Usefule macros
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

//using WICFactory = Factory<IWICImagingFactory2>;

//_AFX_D2D_STATE* pD2DState = AfxGetD2DState();
//
//ID2D1Factory* pDirect2dFactory = pD2DState->GetDirect2dFactory();
//IDWriteFactory* pDirectWriteFactory = pD2DState->GetWriteFactory();
//IWICImagingFactory* pImagingFactory = pD2DState->GetWICFactory();

namespace DWrite
{
    template<typename T>
    inline auto CreateFactory(DirectWrite::FactoryType type = DirectWrite::FactoryType::Shared) -> T
    {
        T result;

        HR(DWriteCreateFactory(static_cast<DWRITE_FACTORY_TYPE>(type),
            __uuidof(result.Get()),
            reinterpret_cast<IUnknown **>(result.GetAddressOf())));

        return result;
    }
}
namespace D2D1
{
    struct D2D1DeviceContext
    {
        Factory1 factory = Direct2D::CreateFactory();
        Direct3D::Device1 d3device = Direct3D::CreateDevice();
        Direct2D::Device d2device = factory.CreateDevice(d3device);
        Dxgi::Factory2 dxgi = d3device.GetDxgiFactory();
    };
    inline Factory1 GetFactory()
    {
        return unique_singleton<D2D1DeviceContext>::instance().factory;
    }
    inline Direct3D::Device1 GetD3Device()
    {
        return unique_singleton<D2D1DeviceContext>::instance().d3device;
    }
    inline Direct2D::Device GetD2Device()
    {
        return unique_singleton<D2D1DeviceContext>::instance().d2device;
    }
    inline Dxgi::Factory2 GetDxgiFactory()
    {
        return unique_singleton<D2D1DeviceContext>::instance().dxgi;
    }
}

class CD2D1RenderTarget
{
public:
protected:
    Factory1 m_factory = D2D1::GetFactory();
    HwndRenderTarget m_target;
    SolidColorBrush m_brush;

public:
    void CreateDeviceResources()
    {
        m_brush = m_target.CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::BlueViolet));
    }
    LRESULT Resize(D2D1_SIZE_U size)
    {
        if (m_target)
        {
            if (S_OK != m_target.Resize(size))
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
        }
        if ((WindowState::Occluded & m_target.CheckWindowState()) == WindowState::None)
        {
            m_target.BeginDraw();
            Draw();
            if (D2DERR_RECREATE_TARGET == m_target.EndDraw())
            {
                m_target.Reset();
            }
        }
    }
    virtual void Draw() = 0;
};

class CD2D1DeviceContext
{
    FLOAT m_dpi = 96.0F;
    D3D_FEATURE_LEVEL m_featureLevel = D3D_FEATURE_LEVEL_10_1;

public:
    Factory1 m_factory = D2D1::GetFactory();

    DeviceContext m_target;
    Dxgi::SwapChain1 m_swapChain;

    void ReleaseDevice()
    {
        m_target.Reset();
        m_swapChain.Reset();
    }

    virtual void Draw() = 0;

    void CreateDeviceSwapChainBitmap()
    {
        auto props = BitmapProperties1(BitmapOptions::Target | BitmapOptions::CannotDraw,
            PixelFormat(Dxgi::Format::B8G8R8A8_UNORM, AlphaMode::Ignore));

        m_target.SetTarget(m_target.CreateBitmapFromDxgiSurface(m_swapChain, props));
    }

    void Render(HWND window)
    {
        if (!m_target)
        {
            auto device = D2D1::GetD3Device();// Direct3D::CreateDevice();
            auto device1 = D2D1::GetD2Device();// m_factory.CreateDevice(device);
            m_target = device1.CreateDeviceContext();
            auto dxgi = D2D1::GetDxgiFactory();// device.GetDxgiFactory();

            Dxgi::SwapChainDescription1 description;
            description.SwapEffect = Dxgi::SwapEffect::Discard;

            m_swapChain = dxgi.CreateSwapChainForHwnd(device,
                window,
                description);

            CreateDeviceSwapChainBitmap();

            auto const dpi = m_factory.GetDesktopDpi();
            m_target.SetDpi(dpi, dpi);
        }

        m_target.BeginDraw();
        Draw();
        m_target.EndDraw();

        auto const hr = m_swapChain.Present();

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
        m_target.SetTarget();

        if (S_OK == m_swapChain.ResizeBuffers())
        {
            CreateDeviceSwapChainBitmap();
        }
        else
        {
            ReleaseDevice();
        }
    }
};


namespace D2D1
{
    inline ComPtr<ID2D1LinearGradientBrush> LinearGradientBrush(
        ID2D1RenderTarget *pRenderTarget,
        D2D1_COLOR_F BorderColor = D2D1::ColorF(D2D1::ColorF::Blue),
        D2D1_COLOR_F CenterColor = D2D1::ColorF(D2D1::ColorF::Indigo))
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
        HR(hr);

        auto size = pRenderTarget->GetSize();
        auto brushProperties =
            D2D1::LinearGradientBrushProperties(D2D1::Point2F(), D2D1::Point2F(size.width, size.height));

        ComPtr<ID2D1LinearGradientBrush> pLinearGradientBrush;
        hr = pRenderTarget->CreateLinearGradientBrush(
            brushProperties,
            pGradientStops.Get(),
            pLinearGradientBrush.GetAddressOf()
            );
        HR(hr);

        return pLinearGradientBrush;
    }
    inline ComPtr<ID2D1BitmapBrush> GridPatternBrush(
        ID2D1RenderTarget *pRenderTarget,
        D2D1_SIZE_F size = D2D1::SizeF(10.F, 10.F),
        FLOAT thickness = (1.F),
        D2D1_COLOR_F color = D2D1::ColorF(0.93f, 0.94f, 0.96f))
    {
        // Create a compatible render target.
        ComPtr<ID2D1BitmapRenderTarget> compatibleRenderTarget;
        HRESULT hr = pRenderTarget->CreateCompatibleRenderTarget(
            size,
            compatibleRenderTarget.GetAddressOf());
        HR(hr);

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
    class CompatibleRenderTarget
    {
    public:
        CompatibleRenderTarget(ID2D1RenderTarget *pRenderTarget)
            : m_pRenderTarget(pRenderTarget)
        {}
        HRESULT Create(D2D1_SIZE_F size)
        {
            assert(m_pRenderTarget);
            assert(size.width > 0.F && size.height > 0.F);
            return m_pRenderTarget->CreateCompatibleRenderTarget(size, m_pBitmapRenderTarget.GetAddressOf());
        }
        HRESULT Render(D2D1_COLOR_F color, const std::function<void(ID2D1RenderTarget *)> &render_func)
        {
            assert(m_pBitmapRenderTarget);
            m_pBitmapRenderTarget->BeginDraw();
            {
                m_pBitmapRenderTarget->Clear(color);
                render_func(m_pBitmapRenderTarget.Get());
            }
            return m_pBitmapRenderTarget->EndDraw();
        }
        HRESULT GetBitmap(ID2D1Bitmap **ppBitmap)
        {
            assert(m_pBitmapRenderTarget);
            return m_pBitmapRenderTarget->GetBitmap(ppBitmap);
        }

        ComPtr<ID2D1RenderTarget> m_pRenderTarget;
        ComPtr<ID2D1BitmapRenderTarget> m_pBitmapRenderTarget;
    };
    inline ComPtr<ID2D1Bitmap> MakeDrawBitmap(
        ID2D1RenderTarget *pRenderTarget,
        D2D1_COLOR_F color,
        D2D1_SIZE_F size,
        std::function<void(ID2D1RenderTarget *)> render_func)
    {
        // Create a compatible render target.
        CompatibleRenderTarget compatibleRenderTarget(pRenderTarget);

        HR(compatibleRenderTarget.Create(size));
        HR(compatibleRenderTarget.Render(color, render_func));

        // Create the bitmap brush.
        ComPtr<ID2D1Bitmap> pBitmap;
        // Retrieve the bitmap from the render target.
        compatibleRenderTarget.GetBitmap(pBitmap.GetAddressOf());

        return pBitmap;
    }
};


