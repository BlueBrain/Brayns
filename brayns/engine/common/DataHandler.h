#pragma once

#include <ospray/ospray.h>

#include <vector>

namespace brayns
{
struct Buffer
{
    OSPData handle = nullptr;

    ~Buffer()
    {
        ospRelease(handle);
    }
};

struct DataHandler
{
    template<typename T>
    static Buffer shareBuffer(const std::vector<T> &data, OSPDataType type)
    {
        OSPData sharedData = ospNewSharedData(data.data(), type, data.size());
        return {sharedData};
    }

    template<typename T>
    static Buffer copyBuffer(const std::vector<T> &data, OSPDataType type)
    {
        auto shared = shareBuffer(data, type);
        OSPData copy = ospNewData(type, data.size());
        ospCopyData(shared, copy);
        ospRelease(shared);
        return {copy};
    }
};
}
