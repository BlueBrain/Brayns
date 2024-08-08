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

#include <brayns/core/json/Json.h>

namespace brayns
{
class JsonRpcException : public std::runtime_error
{
public:
    explicit JsonRpcException(int code, const std::string &message, const JsonValue &data = {});

    int getCode() const;
    const JsonValue &getData() const;

private:
    int _code = 0;
    JsonValue _data;
};

class ParseError : public JsonRpcException
{
public:
    explicit ParseError(const std::string &message);
};

class InvalidRequest : public JsonRpcException
{
public:
    explicit InvalidRequest(const std::string &message);
    explicit InvalidRequest(const std::string &message, const std::vector<JsonSchemaError> &errors);
};

class MethodNotFound : public JsonRpcException
{
public:
    explicit MethodNotFound(const std::string &method);
};

class InvalidParams : public JsonRpcException
{
public:
    explicit InvalidParams(const std::string &message);
    explicit InvalidParams(const std::string &message, const std::vector<JsonSchemaError> &errors);
};

class InternalError : public JsonRpcException
{
public:
    explicit InternalError(const std::string &message);
    explicit InternalError(const std::exception &e);
};
}
