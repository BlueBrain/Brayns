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

#include <span>
#include <string>

#include <ospray/ospray_cpp.h>
#include <ospray/ospray_cpp/ext/rkcommon.h>

#include <brayns/core/utils/Math.h>

namespace brayns
{
template<typename HandleType>
class Object
{
public:
    using Handle = HandleType;

    explicit Object(Handle handle):
        _handle(std::move(handle))
    {
    }

    const Handle &getHandle() const
    {
        return _handle;
    }

    void commit()
    {
        _handle.commit();
    }

    explicit operator bool() const
    {
        return static_cast<bool>(_handle);
    }

protected:
    Box3 getBounds()
    {
        return _handle.template getBounds<Box3>();
    }

    void removeParam(const std::string &key)
    {
        _handle.removeParam(key);
    }

    template<typename U>
    void setParam(const std::string &key, const U &value)
    {
        _handle.setParam(key, value);
    }

private:
    Handle _handle;
};

template<typename T>
using SharedArray = std::span<T>;

template<typename T>
ospray::cpp::SharedData toSharedData(SharedArray<T> data)
{
    return ospray::cpp::SharedData(data.data(), data.size());
}

template<typename T>
using CopiedArray = std::span<T>;

template<typename T>
ospray::cpp::CopiedData toCopiedData(CopiedArray<T> data)
{
    return ospray::cpp::CopiedData(data.data(), data.size());
}

template<typename T>
std::vector<typename T::Handle> extractHandles(CopiedArray<T> objects)
{
    auto handles = std::vector<typename T::Handle>();
    handles.reserve(objects.size());
    for (const auto &object : objects)
    {
        auto &handle = object.getHandle();
        handles.push_back(handle);
    }
    return handles;
}
}
