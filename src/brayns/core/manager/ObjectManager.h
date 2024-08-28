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

#include <map>
#include <string>
#include <vector>

#include <brayns/core/utils/IdGenerator.h>

#include "Messages.h"
#include "Object.h"

namespace brayns
{
class ObjectManager
{
public:
    std::vector<ObjectInfo> getAllObjects() const;
    ObjectInfo getObject(ObjectId id) const;
    void setUserData(ObjectId id, const JsonValue &userData);
    void remove(ObjectId id);
    void clear();

    template<typename T>
    T &get(ObjectId id) const
    {
        return getShared<T>(id)->value;
    }

    template<typename T>
    Stored<T> getStored(ObjectId id) const
    {
        return Stored<T>(getShared<T>(id));
    }

    template<typename T>
    Stored<T> add(T object, std::string type)
    {
        auto id = _ids.next();

        try
        {
            auto info = ObjectInfo{id, std::move(type)};
            auto user = ObjectStorage<T>{std::move(info), std::move(object)};
            auto ptr = std::make_shared<decltype(user)>(std::move(user));

            auto interface = createObjectInterface(ptr);

            _objects.emplace(id, std::move(interface));

            return Stored<T>(std::move(ptr));
        }
        catch (...)
        {
            _ids.recycle(id);
            throw;
        }
    }

private:
    std::map<ObjectId, ObjectInterface> _objects;
    IdGenerator<ObjectId> _ids{1};

    const ObjectInterface &getInterface(ObjectId id) const;

    template<typename T>
    const std::shared_ptr<ObjectStorage<T>> &getShared(ObjectId id) const
    {
        const auto &interface = getInterface(id);

        return castObjectAsShared<ObjectStorage<T>>(interface.value, interface.getInfo());
    }
};
}
