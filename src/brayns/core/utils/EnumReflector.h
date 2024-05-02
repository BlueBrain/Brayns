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

#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/format.h>

namespace brayns::experimental
{
template<typename T>
using EnumInfo = std::vector<std::pair<std::string, T>>;

template<typename T>
struct EnumReflector
{
    template<typename U>
    static constexpr auto alwaysFalse = false;

    static_assert(alwaysFalse<T>, "Please specialize EnumReflector<T>");

    static EnumInfo<T> reflect()
    {
        return {};
    }
};

template<typename T>
const EnumInfo<T> &reflectEnum()
{
    static const EnumInfo<T> info = EnumReflector<T>::reflect();
    return info;
}

template<typename T>
std::vector<std::string> getEnumNames()
{
    const auto &info = reflectEnum<T>();
    std::vector<std::string> names;
    names.reserve(info.size());
    for (const auto &[name, value] : info)
    {
        names.push_back(name);
    }
    return names;
}

template<typename T>
static std::vector<T> getEnumValues()
{
    const auto &info = reflectEnum<T>();
    std::vector<T> values;
    values.reserve(values.size());
    for (const auto &[name, value] : info)
    {
        values.push_back(value);
    }
    return values;
}

template<typename T>
static const std::string *findEnumName(const T &value)
{
    const auto &info = reflectEnum<T>();
    for (const auto &[key, item] : info)
    {
        if (item == value)
        {
            return &key;
        }
    }
    return nullptr;
}

template<typename T>
static const T *findEnumValue(const std::string &name)
{
    const auto &info = reflectEnum<T>();
    for (const auto &[key, item] : info)
    {
        if (key == name)
        {
            return &item;
        }
    }
    return nullptr;
}

template<typename T>
const std::string &getEnumName(const T &value)
{
    const auto *name = findEnumName(value);
    if (name)
    {
        return *name;
    }
    throw std::invalid_argument(fmt::format("Invalid enum value: {}", int(value)));
}

template<typename T>
const T &getEnumValue(const std::string &name)
{
    const auto *value = findEnumValue<T>(name);
    if (value)
    {
        return *value;
    }
    throw std::invalid_argument(fmt::format("Invalid enum name '{}'", name));
}
}