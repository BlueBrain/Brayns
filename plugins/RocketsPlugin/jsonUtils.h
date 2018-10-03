/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
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

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include "staticjson/staticjson.hpp"
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include "rapidjson/document.h"
#include "rapidjson/writer.h"

namespace brayns
{
/** @return JSON schema from JSON-serializable type */
template <class T>
std::string buildJsonSchema(const std::string& title)
{
    T obj;
    return buildJsonSchema(obj, title);
}

/** @return JSON schema from JSON-serializable object */
template <class T>
std::string buildJsonSchema(T& obj, const std::string& title)
{
    using namespace rapidjson;
    auto schema = staticjson::export_json_schema(&obj);
    schema.AddMember(StringRef("title"), StringRef(title.c_str()),
                     schema.GetAllocator());

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    schema.Accept(writer);
    return buffer.GetString();
}

/** @return JSON schema for JSON RPC parameter */
template <class T>
rapidjson::Document getRPCParameterSchema(const std::string& paramName,
                                          const std::string& paramDescription,
                                          T& obj)
{
    using namespace rapidjson;
    auto schema = staticjson::export_json_schema(&obj);
    auto& allocator = schema.GetAllocator();

    schema.AddMember(StringRef("name"), Value(paramName.c_str(), allocator),
                     allocator);
    schema.AddMember(StringRef("description"),
                     Value(paramDescription.c_str(), allocator), allocator);
    return schema;
};

rapidjson::Document _buildJsonRpcSchema(const RpcDescription& desc)
{
    using namespace rapidjson;
    Document schema(kObjectType);
    auto& allocator = schema.GetAllocator();
    schema.AddMember(StringRef("title"), StringRef(desc.methodName.c_str()),
                     allocator);
    schema.AddMember(StringRef("description"),
                     StringRef(desc.methodDescription.c_str()), allocator);
    schema.AddMember(StringRef("type"), StringRef("method"), allocator);
    schema.AddMember(StringRef("async"), desc.type == RpcDescription::ASYNC,
                     allocator);
    return schema;
}

/**
 * @return JSON schema for RPC with one parameter and a return value, according
 * to
 * http://www.simple-is-better.org/json-rpc/jsonrpc20-schema-service-descriptor.html
 */
template <class P, class R>
std::string buildJsonRpcSchemaRequest(const RpcParameterDescription& desc,
                                      P& obj)
{
    using namespace rapidjson;
    auto schema = _buildJsonRpcSchema(desc);
    auto& allocator = schema.GetAllocator();

    R retVal;
    auto retSchema = staticjson::export_json_schema(&retVal);
    schema.AddMember(StringRef("returns"), retSchema, allocator);

    Value params(kArrayType);
    auto paramSchema =
        getRPCParameterSchema<P>(desc.paramName, desc.paramDescription, obj);
    params.PushBack(paramSchema, allocator);
    schema.AddMember(StringRef("params"), params, allocator);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    schema.Accept(writer);
    return buffer.GetString();
}

template <class P, class R>
std::string buildJsonRpcSchemaRequest(const RpcParameterDescription& desc)
{
    P obj;
    return buildJsonRpcSchemaRequest<P, R>(desc, obj);
}

/**
 * @return JSON schema for RPC with no parameter, but a return value, according
 * to
 * http://www.simple-is-better.org/json-rpc/jsonrpc20-schema-service-descriptor.html
 */
template <class R>
std::string buildJsonRpcSchemaRequestReturnOnly(const RpcDescription& desc,
                                                R& retVal)
{
    using namespace rapidjson;
    auto schema = _buildJsonRpcSchema(desc);
    auto& allocator = schema.GetAllocator();

    auto retSchema = staticjson::export_json_schema(&retVal);
    schema.AddMember(StringRef("returns"), retSchema, allocator);

    Value params(kArrayType);
    schema.AddMember(StringRef("params"), params, allocator);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    schema.Accept(writer);
    return buffer.GetString();
}

template <class R>
std::string buildJsonRpcSchemaRequestReturnOnly(const RpcDescription& desc)
{
    R retVal;
    return buildJsonRpcSchemaRequestReturnOnly<R>(desc, retVal);
}

/**
 * @return JSON schema for RPC with one parameter and no return value, according
 * to
 * http://www.simple-is-better.org/json-rpc/jsonrpc20-schema-service-descriptor.html
 */
template <class P>
std::string buildJsonRpcSchemaNotify(const RpcParameterDescription& desc,
                                     P& obj)
{
    using namespace rapidjson;
    auto schema = _buildJsonRpcSchema(desc);
    auto& allocator = schema.GetAllocator();

    Value params(kArrayType);
    auto paramSchema =
        getRPCParameterSchema<P>(desc.paramName, desc.paramDescription, obj);
    params.PushBack(paramSchema, allocator);
    schema.AddMember(StringRef("params"), params, allocator);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    schema.Accept(writer);
    return buffer.GetString();
}

template <class P>
std::string buildJsonRpcSchemaNotify(const RpcParameterDescription& desc)
{
    P obj;
    return buildJsonRpcSchemaNotify<P>(desc, obj);
}

/** @return JSON schema for RPC with no parameter and no return value. */
std::string buildJsonRpcSchemaNotify(const RpcDescription& desc)
{
    using namespace rapidjson;
    auto schema = _buildJsonRpcSchema(desc);
    auto& allocator = schema.GetAllocator();

    schema.AddMember(StringRef("returns"), Value(kNullType), allocator);

    Value params(kArrayType);
    schema.AddMember(StringRef("params"), params, allocator);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    schema.Accept(writer);
    return buffer.GetString();
}
}
