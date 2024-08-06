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
#include <functional>
#include <type_traits>

#include <brayns/core/json/Json.h>

#include "Messages.h"

namespace brayns
{
using ObjectId = std::uint32_t;

constexpr auto nullId = ObjectId(0);

template<typename T>
struct UserObject
{
    ObjectId id;
    T value;
    JsonValue userData = {};
};

template<typename T>
struct UserObjectReflector;

template<typename T>
struct UserObjectReflector<UserObject<T>>
{
    using Value = T;
};

template<typename T>
concept ValidUserObject = requires { typename UserObjectReflector<T>::Value; };

template<typename T>
using GetUserObjectValue = typename UserObjectReflector<T>::Value;

template<typename T>
struct ObjectReflector;

template<typename T>
using GetSettings = typename ObjectReflector<T>::Settings;

template<typename T>
concept WithSettings = ReflectedJson<GetSettings<T>>;

template<typename T>
concept WithType = std::convertible_to<decltype(ObjectReflector<T>::getType()), std::string>;

template<typename T>
using GetProperties = std::decay_t<decltype(ObjectReflector<T>::getProperties(std::declval<const T &>()))>;

template<typename T>
concept WithProperties = ReflectedJson<GetProperties<T>>;

template<typename T>
concept ReflectedObject = ValidUserObject<T> && WithSettings<T> && WithType<T> && WithProperties<T>;

template<typename T>
concept WithSize = std::convertible_to<decltype(ObjectReflector<T>::getSize(std::declval<const T &>())), std::size_t>;

template<ReflectedObject T>
const std::string &getObjectType()
{
    static const std::string type = ObjectReflector<T>::getType();
    return type;
}

template<ReflectedObject T>
GetProperties<T> getObjectProperties(const T &object)
{
    return ObjectReflector<T>::getProperties(object);
}

std::size_t getObjectSize(const auto &object)
{
    (void)object;
    return 0;
}

template<WithSize T>
std::size_t getObjectSize(const T &object)
{
    return ObjectReflector<T>::getSize(object);
}

template<ReflectedObject T>
using ObjectFactory = std::function<GetUserObjectValue<T>(ObjectId, GetSettings<T>)>;

template<ReflectedObject T>
Metadata createMetadata(const T &object)
{
    return {
        .id = object.id,
        .type = getObjectType<T>(),
        .size = getObjectSize(object),
        .userData = object.userData,
    };
}

template<ReflectedObject T>
ObjectResult<GetProperties<T>> createResult(const T &object)
{
    return {createMetadata(object), getObjectProperties(object)};
}
}
