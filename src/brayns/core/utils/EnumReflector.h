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

#include <concepts>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <fmt/format.h>

namespace brayns
{
template<typename T>
struct EnumField
{
    std::string name;
    T value;
    std::string description = {};
};

template<typename T>
struct EnumInfo
{
    std::vector<EnumField<T>> fields;
};

template<typename T>
const EnumField<T> *findEnumByName(const EnumInfo<T> &info, std::string_view name)
{
    auto i = std::ranges::find_if(info.fields, [&](const auto &field) { return field.name == name; });
    return i == info.fields.end() ? nullptr : &*i;
}

template<typename T>
const EnumField<T> *findEnumByValue(const EnumInfo<T> &info, T value)
{
    auto i = std::ranges::find_if(info.fields, [&](const auto &field) { return field.value == value; });
    return i == info.fields.end() ? nullptr : &*i;
}

template<typename T>
struct EnumReflector;

template<typename T>
concept ReflectedEnum = requires {
    { EnumReflector<T>::reflect() } -> std::same_as<EnumInfo<T>>;
};

template<ReflectedEnum T>
const EnumInfo<T> &reflectEnum()
{
    static const auto info = EnumReflector<T>::reflect();
    return info;
}

template<ReflectedEnum T>
const std::vector<EnumField<T>> &getEnumFields()
{
    const auto &info = reflectEnum<T>();
    return info.fields;
}

template<ReflectedEnum T>
const std::string &getEnumName(T value)
{
    const auto &info = reflectEnum<T>();
    const auto *field = findEnumByValue(info, value);

    if (field != nullptr)
    {
        return field->name;
    }

    throw std::invalid_argument(fmt::format("Invalid enum value: {}", std::underlying_type_t<T>(value)));
}

template<ReflectedEnum T>
T getEnumValue(std::string_view name)
{
    const auto &info = reflectEnum<T>();
    const auto *field = findEnumByName(info, name);

    if (field != nullptr)
    {
        return field->value;
    }

    throw std::invalid_argument(fmt::format("Invalid enum name: '{}'", name));
}

template<typename T>
class EnumFieldBuilder
{
public:
    explicit EnumFieldBuilder(EnumField<T> &field):
        _field(&field)
    {
    }

    EnumFieldBuilder description(std::string description)
    {
        _field->description = std::move(description);
        return *this;
    }

private:
    EnumField<T> *_field;
};

template<typename T>
class EnumBuilder
{
public:
    EnumFieldBuilder<T> field(std::string name, T value)
    {
        _fields.push_back({std::move(name), value});
        return EnumFieldBuilder<T>(_fields.back());
    }

    EnumInfo<T> build()
    {
        return EnumInfo<T>(std::exchange(_fields, {}));
    }

private:
    std::vector<EnumField<T>> _fields;
};
}
