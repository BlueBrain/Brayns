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

#include <algorithm>
#include <concepts>

#include <ospray/ospray_cpp.h>
#include <ospray/ospray_cpp/ext/rkcommon.h>

#include <brayns/core/utils/Math.h>

namespace brayns::experimental
{
template<std::convertible_to<OSPObject> HandleType>
class Managed
{
public:
    using Handle = HandleType;

    explicit Managed(Handle handle):
        _handle(handle)
    {
    }

    ~Managed()
    {
        ospRelease(_handle);
    }

    Managed(const Managed &other):
        _handle(other._handle)
    {
        retain();
    }

    Managed(Managed &&other) noexcept:
        _handle(std::exchange(other._handle, nullptr))
    {
    }

    template<std::convertible_to<Handle> OtherHandleType>
    Managed(const Managed<OtherHandleType> &other):
        _handle(other._handle)
    {
        retain();
    }

    template<std::convertible_to<Handle> OtherHandleType>
    Managed(Managed<OtherHandleType> &&other) noexcept:
        _handle(std::exchange(other._handle, nullptr))
    {
    }

    Managed &operator=(const Managed &other)
    {
        releaseAndRetain(other._handle);
        return *this;
    }

    Managed &operator=(Managed &&other) noexcept
    {
        std::swap(_handle, other._handle);
        return *this;
    }

    template<std::convertible_to<Handle> OtherHandleType>
    Managed &operator=(const Managed<OtherHandleType> &other)
    {
        releaseAndRetain(other._handle);
        return *this;
    }

    template<std::convertible_to<Handle> OtherHandleType>
    Managed &operator=(Managed<OtherHandleType> &&other) noexcept
    {
        std::swap(_handle, other._handle);
        return *this;
    }

    Handle getHandle() const
    {
        return _handle;
    }

    void commit()
    {
        ospCommit(_handle);
    }

protected:
    Box3 getBounds() const
    {
        auto [lower, upper] = ospGetBounds(_handle);
        return {{lower[0], lower[1], lower[2]}, {upper[0], upper[1], upper[2]}};
    }

    void removeParam(const char *id)
    {
        ospRemoveParam(_handle, id);
    }

    template<typename T>
    void setParam(const char *id, const T &value)
    {
        constexpr auto type = ospray::OSPTypeFor<T>::value;
        ospSetParam(_handle, id, type, &value);
    }

private:
    Handle _handle;

    void retain()
    {
        if (_handle)
        {
            ospRetain(_handle);
        }
    }

    void releaseAndRetain(Handle handle)
    {
        ospRelease(_handle);
        _handle = handle;
        retain();
    }
};

using Object = Managed<OSPObject>;
}

namespace ospray
{
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::Object, OSP_OBJECT)
}
