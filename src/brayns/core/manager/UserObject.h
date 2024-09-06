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
#include <memory>
#include <string>

#include <brayns/core/jsonrpc/Errors.h>

#include "Messages.h"

namespace brayns
{
struct UserObject
{
    ObjectInfo info;
    std::any value;
};

template<typename T>
class Stored
{
public:
    explicit Stored(std::shared_ptr<UserObject> wrapper, std::shared_ptr<T> object):
        _wrapper(std::move(wrapper)),
        _object(std::move(object))
    {
    }

    const ObjectInfo &getInfo() const
    {
        return _wrapper->info;
    }

    ObjectId getId() const
    {
        return _wrapper->info.id;
    }

    bool isRemoved() const
    {
        return getId() == nullId;
    }

    T &get() const
    {
        return *_object;
    }

private:
    std::shared_ptr<UserObject> _wrapper;
    std::shared_ptr<T> _object;
};

template<typename T>
const std::shared_ptr<T> &castAsShared(const std::any &value, const ObjectInfo &info)
{
    const auto *ptr = std::any_cast<std::shared_ptr<T>>(&value);

    if (ptr != nullptr)
    {
        return *ptr;
    }

    throw InvalidParams(fmt::format("Invalid type for object with ID {}: {}", info.id, info.type));
}

template<typename T>
T &castAs(const std::any &value, const ObjectInfo &info)
{
    return *castAsShared<T>(value, info);
}
}
