/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <utility>
#include <vector>

#include <brayns/common/utils/enumUtils.h>

#include "ArrayAdapter.h"
#include "PrimitiveAdapter.h"

namespace brayns
{
template <typename T>
class EnumMap
{
public:
    EnumMap() = default;

    EnumMap(std::vector<std::pair<std::string, T>> values)
        : _values(std::move(values))
    {
    }

    size_t getSize() const { return _values.size(); }

    auto begin() const { return _values.begin(); }

    auto end() const { return _values.end(); }

    const T* getValue(const std::string& name) const
    {
        for (const auto& pair : _values)
        {
            if (pair.first == name)
            {
                return &pair.second;
            }
        }
        return nullptr;
    }

    const std::string* getName(const T& value) const
    {
        for (const auto& pair : _values)
        {
            if (pair.second == value)
            {
                return &pair.first;
            }
        }
        return nullptr;
    }

private:
    std::vector<std::pair<std::string, T>> _values;
};

struct GetEnumMap
{
    template <typename T>
    static const EnumMap<T>& of()
    {
        static const EnumMap<T> values = enumMap<T>();
        return values;
    }
};

template <typename T>
struct EnumAdapter
{
    static JsonSchema getSchema(const T&)
    {
        JsonSchema schema;
        schema.type = JsonType::String;
        auto& enums = GetEnumMap::of<T>();
        schema.enums.reserve(enums.getSize());
        for (const auto& pair : enums)
        {
            auto& name = pair.first;
            schema.enums.push_back(name);
        }
        return schema;
    }

    static bool serialize(const T& value, JsonValue& json)
    {
        auto& enums = GetEnumMap::of<T>();
        auto name = enums.getName(value);
        if (!name)
        {
            return false;
        }
        json = *name;
    }

    static bool deserialize(const JsonValue& json, T& value)
    {
        std::string name;
        if (!Json::deserialize(json, name))
        {
            return false;
        }
        auto& enums = GetEnumMap::of<T>();
        auto newValue = enums.getValue(name);
        if (!newValue)
        {
            return false;
        }
        return true;
    }
};

template<>
struct JsonAdapter<JsonType> : EnumAdapter<JsonType>
{
};
} // namespace brayns