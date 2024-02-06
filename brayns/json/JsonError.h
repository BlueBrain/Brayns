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
#include <variant>
#include <vector>

#include <spdlog/fmt/fmt.h>

namespace brayns
{
/**
 * @brief Used to identify a JSON element in a nested structure.
 *
 * Can either be an index if inside an array or a key if inside an object.
 *
 */
class JsonPathElement
{
public:
    explicit JsonPathElement(size_t index);
    explicit JsonPathElement(std::string key);

    bool isIndex() const;
    size_t asIndex() const;
    bool isKey() const;
    const std::string &asKey() const;

private:
    std::variant<size_t, std::string> _value;
};

/**
 * @brief Identifies a JSON element in a nested structure.
 *
 */
using JsonPath = std::vector<JsonPathElement>;

/**
 * @brief JSON error info.
 *
 */
struct JsonError
{
    std::string message;
    JsonPath path;
};

/**
 * @brief Result of JSON schema validation.
 *
 */
using JsonErrors = std::vector<JsonError>;

/**
 * @brief Stores the current JSON errors during a validation process.
 *
 */
class JsonErrorBuilder
{
public:
    void push(size_t index);
    void push(std::string key);
    void pop();
    void add(std::string message);
    std::vector<JsonError> build();

    template<typename... Args>
    void add(fmt::format_string<Args...> format, Args &&...args)
    {
        auto message = fmt::format(format, std::forward<Args>(args)...);
        add(std::move(message));
    }

private:
    JsonPath _path;
    std::vector<JsonError> _errors;
};

/**
 * @brief Helper class to stringify JSON errors in a human readable format.
 *
 */
class JsonErrorFormatter
{
public:
    static std::string format(const JsonPath &path);
    static std::string format(const JsonError &error);
    static std::vector<std::string> format(const JsonErrors &errors);
};

/**
 * @brief Promote JSON schema errors to exception.
 *
 */
class JsonSchemaException : public std::runtime_error
{
public:
    explicit JsonSchemaException(const std::string &message, JsonErrors errors);

    const JsonErrors &getErrors() const;

private:
    JsonErrors _errors;
};
} // namespace brayns
