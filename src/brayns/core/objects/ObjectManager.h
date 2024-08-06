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
#include <typeindex>
#include <vector>

#include <fmt/format.h>

#include <brayns/core/json/Json.h>
#include <brayns/core/jsonrpc/Errors.h>
#include <brayns/core/utils/IdGenerator.h>

#include "Messages.h"
#include "UserObject.h"

namespace brayns
{
struct ObjectInterface
{
    std::any object;
    std::function<ObjectId()> getId;
    std::function<void()> removeId;
    std::function<std::string()> getType;
    std::function<std::size_t()> getSize;
    std::function<JsonValue()> getUserData;
    std::function<void(const JsonValue &)> setUserData;
};

Metadata createMetadata(const ObjectInterface &object);

class ObjectManager
{
public:
    explicit ObjectManager();

    std::vector<Metadata> getAllMetadata() const;
    Metadata getMetadata(ObjectId id) const;
    const ObjectInterface &getInterface(ObjectId id) const;
    void setUserData(ObjectId id, const JsonValue &userData);
    void remove(ObjectId id);
    void clear();

    template<ReflectedObject T>
    void addFactory(ObjectFactory<T> factory)
    {
        const auto &type = typeid(T);

        if (_factories.contains(type))
        {
            throw std::invalid_argument("A factory is already registered for given type");
        }

        _factories[type] = factory;
    }

    template<ReflectedObject T>
    const std::shared_ptr<T> &getShared(ObjectId id) const
    {
        const auto &interface = getInterface(id);

        return castObject<T>(id, interface);
    }

    template<ReflectedObject T>
    T &get(ObjectId id) const
    {
        return *getShared<T>(id);
    }

    template<ReflectedObject T>
    GetProperties<T> getProperties(ObjectId id) const
    {
        auto &object = get<T>(id);

        return getObjectProperties(object);
    }

    template<ReflectedObject T>
    ObjectResult<GetProperties<T>> getResult(ObjectId id) const
    {
        auto &object = get<T>(id);

        return createResult(object);
    }

    template<ReflectedObject T>
    T &create(GetSettings<T> settings, const JsonValue &userData = {})
    {
        auto id = _ids.next();

        try
        {
            auto object = createObject<T>(id, userData, std::move(settings));
            auto ptr = std::make_shared<T>(std::move(object));

            auto interface = ObjectInterface{
                .object = ptr,
                .getId = [=] { return ptr->id; },
                .removeId = [=] { ptr->id = nullId; },
                .getType = [] { return getObjectType<T>(); },
                .getSize = [=] { return getObjectSize(*ptr); },
                .getUserData = [=] { return ptr->userData; },
                .setUserData = [=](const auto &value) { ptr->userData = value; },
            };

            _objects.emplace(id, std::move(interface));

            return *ptr;
        }
        catch (...)
        {
            _ids.recycle(id);
            throw;
        }
    }

private:
    std::map<std::type_index, std::any> _factories;
    std::map<ObjectId, ObjectInterface> _objects;
    IdGenerator<ObjectId> _ids;

    template<ReflectedObject T>
    static const std::shared_ptr<T> &castObject(ObjectId id, const ObjectInterface &interface)
    {
        auto ptr = std::any_cast<std::shared_ptr<T>>(&interface.object);

        if (ptr != nullptr)
        {
            return *ptr;
        }

        const auto &expected = getObjectType<T>();
        auto got = interface.getType();

        throw InvalidParams(fmt::format("Invalid type for object with ID {}: expected {}, got {}", id, expected, got));
    }

    template<ReflectedObject T>
    T createObject(ObjectId id, const JsonValue &userData, GetSettings<T> settings)
    {
        auto i = _factories.find(typeid(T));

        if (i == _factories.end())
        {
            throw std::invalid_argument("Unsupported object type");
        }

        const auto &factory = std::any_cast<const ObjectFactory<T> &>(i->second);

        auto value = factory(id, std::move(settings));

        return {id, std::move(value), userData};
    }
};
}
