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

#include "JsonError.h"

#include <utility>

namespace brayns
{
JsonPathElement::JsonPathElement(size_t index):
    _value(index)
{
}

JsonPathElement::JsonPathElement(std::string key):
    _value(std::move(key))
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

void JsonErrorBuilder::push(size_t index)
{
    _path.emplace_back(index);
}

void JsonErrorBuilder::push(std::string key)
{
    _path.emplace_back(std::move(key));
}

void JsonErrorBuilder::pop()
{
    _path.pop_back();
}

void JsonErrorBuilder::add(std::string message)
{
    _errors.push_back({std::move(message), _path});
}

std::vector<JsonError> JsonErrorBuilder::build()
{
    return std::exchange(_errors, {});
}

std::string JsonErrorFormatter::format(const JsonPath &path)
{
    auto result = std::string();
    for (const auto &element : path)
    {
        if (element.isIndex())
        {
            result.append(fmt::format("[{}]", element.asIndex()));
            continue;
        }
        if (!result.empty())
        {
            result.append(".");
        }
        result.append(element.asKey());
    }
    return result;
}

std::string JsonErrorFormatter::format(const JsonError &error)
{
    auto path = format(error.path);
    if (path.empty())
    {
        return error.message;
    }
    return fmt::format("{}: {}", path, error.message);
}

std::vector<std::string> JsonErrorFormatter::format(const JsonErrors &errors)
{
    auto messages = std::vector<std::string>();
    messages.reserve(errors.size());
    for (const auto &error : errors)
    {
        auto message = format(error);
        messages.push_back(message);
    }
    return messages;
}

JsonSchemaException::JsonSchemaException(const std::string &message, JsonErrors errors):
    std::runtime_error(message),
    _errors(std::move(errors))
{
}

const JsonErrors &JsonSchemaException::getErrors() const
{
    return _errors;
}
} // namespace brayns
