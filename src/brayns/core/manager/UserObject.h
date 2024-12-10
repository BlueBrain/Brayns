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
#include <memory>

#include <fmt/format.h>

#include <brayns/core/jsonrpc/Errors.h>

#include "Messages.h"

namespace brayns
{
template<typename T>
std::shared_ptr<T> toShared(T value)
{
    return std::make_shared<T>(std::move(value));
}

template<typename T>
const std::shared_ptr<T> &castAsShared(const std::any &value, ObjectId id, const std::string &type)
{
    const auto *ptr = std::any_cast<std::shared_ptr<T>>(&value);

    if (ptr != nullptr)
    {
        return *ptr;
    }

    throw InvalidParams(fmt::format("Invalid type for object with ID {}: {}", id, type));
}

struct UserObject
{
    ObjectId id;
    ObjectMetadata metadata;
    ObjectSettings settings;
    std::any value;
};

template<typename T>
UserObject createObject(std::shared_ptr<T> value, ObjectMetadata metadata, ObjectSettings settings)
{
    return {
        .id = nullId,
        .metadata = std::move(metadata),
        .settings = std::move(settings),
        .value = std::move(value),
    };
}

inline ObjectSummary getSummary(const UserObject &object)
{
    return {object.id, object.metadata.type, object.settings.tag};
}

inline GetObjectResult getResult(const UserObject &object)
{
    return {object.metadata, object.settings};
}

template<typename T>
const std::shared_ptr<T> &castAsShared(const UserObject &object)
{
    return castAsShared<T>(object.value, object.id, object.metadata.type);
}

template<typename T>
class Stored
{
public:
    explicit Stored(std::shared_ptr<UserObject> object, std::shared_ptr<T> value):
        _object(std::move(object)),
        _value(std::move(value))
    {
    }

    ObjectId getId() const
    {
        return _object->id;
    }

    const std::string &getType() const
    {
        return _object->metadata.type;
    }

    T &get() const
    {
        return *_value;
    }

private:
    std::shared_ptr<UserObject> _object;
    std::shared_ptr<T> _value;
};

template<typename T, typename U>
const std::shared_ptr<T> &castAsShared(const std::any &value, const Stored<U> &object)
{
    return castAsShared<T>(value, object.getId(), object.getType());
}

template<typename T>
CreateObjectResult getResult(const Stored<T> &object)
{
    return {object.getId()};
}
}
