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
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <brayns/core/json/Json.h>
#include <brayns/core/jsonrpc/Errors.h>
#include <brayns/core/utils/IdGenerator.h>

namespace brayns
{
using ObjectId = std::uint32_t;

constexpr auto nullId = ObjectId(0);

struct Metadata
{
    ObjectId id;
    std::string type;
    std::string tag = {};
    JsonValue userData = {};
};

template<>
struct JsonObjectReflector<Metadata>
{
    static auto build()
    {
        auto builder = JsonBuilder<Metadata>();
        builder.field("id", [](auto &object) { return &object.id; })
            .description("Object ID, primary way to query this object");
        builder.field("type", [](auto &object) { return &object.type; })
            .description("Object type, use endpoint 'get-{type}' to query detailed information about the object");
        builder.field("tag", [](auto &object) { return &object.tag; })
            .description("Optional user defined tag, can also be used to query this object");
        builder.field("user_data", [](auto &object) { return &object.userData; })
            .description("Optional user data (only for user, not used by brayns)");
        return builder.build();
    }
};

template<ReflectedJson Properties>
struct UserObject
{
    Metadata metadata;
    Properties properties;
};

template<ReflectedJson Properties>
struct JsonObjectReflector<UserObject<Properties>>
{
    static auto build()
    {
        auto builder = JsonBuilder<UserObject<Properties>>();
        builder.field("metadata", [](auto &object) { return &object.metadata; })
            .description("Generic object properties (not specific to object type)");
        builder.field("properties", [](auto &object) { return &object.properties; })
            .description("Object properties (specific to object type)");
        return builder.build();
    }
};

template<typename T>
struct UserObjectReflector;

template<ReflectedJson Properties>
struct UserObjectReflector<UserObject<Properties>>
{
    using Type = Properties;
};

template<typename T>
concept ValidUserObject = requires { typename UserObjectReflector<T>::Type; };

template<ValidUserObject T>
using GetUserObjectProperties = typename UserObjectReflector<T>::Type;

struct ObjectManagerEntry
{
    std::any object;
    std::function<Metadata &()> getMetadata;
};

struct UserObjectSettings
{
    std::string type;
    std::string tag = {};
    JsonValue userData = {};
};

class ObjectManager
{
public:
    explicit ObjectManager();

    std::vector<ObjectId> getIds() const;
    const Metadata &getMetadata(ObjectId id) const;
    ObjectId getId(const std::string &tag) const;
    void remove(ObjectId id);

    template<ValidUserObject T>
    const std::shared_ptr<T> &getShared(ObjectId id) const
    {
        auto &entry = getEntry(id);
        auto &object = entry.object;

        if (object.type() != typeid(T))
        {
            throw invalidType(entry);
        }

        return std::any_cast<std::shared_ptr<T>>(object);
    }

    template<ValidUserObject T>
    T &get(ObjectId id) const
    {
        return *getShared<T>(id);
    }

    template<ValidUserObject T>
    T &create(UserObjectSettings settings, GetUserObjectProperties<T> properties)
    {
        auto id = _ids.next();

        try
        {
            auto &[type, tag, userData] = settings;
            auto metadata = Metadata{id, std::move(type), std::move(tag), userData};
            auto object = T{std::move(metadata), std::move(properties)};
            auto ptr = std::make_shared<T>(std::move(object));

            auto entry = ObjectManagerEntry{ptr, [=] { return ptr->metadata; }};

            addEntry(id, std::move(entry));

            return *ptr;
        }
        catch (...)
        {
            _ids.recycle(id);
            throw;
        }
    }

private:
    std::map<ObjectId, ObjectManagerEntry> _objects;
    std::unordered_map<std::string, ObjectId> _idsByTag;
    IdGenerator<ObjectId> _ids;

    static InvalidParams invalidType(const ObjectManagerEntry &entry);

    const ObjectManagerEntry &getEntry(ObjectId id) const;
    void addEntry(ObjectId id, ObjectManagerEntry entry);
};
}
