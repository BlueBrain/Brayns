/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <utility>
#include <vector>

#include <brayns/utils/EnumUtils.h>

#include "ArrayAdapter.h"
#include "PrimitiveAdapter.h"

namespace brayns
{
/**
 * @brief Wrapper around a bimap from enum value to enum name.
 *
 * @tparam T Enum type
 */
template <typename T>
class EnumMap
{
public:
    EnumMap() = default;

    /**
     * @brief Construct an EnumMap using names and values
     *
     * @param values
     */
    EnumMap(std::vector<std::pair<std::string, T>> values)
        : _values(std::move(values))
    {
    }

    /**
     * @brief Get the number of enum values.
     *
     * @return size_t Number of available enums values
     */
    size_t getSize() const { return _values.size(); }

    /**
     * @brief Pair value-name iterator.
     *
     * @return auto Iterator.
     */
    auto begin() const { return _values.begin(); }

    /**
     * @brief Pair value-name iterator.
     *
     * @return auto Iterator.
     */
    auto end() const { return _values.end(); }

    /**
     * @brief Get the value corresponding to enum name.
     *
     * @param name Enum name.
     * @return const T* Enum value or null if invalid name.
     */
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

    /**
     * @brief Get the name corresponding to the enum value.
     *
     * @param value Enum value.
     * @return const std::string* Enum name or null if invalid value.
     */
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

/**
 * @brief Class used to get the EnumMap of an enum type.
 *
 * @tparam T Enum type to reflect.
 */
template <typename T>
struct EnumReflector
{
    /**
     * @brief Return a new EnumMap with values and names of T.
     *
     * @return EnumMap<T> Enum map of enum type.
     */
    static EnumMap<T> reflect() { return enumMap<T>(); }
};

/**
 * @brief Base JSON adapter for enumerations.
 *
 * @tparam T Enum type.
 */
template <typename T>
struct EnumAdapter
{
    /**
     * @brief Create and store a static instance of EnumMap<T>.
     *
     * @return const EnumMap<T>& Reference to the static EnumMap<T> instance.
     */
    static const EnumMap<T>& getEnumMap()
    {
        static const EnumMap<T> values = EnumReflector<T>::reflect();
        return values;
    }

    /**
     * @brief Create an enum JSON schema.
     *
     * An enum schema is a string with all names in EnumMap<T> as available
     * values.
     *
     * @return JsonSchema Schema of enum type.
     */
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

    /**
     * @brief Serialize an enum value using its name.
     *
     * @param value Value to serialize.
     * @param json Output JSON.
     * @return true Success.
     * @return false Failure.
     */
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

    /**
     * @brief Deserialize an enum value from its name.
     *
     * @param json Input JSON.
     * @param value Output value.
     * @return true Success.
     * @return false Failure.
     */
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

/**
 * @brief Shortcut to get the name of an enum value using EnumAdapter<T>.
 *
 */
struct GetEnumName
{
    /**
     * @brief Return the name of an enum value.
     *
     * @tparam T Enum type.
     * @param value Enum value.
     * @return const std::string& Reference on the static name of value.
     * @throw std::runtime_error No name found for value.
     */
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

/**
 * @brief Shortcut to reflect an enum with all its available name-value pairs.
 *
 * Reflected enum can be used in JSON (schema, serialization) and GetEnumName.
 *
 * @code {.cpp}
 * BRAYNS_JSON_ADAPTER_ENUM(MyEnum,
 *      {"Value1": MyEnum::Value1},
 *      {"Value2", MyEnum::Value2})
 * @endcode
 *
 */
#define BRAYNS_JSON_ADAPTER_ENUM(TYPE, ...)                          \
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

/**
 * @brief Reflect JsonType enum.
 *
 */
BRAYNS_JSON_ADAPTER_ENUM(JsonType, {"", JsonType::Unknown},
                         {JsonTypeName::ofNull(), JsonType::Null},
                         {JsonTypeName::ofBoolean(), JsonType::Boolean},
                         {JsonTypeName::ofInteger(), JsonType::Integer},
                         {JsonTypeName::ofNumber(), JsonType::Number},
                         {JsonTypeName::ofString(), JsonType::String},
                         {JsonTypeName::ofArray(), JsonType::Array},
                         {JsonTypeName::ofObject(), JsonType::Object})
} // namespace brayns
