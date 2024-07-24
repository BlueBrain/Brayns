/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#pragma once

#include <cassert>
#include <ranges>
#include <span>

#include "Device.h"
#include "Object.h"

namespace brayns
{
template<OsprayDataType T>
class Data3D
{
public:
    explicit Data3D(OSPData handle, T *ptr, const Size3 &dimensions, const Size3 &stride = {0, 0, 0}):
        _managed(handle),
        _ptr(ptr),
        _dimensions(dimensions),
        _stride(stride)
    {
    }

    OSPData getHandle() const
    {
        return _managed.getHandle();
    }

    T *getPtr() const
    {
        return _ptr;
    }

    const Size3 &getDimensions() const
    {
        return _dimensions;
    }

    const Size3 &getStride() const
    {
        return _stride;
    }

    char *getBytes() const
    {
        return reinterpret_cast<char *>(_ptr);
    }

    std::span<T> getItems() const
    {
        return {_ptr, getItemCount()};
    }

    std::size_t getItemCount() const
    {
        return reduceMultiply(_dimensions);
    }

    std::size_t getWidth() const
    {
        return _dimensions[0];
    }

    std::size_t getHeight() const
    {
        return _dimensions[1];
    }

    std::size_t getDepth() const
    {
        return _dimensions[2];
    }

private:
    Managed<OSPData> _managed;
    T *_ptr;
    Size3 _dimensions;
    Size3 _stride;
};

template<OsprayDataType T>
class Data2D : public Data3D<T>
{
public:
    explicit Data2D(Data3D<T> data):
        Data3D<T>(std::move(data))
    {
        assert(Data3D<T>::getDepth() == 1);
    }
};

template<OsprayDataType T>
class Data : public Data2D<T>
{
public:
    explicit Data(Data3D<T> data):
        Data2D<T>(std::move(data))
    {
        assert(Data3D<T>::getHeight() == 1);
    }
};

template<OsprayDataType T>
struct ObjectParamReflector<Data3D<T>>
{
    static void set(OSPObject handle, const char *id, const Data3D<T> &value)
    {
        setObjectParam(handle, id, value.getHandle());
    }
};

template<OsprayDataType T>
struct ObjectParamReflector<Data2D<T>> : ObjectParamReflector<Data3D<T>>
{
};

template<OsprayDataType T>
struct ObjectParamReflector<Data<T>> : ObjectParamReflector<Data3D<T>>
{
};

template<OsprayDataType T>
Data3D<T> allocateData3D(Device &device, const Size3 &dimensions)
{
    auto [x, y, z] = dimensions;
    auto itemCount = reduceMultiply(dimensions);

    assert(itemCount > 0);

    auto type = dataTypeOf<T>;

    auto deleter = [](const void *userData, const void *sharedData)
    {
        (void)userData;
        auto *ptr = static_cast<T *>(const_cast<void *>(sharedData));
        delete[] ptr;
    };

    auto *ptr = new T[itemCount];

    try
    {
        auto *handle = ospNewSharedData(ptr, type, x, 0, y, 0, z, 0, deleter, nullptr);
        checkObjectHandle(device, handle);
        return Data3D<T>(handle, ptr, dimensions);
    }
    catch (...)
    {
        delete[] ptr;
        throw;
    }
}

template<OsprayDataType T>
Data2D<T> allocateData2D(Device &device, const Size2 &dimensions)
{
    auto data = allocateData3D<T>(device, Size3(dimensions, 1));
    return Data2D<T>(std::move(data));
}

template<OsprayDataType T>
Data<T> allocateData(Device &device, std::size_t itemCount)
{
    auto data = allocateData3D<T>(device, Size3(itemCount, 1, 1));
    return Data<T>(std::move(data));
}

template<OsprayDataType T, std::ranges::range U>
    requires(std::convertible_to<std::ranges::range_value_t<U>, T>)
Data<T> createData(Device &device, U &&items)
{
    auto data = allocateData<T>(device, items.size());
    auto destination = data.getItems();
    std::ranges::copy(items, destination.begin());
    return data;
}

template<OsprayDataType T, std::convertible_to<T> U>
Data<T> createData(Device &device, std::initializer_list<U> items)
{
    return createData<T>(device, std::span(items.begin(), items.size()));
}

struct DataRegion3D
{
    Size3 dimensions;
    Size3 stride = {0, 0, 0};
    std::size_t offset = 0;
};

struct DataRegion2D
{
    Size2 dimensions;
    Size2 stride = {0, 0};
    std::size_t offset = 0;
};

struct DataRegion
{
    std::size_t itemCount;
    std::size_t stride = 0;
    std::size_t offset = 0;
};

template<OsprayDataType OutputType, OsprayDataType T>
Data3D<OutputType> createData3DView(const Data3D<T> &data, const DataRegion3D &region)
{
    auto *bytes = data.getBytes() + region.offset;
    auto *ptr = reinterpret_cast<OutputType *>(bytes);

    auto type = dataTypeOf<OutputType>;

    auto [x, y, z] = region.dimensions;

    assert(x <= data.getWidth() && y <= data.getHeight() && z <= data.getDepth());

    auto [sx, sy, sz] = region.stride;

    auto deleter = [](const void *userData, const void *sharedData)
    {
        (void)sharedData;
        auto handle = static_cast<OSPData>(const_cast<void *>(userData));
        ospRelease(handle);
    };

    auto *userData = data.getHandle();

    auto *handle = ospNewSharedData(ptr, type, x, sx, y, sy, z, sz, deleter, userData);

    if (handle == nullptr)
    {
        throw DeviceException(OSP_UNKNOWN_ERROR, "Failed to create data view");
    }

    ospRetain(userData);

    return Data3D<OutputType>(handle, ptr, region.dimensions, region.stride);
}

template<OsprayDataType OutputType, OsprayDataType T>
Data2D<OutputType> createData2DView(const Data2D<T> &data, const DataRegion2D &region)
{
    auto region3D = DataRegion3D{Size3(region.dimensions, 1), Size3(region.stride, 0), region.offset};
    auto view = createData3DView<OutputType>(data, region3D);
    return Data2D<OutputType>(std::move(view));
}

template<OsprayDataType OutputType, OsprayDataType T>
Data<OutputType> createDataView(const Data<T> &data, const DataRegion &region)
{
    auto region3D = DataRegion3D{Size3(region.itemCount, 1, 1), Size3(region.stride, 0, 0), region.offset};
    auto view = createData3DView<OutputType>(data, region3D);
    return Data<OutputType>(std::move(view));
}
}
