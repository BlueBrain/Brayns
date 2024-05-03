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
#include <vector>

#include <fmt/format.h>

namespace brayns::experimental
{
template<typename T>
class EnumInfo
{
public:
    explicit EnumInfo(std::string name, std::vector<std::pair<std::string, T>> mapping):
        _name(std::move(name)),
        _mapping(std::move(mapping))
    {
    }

    const std::string &getName() const
    {
        return _name;
    }

    std::vector<std::string> getNames() const
    {
        auto names = std::vector<std::string>();
        names.reserve(_mapping.size());
        for (const auto &[name, value] : _mapping)
        {
            names.push_back(name);
        }
        return names;
    }

    std::vector<T> getValues() const
    {
        auto values = std::vector<T>();
        values.reserve(values.size());
        for (const auto &[name, value] : _mapping)
        {
            values.push_back(value);
        }
        return values;
    }

    const std::string *findName(const T &value) const
    {
        for (const auto &[key, item] : _mapping)
        {
            if (item == value)
            {
                return &key;
            }
        }
        return nullptr;
    }

    const T *findValue(const std::string &name) const
    {
        for (const auto &[key, item] : _mapping)
        {
            if (key == name)
            {
                return &item;
            }
        }
        return nullptr;
    }

    const std::string &getName(const T &value) const
    {
        const auto *name = findName(value);
        if (name)
        {
            return *name;
        }
        throw std::invalid_argument(fmt::format("Invalid enum value: {}", int(value)));
    }

    const T &getValue(const std::string &name) const
    {
        const auto *value = findValue(name);
        if (value)
        {
            return *value;
        }
        throw std::invalid_argument(fmt::format("Invalid enum name '{}'", name));
    }

private:
    std::string _name;
    std::vector<std::pair<std::string, T>> _mapping;
};

template<typename T>
struct EnumReflector
{
    template<typename U>
    static constexpr auto alwaysFalse = false;

    static_assert(alwaysFalse<T>, "Please specialize EnumReflector<T>");

    static EnumInfo<T> reflect()
    {
        throw std::runtime_error("Not implemented");
    }
};

template<typename T>
EnumInfo<T> reflectEnum()
{
    return EnumReflector<T>::reflect();
}
}
