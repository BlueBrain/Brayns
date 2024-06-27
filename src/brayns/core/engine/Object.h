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
#include <string>

#include <ospray/ospray_cpp.h>
#include <ospray/ospray_cpp/ext/rkcommon.h>

#include <brayns/core/utils/Math.h>

namespace brayns::experimental
{
class Object
{
public:
    using Handle = OSPObject;

    explicit Object(Handle handle):
        _handle(handle)
    {
    }

    ~Object()
    {
        if (_handle)
        {
            ospRelease(_handle);
        }
    }

    Object(const Object &other):
        _handle(other._handle)
    {
        if (_handle)
        {
            ospRetain(_handle);
        }
    }

    Object(Object &&other) noexcept:
        _handle(other.release())
    {
    }

    Object &operator=(const Object &other)
    {
        if (_handle)
        {
            ospRelease(_handle);
        }

        _handle = other._handle;

        if (_handle)
        {
            ospRetain(_handle);
        }

        return *this;
    }

    Object &operator=(Object &&other) noexcept
    {
        std::swap(_handle, other._handle);
        return *this;
    }

    Handle getHandle() const
    {
        return _handle;
    }

    Handle release()
    {
        return std::exchange(_handle, nullptr);
    }

    template<std::derived_from<Object> T>
    T as() const
    {
        return T(static_cast<typename T::Handle>(_handle));
    }

private:
    Handle _handle;
};

template<std::convertible_to<OSPObject> HandleType>
class Managed : public Object
{
public:
    using Handle = HandleType;

    explicit Managed(Handle handle):
        Object(handle)
    {
    }

    Handle getHandle() const
    {
        return static_cast<Handle>(Object::getHandle());
    }
};

inline Box3 getObjectBounds(OSPObject handle)
{
    auto [lower, upper] = ospGetBounds(handle);
    return {{lower[0], lower[1], lower[2]}, {upper[0], upper[1], upper[2]}};
}

inline void commitObject(OSPObject handle)
{
    ospCommit(handle);
}

inline void removeObjectParam(OSPObject handle, const char *id)
{
    ospRemoveParam(handle, id);
}

using DataType = OSPDataType;

template<typename T>
constexpr auto dataTypeOf = ospray::OSPTypeFor<T>::value;

template<typename T>
concept OsprayDataType = (dataTypeOf<T> != OSP_UNKNOWN);

template<OsprayDataType T>
void setObjectParam(OSPObject handle, const char *id, const T &value)
{
    constexpr auto type = ospray::OSPTypeFor<T>::value;
    ospSetParam(handle, id, type, &value);
}

inline void throwLastDeviceErrorIfNull(OSPDevice device, OSPObject object)
{
    if (object != nullptr)
    {
        return;
    }

    const auto *message = ospDeviceGetLastErrorMsg(device);

    throw std::runtime_error(message);
}

template<typename T>
struct ObjectReflector;

template<typename T>
concept ReflectedObjectSettings = requires { typename ObjectReflector<T>::Settings; };

template<ReflectedObjectSettings T>
using SettingsOf = typename ObjectReflector<T>::Settings;

template<typename T>
concept ReflectorObjectHandle =
    requires(OSPDevice device, SettingsOf<T> settings) { T(ObjectReflector<T>::createHandle(device, settings)); };

template<typename T>
concept ReflectedObject = ReflectedObjectSettings<T> && ReflectorObjectHandle<T>;
}

namespace ospray
{
template<std::derived_from<brayns::experimental::Object> T>
struct OSPTypeFor<T>
{
    using Handle = typename T::Handle;

    static_assert(sizeof(T) == sizeof(Handle), "Object and handle must have the same size");

    static constexpr auto value = OSPTypeFor<Handle>::value;
};
}
