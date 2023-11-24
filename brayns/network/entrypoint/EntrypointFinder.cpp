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

#include "EntrypointFinder.h"

#include <brayns/json/Json.h>

#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace
{
class MethodFinder
{
public:
    static brayns::EntrypointRef &find(const std::string &method, brayns::EntrypointRegistry &entrypoints)
    {
        auto entrypoint = entrypoints.find(method);
        if (!entrypoint)
        {
            throw brayns::MethodNotFoundException(method);
        }
        return *entrypoint;
    }
};

class SchemaValidator
{
public:
    static void validate(const brayns::JsonValue &params, const std::optional<brayns::JsonSchema> &schema)
    {
        if (!schema)
        {
            _checkParamsIsEmpty(params);
            return;
        }
        _validate(params, *schema);
    }

private:
    static void _checkParamsIsEmpty(const brayns::JsonValue &params)
    {
        if (!params.isEmpty())
        {
            throw brayns::InvalidParamsException("This method takes no params");
        }
    }

    static void _validate(const brayns::JsonValue &params, const brayns::JsonSchema &schema)
    {
        auto errors = brayns::Json::validate(params, schema);
        if (!errors.empty())
        {
            throw brayns::InvalidParamsException("Invalid params schema", errors);
        }
    }
};
} // namespace

namespace brayns
{
EntrypointRef &EntrypointFinder::find(const JsonRpcRequest &request, EntrypointRegistry &entrypoints)
{
    auto &method = request.getMethod();
    auto &entrypoint = MethodFinder::find(method, entrypoints);
    auto &params = request.getParams();
    auto &schema = entrypoint.getParamsSchema();
    SchemaValidator::validate(params, schema);
    return entrypoint;
}
} // namespace brayns
