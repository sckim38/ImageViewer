#pragma once

#include <Windows.h>
#include <algorithm>
#include <cassert>

#include <Unknwn.h>
#include <wincodecsdk.h>

#include <d3d11_1.h>
#include <d2d1_1helper.h>

#include <wrl.h>
#include <ppl.h>

using Microsoft::WRL::ComPtr;

#pragma comment(lib, "d2d1")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "dwrite")

// Usefule macros
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

namespace D2D1
{
    template<typename T>
    class Factory;

    template<typename T>
    class Singleton
    {
    public:
        static ComPtr<T> GetFactory()
        {
            if (!factory_)
            {
                concurrency::critical_section::scoped_lock lock(cs_);
                if (!factory_)
                {
                    HR(Factory<T>::Create(factory_));
                }
            }
            return factory_;
        }

    private:
        static concurrency::critical_section cs_;
        static ComPtr<T> factory_;
    };
    template<typename T> concurrency::critical_section Singleton<T>::cs_;
    template<typename T> ComPtr<T> Singleton<T>::factory_;

    template<>
    class Factory<ID2D1Factory1> : public Singleton<ID2D1Factory1>
    {

    public:
        static HRESULT Create(ComPtr<ID2D1Factory1> &pID2D1Factory)
        {
            D2D1_FACTORY_OPTIONS options = {};
#if defined(DEBUG) || defined(_DEBUG)
            options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
            return D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, options, pID2D1Factory.GetAddressOf());
        }
    };

    template<>
    class Factory<IWICImagingFactory2> : public Singleton<IWICImagingFactory2>
    {
    public:
        static HRESULT Create(ComPtr<IWICImagingFactory2> &pIWICImagingFactory)
        {
            return CoCreateInstance(
                CLSID_WICImagingFactory2,
                nullptr,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(pIWICImagingFactory.GetAddressOf()));
        }
    };

    template<>
    class Factory<IDWriteFactory> : public Singleton<IDWriteFactory>
    {
    public:
        static HRESULT Create(ComPtr<IDWriteFactory> &pIDWriteFactory)
        {
            return DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(pIDWriteFactory.Get()),
                reinterpret_cast<IUnknown**>(pIDWriteFactory.GetAddressOf()));
        }
    };
}