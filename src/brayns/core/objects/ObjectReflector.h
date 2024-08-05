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
#include <string>
#include <type_traits>

#include <brayns/core/json/Json.h>

#include "Messages.h"

namespace brayns
{
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
concept ReflectedObject = WithSettings<T> && WithType<T> && WithProperties<T>;

template<ReflectedObject T>
struct ParamsOf
{
    ObjectId id;
    GetSettings<T> settings;
};

template<typename T>
concept WithSize = std::convertible_to<decltype(ObjectReflector<T>::getSize(std::declval<const T &>())), std::size_t>;

template<typename T>
concept WithRemove = std::same_as<decltype(ObjectReflector<T>::remove(std::declval<T &>())), void>;

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

void removeObject(auto &object)
{
    (void)object;
}

template<WithRemove T>
void removeObject(T &object)
{
    ObjectReflector<T>::remove(object);
}
}
