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
std::string getSchema(const std::string& title)
{
    T obj;
    return getSchema(obj, title);
}

/** @return JSON schema from JSON-serializable object */
template <class T>
std::string getSchema(T& obj, const std::string& title)
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
    rapidjson::Document schema = staticjson::export_json_schema(&obj);

    using namespace rapidjson;
    schema.AddMember(StringRef("name"),
                     Value(paramName.c_str(), schema.GetAllocator()),
                     schema.GetAllocator());
    schema.AddMember(StringRef("description"),
                     Value(paramDescription.c_str(), schema.GetAllocator()),
                     schema.GetAllocator());
    return schema;
};

/** Documentation for RPC call with one parameter. */
struct RpcDocumentation
{
    std::string functionDescription;
    std::string paramName;
    std::string paramDescription;
};

/**
 * @return JSON schema for RPC with one parameter and a return value, according
 * to
 * http://www.simple-is-better.org/json-rpc/jsonrpc20-schema-service-descriptor.html
 */
template <class P, class R>
std::string buildJsonRpcSchema(const std::string& title,
                               const RpcDocumentation& doc, P& obj)
{
    using namespace rapidjson;
    Document schema(kObjectType);
    schema.AddMember(StringRef("title"), StringRef(title.c_str()),
                     schema.GetAllocator());
    schema.AddMember(StringRef("description"),
                     StringRef(doc.functionDescription.c_str()),
                     schema.GetAllocator());
    schema.AddMember(StringRef("type"), StringRef("method"),
                     schema.GetAllocator());

    R retVal;
    auto retSchema = staticjson::export_json_schema(&retVal);
    schema.AddMember(StringRef("returns"), retSchema, schema.GetAllocator());

    Value params(kArrayType);
    auto paramSchema =
        getRPCParameterSchema<P>(doc.paramName, doc.paramDescription, obj);
    params.PushBack(paramSchema, schema.GetAllocator());
    schema.AddMember(StringRef("params"), params, schema.GetAllocator());

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    schema.Accept(writer);
    return buffer.GetString();
}

template <class P, class R>
std::string buildJsonRpcSchema(const std::string& title,
                               const RpcDocumentation& doc)
{
    P obj;
    return buildJsonRpcSchema<P, R>(title, doc, obj);
}

/**
 * @return JSON schema for RPC with one parameter and no return value, according
 * to
 * http://www.simple-is-better.org/json-rpc/jsonrpc20-schema-service-descriptor.html
 */
template <class P>
std::string buildJsonRpcSchema(const std::string& title,
                               const RpcDocumentation& doc, P& obj)
{
    using namespace rapidjson;
    Document schema(kObjectType);
    schema.AddMember(StringRef("title"), StringRef(title.c_str()),
                     schema.GetAllocator());
    schema.AddMember(StringRef("description"),
                     StringRef(doc.functionDescription.c_str()),
                     schema.GetAllocator());
    schema.AddMember(StringRef("type"), StringRef("method"),
                     schema.GetAllocator());

    Value params(kArrayType);
    auto paramSchema =
        getRPCParameterSchema<P>(doc.paramName, doc.paramDescription, obj);
    params.PushBack(paramSchema, schema.GetAllocator());
    schema.AddMember(StringRef("params"), params, schema.GetAllocator());

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    schema.Accept(writer);
    return buffer.GetString();
}

template <class P>
std::string buildJsonRpcSchema(const std::string& title,
                               const RpcDocumentation& doc)
{
    P obj;
    return buildJsonRpcSchema<P>(title, doc, obj);
}

/** @return JSON schema for RPC with no parameter and no return value. */
std::string buildJsonRpcSchema(const std::string& title,
                               const std::string& description)
{
    using namespace rapidjson;
    Document schema(kObjectType);
    schema.AddMember(StringRef("title"), StringRef(title.c_str()),
                     schema.GetAllocator());
    schema.AddMember(StringRef("description"), StringRef(description.c_str()),
                     schema.GetAllocator());
    schema.AddMember(StringRef("type"), StringRef("method"),
                     schema.GetAllocator());

    schema.AddMember(StringRef("returns"), Value(kNullType),
                     schema.GetAllocator());

    Value params(kArrayType);
    schema.AddMember(StringRef("params"), params, schema.GetAllocator());

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    schema.Accept(writer);
    return buffer.GetString();
}
}
