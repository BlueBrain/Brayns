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

#pragma once

#include <stdexcept>

#include <spdlog/fmt/fmt.h>

#include <brayns/json/Json.h>

namespace brayns
{
/**
 * @brief Exception used to throw during the execution of a request inside an
 * entrypoint.
 *
 * Contains the necessary info to build an error message and send it
 * to the client.
 *
 */
class JsonRpcException : public std::runtime_error
{
public:
    /**
     * @brief Construct an exception with only a description (code = 0).
     *
     * @param message Error description.
     */
    explicit JsonRpcException(const std::string &message);

    /**
     * @brief Construct an exception with a code and a description.
     *
     * @param code Error code.
     * @param message Error description.
     * @param data Additional optional error data.
     */
    explicit JsonRpcException(int code, const std::string &message, const JsonValue &data = {});

    /**
     * @brief Get the error code of the exception.
     *
     * @return int Error code.
     */
    int getCode() const;

    /**
     * @brief Get additional info about the error.
     *
     * @return const JsonValue& Error data.
     */
    const JsonValue &getData() const;

private:
    int _code = 0;
    JsonValue _data;
};

/**
 * @brief The request cannot be parsed to a JsonRpcMessage.
 *
 */
class ParsingErrorException : public JsonRpcException
{
public:
    explicit ParsingErrorException(const std::string &message);
};

/**
 * @brief The request has invalid schema or header.
 *
 */
class InvalidRequestException : public JsonRpcException
{
public:
    explicit InvalidRequestException(const std::string &message);

    explicit InvalidRequestException(const std::string &message, const JsonErrors &errors);
};

/**
 * @brief Method specified doesn't exist.
 *
 */
class MethodNotFoundException : public JsonRpcException
{
public:
    explicit MethodNotFoundException(const std::string &method);
};

/**
 * @brief Invalid params schema or content.
 *
 */
class InvalidParamsException : public JsonRpcException
{
public:
    explicit InvalidParamsException(const std::string &message);

    explicit InvalidParamsException(const std::string &message, const JsonErrors &errors);
};

/**
 * @brief Valid request but internal error.
 *
 */
class InternalErrorException : public JsonRpcException
{
public:
    explicit InternalErrorException(const std::string &message);
};

/**
 * @brief Method has been cancelled.
 *
 */
class TaskCancelledException : public JsonRpcException
{
public:
    explicit TaskCancelledException();
};
} // namespace brayns

namespace fmt
{
template<>
struct formatter<brayns::JsonRpcException> : fmt::formatter<std::string>
{
    auto format(const brayns::JsonRpcException &e, fmt::format_context &context) const
    {
        return format_to(context.out(), "{{code = {}, message = {}}}", e.getCode(), e.what());
    }
};
} // namespace fmt
