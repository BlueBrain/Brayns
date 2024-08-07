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
concept WithSize = std::same_as<std::size_t, decltype(ObjectReflector<T>::getSize(std::declval<const T &>()))>;

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
std::size_t getObjectSize(const T &object)
{
    (void)object;
    return 0;
}

template<WithSize T>
std::size_t getObjectSize(const T &object)
{
    return ObjectReflector<T>::getSize(object);
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

template<ReflectedObject T>
struct UserObject
{
    ObjectId id;
    T value;
    JsonValue userData = {};
};

template<ReflectedObject T>
void removeStoredObject(UserObject<T> &object)
{
    object.id = nullId;
    removeObject(object.value);
}

template<ReflectedObject T>
Metadata createObjectMetadata(const UserObject<T> &object)
{
    return {
        .id = object.id,
        .type = getObjectType(object.value),
        .size = getObjectSize(object.value),
        .userData = object.userData,
    };
}

template<ReflectedObject T>
class Stored
{
public:
    explicit Stored(std::shared_ptr<UserObject<T>> object):
        _object(std::move(object))
    {
    }

    ObjectId getId() const
    {
        return _object->id;
    }

    bool isRemoved() const
    {
        return _object->id == nullId;
    }

    std::string getType() const
    {
        return getObjectType(_object->value);
    }

    std::size_t getSize() const
    {
        return getObjectSize(_object->value);
    }

    JsonValue getUserData() const
    {
        return _object->userData;
    }

    void setUserData(const JsonValue &userData)
    {
        _object->userData = userData;
    }

    Metadata getMetadata() const
    {
        return createObjectMetadata(*_object);
    }

    T &get() const
    {
        return _object->value;
    }

private:
    std::shared_ptr<UserObject<T>> _object;
};

struct ObjectInterface
{
    std::any value;
    std::function<ObjectId()> getId;
    std::function<void()> remove;
    std::function<std::string()> getType;
    std::function<std::size_t()> getSize;
    std::function<JsonValue()> getUserData;
    std::function<void(const JsonValue &)> setUserData;
};

template<ReflectedObject T>
ObjectInterface createObjectInterface(const std::shared_ptr<UserObject<T>> &object)
{
    return {
        .value = object,
        .getId = [=] { return object->id; },
        .remove = [=] { removeStoredObject(*object); },
        .getType = [=] { return getObjectType(object->value); },
        .getSize = [=] { return getObjectSize(object->value); },
        .getUserData = [=] { return object->userData; },
        .setUserData = [=](const auto &userData) { object->userData = userData; },
    };
}

inline Metadata createObjectMetadata(const ObjectInterface &object)
{
    return {
        .id = object.getId(),
        .type = object.getType(),
        .size = object.getSize(),
        .userData = object.getUserData(),
    };
}
}
