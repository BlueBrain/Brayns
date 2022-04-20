/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <ospray/ospray.h>

#include <stdexcept>
#include <vector>

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
        if (handle)
        {
            ospRelease(handle);
        }
    }

    OSPBuffer(OSPBuffer &&other)
    {
        *this = std::move(other);
    }
    OSPBuffer &operator=(OSPBuffer &&other)
    {
        handle = other.handle;
        size = other.size;

        other.handle = nullptr;
        return *this;
    }

    OSPBuffer(const OSPBuffer &) = delete;
    OSPBuffer &operator=(const OSPBuffer &) = delete;

    OSPData handle = nullptr;
    size_t size{};
};

struct DataHandler
{
    template<typename T>
    static OSPBuffer shareBuffer(const std::vector<T> &data, OSPDataType type)
    {
        if (data.empty())
        {
            throw std::invalid_argument("Cannot send empty array to OSPRay");
        }

        OSPData sharedData = ospNewSharedData(data.data(), type, data.size());
        return OSPBuffer(sharedData, data.size());
    }

    template<typename T>
    static OSPBuffer shareBuffer(const T *data, const size_t size, OSPDataType type)
    {
        if (size == 0)
        {
            throw std::invalid_argument("Cannot send empty array to OSPRay");
        }

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
