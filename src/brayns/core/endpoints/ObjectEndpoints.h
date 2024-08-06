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
#include <mutex>
#include <type_traits>
#include <vector>

#include <brayns/core/api/ApiBuilder.h>
#include <brayns/core/objects/ObjectManager.h>
#include <brayns/core/utils/Logger.h>

namespace brayns
{
class LockedObjects
{
public:
    explicit LockedObjects(ObjectManager objects, Logger &logger);

    auto visit(std::invocable<ObjectManager &> auto &&callable) -> decltype(callable(std::declval<ObjectManager &>()))
    {
        _logger->info("Waiting for object manager lock");
        auto lock = std::lock_guard(_mutex);
        _logger->info("Object manager lock acquired");

        return callable(_objects);
    }

private:
    std::mutex _mutex;
    ObjectManager _objects;
    Logger *_logger;
};

struct ObjectIdParams
{
    ObjectId id;
};

template<>
struct JsonObjectReflector<ObjectIdParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ObjectIdParams>();
        builder.field("id", [](auto &object) { return &object.id; }).description("Objects ID");
        return builder.build();
    }
};

struct ObjectIds
{
    std::vector<ObjectId> ids;
};

template<>
struct JsonObjectReflector<ObjectIds>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ObjectIds>();
        builder.field("ids", [](auto &object) { return &object.ids; }).description("List of object IDs");
        return builder.build();
    }
};

template<ReflectedObject T>
ObjectResult<GetProperties<T>> createObject(LockedObjects &locked, ObjectParams<GetSettings<T>> params)
{
    return locked.visit(
        [&](ObjectManager &objects)
        {
            auto &object = objects.create<T>(std::move(params.settings), params.userData);
            return objects.getResult<T>(object.id);
        });
}

template<ReflectedObject T>
ObjectResult<GetProperties<T>> getObject(LockedObjects &locked, ObjectId id)
{
    return locked.visit([=](ObjectManager &objects) { return objects.getResult<T>(id); });
}

template<ReflectedObject T>
void addCreateAndGet(ApiBuilder &builder, LockedObjects &locked)
{
    const auto &type = getObjectType<T>();

    builder.endpoint("get-" + type, [&](ObjectIdParams params) { return getObject<T>(locked, params.id); })
        .description("Get all properties of " + type + " with given ID");

    builder
        .endpoint(
            "create-" + type,
            [&](ObjectParams<GetSettings<T>> params) { return createObject<T>(locked, std::move(params)); })
        .description("Create a new " + type + " and returns it");
}

void addDefaultObjects(ObjectManager &objects);
void addObjectEndpoints(ApiBuilder &builder, LockedObjects &objects);
}
