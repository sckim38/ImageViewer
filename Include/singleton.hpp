#pragma once

#include <memory>
#include <ppl.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;

template<typename T>
struct CreateSharedPtr
{
    static std::shared_ptr<T> Create()
    {
        return std::make_shared<T>();
    }
};
template<typename T>
struct CreateUniquePtr
{
    static std::unique_ptr<T> Create()
    {
        return std::make_unique<T>();
    }
};
template<typename T>
struct CreateComPtr
{
    static ComPtr<T> Create()
    {
        return ComPtr<T>();
    }
};

template<typename T, template <typename> class CreatorPolicy>
struct singleton
{
protected:
    singleton() = delete;
public:
    static ComPtr<T> GetFactory()
    {
        if (!factory_)
        {
            concurrency::critical_section::scoped_lock lock(cs_);
            if (!factory_)
            {
                HRESULT hr = CreatorPolicy<T>::Create(factory_.GetAddressOf());
                ATLENSURE_SUCCEEDED(hr);
            }
        }
        return factory_;
    }

private:
    static concurrency::critical_section cs_;
    static ComPtr<T> factory_;
};
template<typename T, template <typename> class CreatorPolicy> concurrency::critical_section singleton<T, CreatorPolicy>::cs_;
template<typename T, template <typename> class CreatorPolicy> ComPtr<T> singleton<T, CreatorPolicy>::factory_;

template<typename T>
struct singleton<T, CreateUniquePtr>
{
protected:
    singleton() = default;
public:
    static T &instance()
    {
        if (!instance_)
        {
            concurrency::critical_section::scoped_lock lock(cs_);
            if (!instance_)
            {
                instance_ = CreateUniquePtr<T>::Create();
            }
        }
        return *instance_;
    }
private:
    static concurrency::critical_section cs_;
    static std::unique_ptr<T> instance_;
};
template<typename T> concurrency::critical_section singleton<T, CreateUniquePtr>::cs_;
template<typename T> std::unique_ptr<T> singleton<T, CreateUniquePtr>::instance_;

template<typename T>
using unique_singleton = singleton<T, CreateUniquePtr>;

template<typename T>
struct singleton<T, CreateSharedPtr>
{
protected:
    singleton() = delete;
public:
    static std::shared_ptr<T> instance()
    {
        if (pointer_.expired())
        {
            concurrency::critical_section::scoped_lock lock(cs_);
            if (pointer_.expired())
            {
                std::shared_ptr<T> pointer = CreateSharedPtr<T>::Create();
                pointer_ = pointer;
                return pointer_.lock();
            }
        }
        return pointer_.lock();
    }

private:
    static concurrency::critical_section cs_;
    static std::weak_ptr<T> pointer_;
};
template<typename T> concurrency::critical_section singleton<T, CreateSharedPtr>::cs_;
template<typename T> std::weak_ptr<T> singleton<T, CreateSharedPtr>::pointer_;

template<typename T>
using shared_singleton = singleton<T, CreateSharedPtr>;

