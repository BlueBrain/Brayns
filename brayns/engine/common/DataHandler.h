#pragma once

#include <ospray/ospray.h>

#include <vector>

#include <iostream>

namespace brayns
{
struct OSPBuffer
{
    OSPBuffer() = default;

    OSPBuffer(OSPData handler, size_t size)
     : handle(handler)
     , size(size)
    {
    }

    ~OSPBuffer()
    {
        if(handle)
        {
            //std::cout << "DELETED" << std::endl;
            //ospRelease(handle);
        }
    }

    OSPBuffer(OSPBuffer &&) = default;
    OSPBuffer &operator=(OSPBuffer &&) = default;

    OSPBuffer(const OSPBuffer &) = delete;
    OSPBuffer &operator=(const OSPBuffer &) = delete;

    OSPData handle = nullptr;
    size_t size {};
};

struct DataHandler
{
    template<typename T>
    static OSPBuffer shareBuffer(const std::vector<T> &data, OSPDataType type)
    {
        OSPData sharedData = ospNewSharedData(data.data(), type, data.size());
        return OSPBuffer(sharedData, data.size());
    }

    template<typename T>
    static OSPBuffer shareBuffer(const T *data, const size_t size, OSPDataType type)
    {
        OSPData sharedData = ospNewSharedData(data, type, size);
        return OSPBuffer(sharedData, size);
    }

    template<typename T>
    static OSPBuffer copyBuffer(const std::vector<T> &data, OSPDataType type)
    {
        auto shared = shareBuffer(data, type);
        OSPData copy = ospNewData(type, data.size());
        ospCopyData(shared.handle, copy);
        return OSPBuffer(copy, data.size());
    }

    template<typename T>
    static OSPBuffer copyBuffer(const T *data, const size_t size, OSPDataType type)
    {
        auto shared = shareBuffer(data, size, type);
        OSPData copy = ospNewData(type, size);
        ospCopyData(shared.handle, copy);
        return OSPBuffer(copy, size);
    }
};
}
