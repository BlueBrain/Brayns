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

#include <concepts>
#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

#include <ospray/ospray_cpp.h>
#include <ospray/ospray_cpp/ext/rkcommon.h>

#include <brayns/core/utils/Math.h>

#include "Device.h"

namespace brayns
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
        decrementRefCount();
    }

    Object(const Object &other):
        _handle(other._handle)
    {
        incrementRefCount();
    }

    Object(Object &&other) noexcept:
        _handle(other.release())
    {
    }

    Object &operator=(const Object &other)
    {
        decrementRefCount();
        _handle = other._handle;
        incrementRefCount();
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
        incrementRefCount();
        return T(static_cast<typename T::Handle>(_handle));
    }

private:
    Handle _handle;

    void incrementRefCount() const
    {
        if (_handle)
        {
            ospRetain(_handle);
        }
    }

    void decrementRefCount() const
    {
        if (_handle)
        {
            ospRelease(_handle);
        }
    }
};

template<std::convertible_to<OSPObject> HandleType>
class Managed : public Object
{
public:
    using Handle = HandleType;

    explicit Managed(Handle handle = nullptr):
        Object(handle)
    {
    }

    Handle getHandle() const
    {
        return static_cast<Handle>(Object::getHandle());
    }
};

inline void checkObjectHandle(Device &device, OSPObject handle)
{
    device.throwIfError();

    if (handle == nullptr)
    {
        throw DeviceException(OSP_UNKNOWN_ERROR, "Null object returned without error");
    }
}

template<std::derived_from<Object> T>
T wrapObjectHandleAs(Device &device, typename T::Handle handle)
{
    checkObjectHandle(device, handle);
    return T(handle);
}

inline Box3 getObjectBounds(OSPObject handle)
{
    auto [lower, upper] = ospGetBounds(handle);
    return {{lower[0], lower[1], lower[2]}, {upper[0], upper[1], upper[2]}};
}

inline void commitObject(OSPObject handle)
{
    ospCommit(handle);
}

inline void commitObject(Device &device, OSPObject handle)
{
    ospCommit(handle);
    device.throwIfError();
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

template<typename T>
struct ObjectParamReflector;

template<typename T>
concept ObjectParam = std::is_void_v<decltype(ObjectParamReflector<T>::set(OSPObject(), "", std::declval<const T &>()))>;

template<ObjectParam T>
void setObjectParam(OSPObject handle, const char *id, const T &value)
{
    ObjectParamReflector<T>::set(handle, id, value);
}

template<OsprayDataType T>
struct ObjectParamReflector<T>
{
    static void set(OSPObject handle, const char *id, const T &value)
    {
        ospSetParam(handle, id, dataTypeOf<T>, &value);
    }
};

template<ObjectParam T>
struct ObjectParamReflector<std::optional<T>>
{
    static void set(OSPObject handle, const char *id, const std::optional<T> &value)
    {
        if (!value)
        {
            removeObjectParam(handle, id);
            return;
        }
        setObjectParam(handle, id, *value);
    }
};

template<ObjectParam... Ts>
struct ObjectParamReflector<std::variant<Ts...>>
{
    static void set(OSPObject handle, const char *id, const std::variant<Ts...> &values)
    {
        std::visit([=](const auto &value) { setObjectParam(handle, id, value); }, values);
    }
};

template<>
struct ObjectParamReflector<std::monostate>
{
    static void set(OSPObject handle, const char *id, std::monostate)
    {
        removeObjectParam(handle, id);
    }
};
}

namespace ospray
{
template<std::derived_from<brayns::Object> T>
struct OSPTypeFor<T>
{
    using Handle = typename T::Handle;

    static_assert(sizeof(T) == sizeof(Handle), "Object and handle must have the same size");

    static constexpr auto value = OSPTypeFor<Handle>::value;
};
}
