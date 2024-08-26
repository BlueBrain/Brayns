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

#include <any>
#include <concepts>
#include <functional>
#include <string>

#include <fmt/format.h>

#include <brayns/core/jsonrpc/Errors.h>

#include "Messages.h"
#include "ObjectReflector.h"

namespace brayns
{
template<ReflectedObject T>
struct UserObjectOf
{
    ObjectInfo info;
    T value;
};

template<ReflectedObject T>
class Stored
{
public:
    explicit Stored(std::shared_ptr<UserObjectOf<T>> object):
        _object(std::move(object))
    {
    }

    ObjectId getId() const
    {
        return _object->info.id;
    }

    bool isRemoved() const
    {
        return _object->info.id == nullId;
    }

    const std::string &getType() const
    {
        return _object->info.type;
    }

    JsonValue getUserData() const
    {
        return _object->info.userData;
    }

    void setUserData(const JsonValue &userData)
    {
        _object->info.userData = userData;
    }

    const ObjectInfo &getInfo() const
    {
        return _object->info;
    }

    ObjectResult getResult() const
    {
        return {getId()};
    }

    T &get() const
    {
        return _object->value;
    }

    T *operator->() const
    {
        return &_object->value;
    }

    T &operator*() const
    {
        return _object->value;
    }

private:
    std::shared_ptr<UserObjectOf<T>> _object;
};

struct ObjectInterface
{
    std::any value;
    std::function<ObjectInfo &()> getInfo;
    std::function<void()> remove;
};

template<ReflectedObject T>
ObjectInterface createObjectInterface(const std::shared_ptr<UserObjectOf<T>> &object)
{
    auto getInfo = [=]() -> auto &
    {
        return object->info;
    };

    auto remove = [=]
    {
        object->info.id = nullId;
        removeObject(object->value);
    };

    return {object, getInfo, remove};
}

template<typename T>
const std::shared_ptr<T> &castSharedObject(const std::any &value, const ObjectInfo &info)
{
    const auto *ptr = std::any_cast<std::shared_ptr<T>>(&value);

    if (ptr != nullptr)
    {
        return *ptr;
    }

    throw InvalidParams(fmt::format("Invalid type for object with ID {}: {}", info.id, info.type));
}

template<typename T>
T &castObject(const std::any &value, const ObjectInfo &info)
{
    return *castSharedObject<T>(value, info);
}
}
