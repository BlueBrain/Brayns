/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jonas Karlsson <jonas.karlsson@epfl.ch>
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

#include <stdexcept>
#include <string>
#include <vector>

namespace brayns
{
template<typename T>
using EnumMap = std::vector<std::pair<std::string, T>>;

/**
 * @brief Struct to specialize to use EnumInfo methods with T.
 *
 * @tparam T Enum type to reflect.
 */
template<typename T>
struct EnumReflector
{
    template<typename U>
    static constexpr auto alwaysFalse = false;

    static_assert(alwaysFalse<T>, "Enumeration is not reflected");

    /**
     * @brief Return a new EnumMap with values and names of T.
     *
     * @return EnumMap<T> Enum map of enum type.
     */
    static EnumMap<T> reflect()
    {
        return {};
    }
};

/**
 * @brief Shortcut to get enum info.
 *
 */
class EnumInfo
{
public:
    template<typename T>
    static const EnumMap<T> &getMapping()
    {
        return _mapping<T>;
    }

    template<typename T>
    static std::vector<std::string> getNames()
    {
        auto &mapping = getMapping<T>();
        std::vector<std::string> names;
        names.reserve(mapping.size());
        for (const auto &pair : mapping)
        {
            names.push_back(pair.first);
        }
        return names;
    }

    template<typename T>
    static std::vector<T> getValues()
    {
        auto &mapping = getMapping<T>();
        std::vector<T> values;
        values.reserve(values.size());
        for (const auto &pair : mapping)
        {
            values.push_back(pair.second);
        }
        return values;
    }

    template<typename T>
    static const std::string *findName(const T &value)
    {
        auto &mapping = getMapping<T>();
        for (const auto &[key, item] : mapping)
        {
            if (item == value)
            {
                return &key;
            }
        }
        return nullptr;
    }

    template<typename T>
    static const T *findValue(const std::string &name)
    {
        auto &mapping = getMapping<T>();
        for (const auto &[key, item] : mapping)
        {
            if (key == name)
            {
                return &item;
            }
        }
        return nullptr;
    }

    template<typename T>
    static const std::string &getName(const T &value)
    {
        auto name = findName(value);
        if (name)
        {
            return *name;
        }
        throw std::runtime_error("Invalid enum value");
    }

    template<typename T>
    static const T &getValue(const std::string &name)
    {
        auto value = findValue<T>(name);
        if (value)
        {
            return *value;
        }
        throw std::runtime_error("Invalid enum name '" + name + "'");
    }

private:
    template<typename T>
    static inline const EnumMap<T> _mapping = EnumReflector<T>::reflect();
};
} // namespace brayns
