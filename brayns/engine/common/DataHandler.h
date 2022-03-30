#pragma once

#include <ospray/ospray.h>

#include <vector>

namespace brayns
{
struct OSPBuffer
{
    OSPData handle = nullptr;
    size_t size {};

    ~OSPBuffer()
    {
        if(handle)
        {
            ospRelease(handle);
        }
    }
};

struct DataHandler
{
    template<typename T>
    static OSPBuffer shareBuffer(const std::vector<T> &data, OSPDataType type)
    {
        OSPData sharedData = ospNewSharedData(data.data(), type, data.size());
        return {sharedData, data.size()};
    }

    template<typename T>
    static OSPBuffer shareBuffer(const T *data, const size_t size, OSPDataType type)
    {
        OSPData sharedData = ospNewSharedData(data, type, size);
        return {sharedData, size};
    }

    template<typename T>
    static OSPBuffer copyBuffer(const std::vector<T> &data, OSPDataType type)
    {
        auto shared = shareBuffer(data, type);
        OSPData copy = ospNewData(type, data.size());
        ospCopyData(shared, copy);
        ospRelease(shared);
        return {copy, data.size()};
    }

    template<typename T>
    static OSPBuffer copyBuffer(const T *data, const size_t size, OSPDataType type)
    {
        auto shared = shareBuffer(data, size, type);
        OSPData copy = ospNewData(type, size);
        ospCopyData(shared, copy);
        ospRelease(shared);
        return {copy, size};
    }
};
}
