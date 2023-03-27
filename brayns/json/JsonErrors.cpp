/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

#include "JsonErrors.h"

namespace brayns
{
JsonPathElement::JsonPathElement(size_t index)
    : _value(index)
{
}

JsonPathElement::JsonPathElement(std::string key)
    : _value(std::move(key))
{
}

bool JsonPathElement::isIndex() const
{
    return std::holds_alternative<size_t>(_value);
}

size_t JsonPathElement::asIndex() const
{
    return std::get<size_t>(_value);
}

bool JsonPathElement::isKey() const
{
    return std::holds_alternative<std::string>(_value);
}

const std::string &JsonPathElement::asKey() const
{
    return std::get<std::string>(_value);
}

bool JsonErrors::isEmpty() const
{
    return _errors.empty();
}

const std::vector<JsonError> &JsonErrors::asVector() const
{
    return _errors;
}

void JsonErrors::push(size_t index)
{
    _path.emplace_back(index);
}

void JsonErrors::push(std::string key)
{
    _path.emplace_back(std::move(key));
}

void JsonErrors::pop()
{
    _path.pop_back();
}

void JsonErrors::add(std::string message)
{
    _errors.push_back({std::move(message), _path});
}

std::string JsonErrorFormatter::format(const JsonPathElement &element)
{
    if (element.isIndex())
    {
        return fmt::format("[{}]", element.asIndex());
    }
    return fmt::format(".{}", element.asKey());
}

std::string JsonErrorFormatter::format(const JsonPath &path)
{
    auto result = std::string();
    for (const auto &element : path)
    {
        auto part = format(element);
        result.append(part);
    }
    return result;
}

std::string JsonErrorFormatter::format(const JsonError &error)
{
    auto path = format(error.path);
    if (!path.empty())
    {
        path = fmt::format(" for {}", path);
    }
    return fmt::format("Invalid schema{}: {}", path, error.message);
}

std::vector<std::string> JsonErrorFormatter::format(const JsonErrors &errors)
{
    auto result = std::vector<std::string>();
    auto &items = errors.asVector();
    result.reserve(items.size());
    for (const auto &item : items)
    {
        result.push_back(format(item));
    }
    return result;
}
} // namespace brayns
