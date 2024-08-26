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

#include "Messages.h"

namespace brayns
{
template<typename T>
struct ObjectReflector;

template<typename T>
concept WithType = std::same_as<std::string, decltype(ObjectReflector<T>::getType(std::declval<const T &>()))>;

template<typename T>
concept WithAdd = std::is_void_v<decltype(ObjectReflector<T>::add(std::declval<T &>(), ObjectId()))>;

template<typename T>
concept WithRemove = std::is_void_v<decltype(ObjectReflector<T>::remove(std::declval<T &>()))>;

template<typename T>
concept ReflectedObject = WithType<T> && std::default_initializable<T>;

template<ReflectedObject T>
std::string getObjectType(const T &object)
{
    return ObjectReflector<T>::getType(object);
}

template<typename T>
void addObject(T &object, ObjectId id)
{
    (void)object;
    (void)id;
}

template<WithAdd T>
void addObject(T &object, ObjectId id)
{
    return ObjectReflector<T>::add(object, id);
}

template<typename T>
void removeObject(T &object)
{
    (void)object;
}

template<WithRemove T>
void removeObject(T &object)
{
    return ObjectReflector<T>::remove(object);
}
}
