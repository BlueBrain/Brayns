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

#include <functional>
#include <typeindex>

#include <spdlog/fmt/fmt.h>

#include <brayns/core/json/JsonAdapter.h>

namespace brayns
{
/**
 * @brief Represent a JSON-RPC request ID.
 *
 * Can be null, integer or string. Being null means that no replies are needed.
 *
 */
class RequestId
{
public:
    RequestId() = default;
    explicit RequestId(int64_t value);
    explicit RequestId(std::string value);

    bool isEmpty() const;
    bool isInt() const;
    bool isString() const;
    int64_t toInt() const;
    const std::string &toString() const;
    std::string getDisplayText() const;
    size_t getHashCode() const;

    bool operator==(const RequestId &other) const;
    bool operator!=(const RequestId &other) const;

private:
    std::type_index _type = typeid(void);
    int64_t _int = 0;
    std::string _string;
};

/**
 * @brief JSON interface for RequestId.
 *
 */
template<>
struct JsonAdapter<RequestId>
{
    static JsonSchema getSchema();
    static void serialize(const RequestId &value, JsonValue &json);
    static void deserialize(const JsonValue &json, RequestId &value);
};
} // namespace brayns

namespace std
{
template<>
struct hash<brayns::RequestId>
{
    size_t operator()(const brayns::RequestId &id) const
    {
        return id.getHashCode();
    }
};
} // namespace std

namespace fmt
{
template<>
struct formatter<brayns::RequestId> : fmt::formatter<std::string>
{
    auto format(const brayns::RequestId &id, fmt::format_context &context) const
    {
        return fmt::formatter<std::string>::format(id.getDisplayText(), context);
    }
};
} // namespace fmt
