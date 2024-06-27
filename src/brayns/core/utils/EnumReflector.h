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

namespace brayns::experimental
{
template<typename T>
struct EnumField
{
    std::string name;
    T value;
    std::string description;
};

template<typename T>
class EnumInfo
{
public:
    explicit EnumInfo(std::vector<EnumField<T>> fields):
        _fields(std::move(fields))
    {
    }

    const std::vector<EnumField<T>> &getFields() const
    {
        return _fields;
    }

    const EnumField<T> *findFieldByName(std::string_view name) const
    {
        auto sameName = [&](const auto &field) { return field.name == name; };
        auto i = std::ranges::find_if(_fields, sameName);
        return i == _fields.end() ? nullptr : &*i;
    }

    const EnumField<T> *findFieldByValue(T value) const
    {
        auto sameValue = [&](const auto &field) { return field.value == value; };
        auto i = std::ranges::find_if(_fields, sameValue);
        return i == _fields.end() ? nullptr : &*i;
    }

    const EnumField<T> &getFieldByName(std::string_view name) const
    {
        const auto *field = findFieldByName(name);
        if (field)
        {
            return *field;
        }
        throw std::invalid_argument(fmt::format("Invalid enum name: '{}'", name));
    }

    const EnumField<T> &getFieldByValue(T value) const
    {
        const auto *field = findFieldByValue(value);
        if (field)
        {
            return *field;
        }
        throw std::invalid_argument(fmt::format("Invalid enum value: {}", std::underlying_type_t<T>(value)));
    }

private:
    std::vector<EnumField<T>> _fields;
};

template<typename T>
struct EnumReflector;

template<typename T>
concept ReflectedEnum = std::same_as<EnumInfo<T>, decltype(EnumReflector<T>::reflect())>;

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
    return info.getFields();
}

template<ReflectedEnum T>
const std::string &getEnumName(T value)
{
    const auto &info = reflectEnum<T>();
    const auto &field = info.getFieldByValue(value);
    return field.name;
}

template<ReflectedEnum T>
T getEnumValue(std::string_view name)
{
    const auto &info = reflectEnum<T>();
    const auto &field = info.getFieldByName(name);
    return field.value;
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
class EnumInfoBuilder
{
public:
    EnumFieldBuilder<T> field(std::string name, T value)
    {
        auto &emplaced = _fields.emplace_back();
        emplaced.name = std::move(name);
        emplaced.value = value;
        return EnumFieldBuilder<T>(emplaced);
    }

    EnumInfo<T> build()
    {
        return EnumInfo<T>(std::exchange(_fields, {}));
    }

private:
    std::vector<EnumField<T>> _fields;
};
}
