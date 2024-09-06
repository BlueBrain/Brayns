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
#include <map>
#include <memory>
#include <vector>

#include <brayns/core/utils/IdGenerator.h>

#include "Messages.h"
#include "UserObject.h"

namespace brayns
{
class ObjectRegistry
{
public:
    void remove(ObjectId id);
    void clear();
    ObjectInfo get(ObjectId id) const;
    std::vector<ObjectInfo> getAll() const;
    void update(ObjectId id, const JsonValue &userData);

    template<typename T>
    Stored<T> add(T object, std::string type)
    {
        auto objectPtr = std::make_shared<T>(std::move(object));
        auto info = ObjectInfo{std::move(type)};

        auto wrapper = UserObject{std::move(info), objectPtr};
        auto wrapperPtr = std::make_shared<decltype(wrapper)>(std::move(wrapper));

        store(wrapperPtr);

        return Stored(std::move(wrapperPtr), std::move(objectPtr));
    }

    template<typename T>
    Stored<T> getAsStored(ObjectId id)
    {
        const auto &wrapper = retreive(id);
        const auto &object = castAsShared<T>(wrapper->value, wrapper->info);
        return Stored<T>(wrapper, object);
    }

    template<typename T>
    T &getAs(ObjectId id)
    {
        const auto &wrapper = retreive(id);
        return *castAsShared<T>(wrapper->value, wrapper->info);
    }

private:
    std::map<ObjectId, std::shared_ptr<UserObject>> _objects;
    IdGenerator<ObjectId> _ids{1};

    void store(std::shared_ptr<UserObject> object);
    const std::shared_ptr<UserObject> &retreive(ObjectId id) const;
};
}
