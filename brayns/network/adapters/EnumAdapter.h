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

template <typename T>
struct EnumReflector
{
    static EnumMap<T> reflect() { return enumMap<T>(); }
};

template <typename T>
struct EnumAdapter
{
    static const EnumMap<T>& getEnumMap()
    {
        static const EnumMap<T> values = EnumReflector<T>::reflect();
        return values;
    }

    static JsonSchema getSchema(const T&)
    {
        JsonSchema schema;
        schema.type = JsonType::String;
        auto& enums = getEnumMap();
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
        auto& enums = getEnumMap();
        auto name = enums.getName(value);
        if (!name)
        {
            return false;
        }
        json = *name;
        return true;
    }

    static bool deserialize(const JsonValue& json, T& value)
    {
        std::string name;
        if (!Json::deserialize(json, name))
        {
            return false;
        }
        auto& enums = getEnumMap();
        auto newValue = enums.getValue(name);
        if (!newValue)
        {
            return false;
        }
        value = *newValue;
        return true;
    }
};

struct GetEnumName
{
    template <typename T>
    static const std::string& of(const T& value)
    {
        auto& enumMap = EnumAdapter<T>::getEnumMap();
        auto name = enumMap.getName(value);
        if (!name)
        {
            throw std::runtime_error("Unknown enum value");
        }
        return *name;
    }
};

#define BRAYNS_ADAPTER_ENUM(TYPE, ...)                               \
    template <>                                                      \
    inline std::vector<std::pair<std::string, TYPE>> enumMap<TYPE>() \
    {                                                                \
        return {__VA_ARGS__};                                        \
    }                                                                \
                                                                     \
    template <>                                                      \
    struct JsonAdapter<TYPE> : EnumAdapter<TYPE>                     \
    {                                                                \
    };

BRAYNS_ADAPTER_ENUM(JsonType, {JsonTypeName::ofNull(), JsonType::Null},
                    {JsonTypeName::ofBoolean(), JsonType::Boolean},
                    {JsonTypeName::ofInteger(), JsonType::Integer},
                    {JsonTypeName::ofNumber(), JsonType::Number},
                    {JsonTypeName::ofString(), JsonType::String},
                    {JsonTypeName::ofArray(), JsonType::Array},
                    {JsonTypeName::ofObject(), JsonType::Object})
} // namespace brayns