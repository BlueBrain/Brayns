/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/json/JsonType.h>

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
    JsonRpcException(const std::string &message);

    /**
     * @brief Construct an exception with a code and a description.
     *
     * @param code Error code.
     * @param message Error description.
     * @param data Additional optional error data.
     */
    JsonRpcException(int code, const std::string &message, const JsonValue &data = {});

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

class ParsingErrorException : public JsonRpcException
{
public:
    ParsingErrorException(const std::string &message);
};

class InvalidRequestException : public JsonRpcException
{
public:
    InvalidRequestException(const std::string &message, const std::vector<std::string> &errors = {});
};

class MethodNotFoundException : public JsonRpcException
{
public:
    MethodNotFoundException(const std::string &method);
};

class InvalidParamsException : public JsonRpcException
{
public:
    InvalidParamsException(const std::string &message, const std::vector<std::string> &errors = {});
};

class InternalErrorException : public JsonRpcException
{
public:
    InternalErrorException(const std::string &message);
};
} // namespace brayns
