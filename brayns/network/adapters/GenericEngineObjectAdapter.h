/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/network/common/EngineObjectFactory.h>
#include <brayns/json/JsonAdapterMacro.h>

namespace brayns
{
/**
 * @brief Utility class to serialize/deserialize an EngineSerializableObject
 */
template<typename T>
class GenericEngineObjectAdapter
{
public:
    void setType(const std::string &type) noexcept
    {
        _type = type;
    }

    void setParams(const JsonValue &params) noexcept
    {
        _params = params;
    }

    const std::string& getType() const noexcept
    {
        return _type;
    }

    const JsonValue& getParams() const noexcept
    {
        return _params;
    }

    JsonValue serialize(const EngineObjectFactory<T> &factory, const T& object) noexcept
    {
        _type = std::string(object.getName());
        _params = factory.serialize(object);
    }

    std::unique_ptr<T> deserialize(const EngineObjectFactory<T> &factory) const noexcept
    {
        return factory.deserialize(_type, _params);
    }

private:
    std::string _type;
    JsonValue _params;
};
}
