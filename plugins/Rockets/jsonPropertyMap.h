/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jonas Karlsson <jonas.karlsson@epfl.ch>
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

#include "jsonSerialization.h"

#include <brayns/common/propertymap/PropertyMap.h>
#include <brayns/common/utils/stringUtils.h>
#include <brayns/common/utils/utils.h>

namespace
{
inline std::string matchEnum(const std::vector<std::string>& enums,
                             const int32_t value)
{
    const bool inRange =
        value >= 0 && static_cast<size_t>(value) < enums.size();
    return inRange ? enums[static_cast<size_t>(value)] : "InvalidEnum";
}
} // namespace

namespace brayns
{
namespace
{
std::string camelCaseToSnakeCase(const std::string& camelCase)
{
    return brayns::string_utils::camelCaseToSeparated(camelCase, '_');
}

std::string snakeCaseToCamelCase(const std::string& snakeCase)
{
    return brayns::string_utils::separatedToCamelCase(snakeCase, '_');
}

// Make a (movable) rapidjson string
rapidjson::Value make_value_string(
    const std::string& str, rapidjson::Document::AllocatorType& allocator)
{
    rapidjson::Value val;
    val.SetString(str.c_str(), str.length(), allocator);
    return val;
}

// Make a (movable) rapidjson snake_case string from the given camelCase string.
rapidjson::Value make_json_string(const std::string& camelCase,
                                  rapidjson::Document::AllocatorType& allocator)
{
    return make_value_string(camelCaseToSnakeCase(camelCase), allocator);
}

template <typename T>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const T& value)
{
    document.AddMember(rapidjson::StringRef("default"), value, allocator);
}

template <>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::string& value)
{
    document.AddMember(rapidjson::StringRef("default"),
                       make_value_string(value, allocator).Move(), allocator);
}

template <typename T, int S>
void addDefaultValueGlm(rapidjson::Document& document,
                          rapidjson::Document::AllocatorType& allocator,
                          const glm::vec<S, T>& value)
{
    rapidjson::Value arr(rapidjson::kArrayType);
    for (int i = 0; i < S; ++i)
    {
        arr.PushBack(value[i], allocator);
    }
    document.AddMember(rapidjson::StringRef("default"), arr, allocator);
}

template <typename T, int S>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const glm::vec<S, T>& value)
{
    addDefaultValueGlm(document, allocator, value);
}

template <>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::vector<double>& value)
{
    rapidjson::Value arr(rapidjson::kArrayType);
    for (const auto v : value)
        arr.PushBack(v, allocator);
    document.AddMember(rapidjson::StringRef("default"), arr, allocator);
}

template <>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::vector<float>& value)
{
    rapidjson::Value arr(rapidjson::kArrayType);
    for (const auto v : value)
        arr.PushBack(v, allocator);
    document.AddMember(rapidjson::StringRef("default"), arr, allocator);
}

template <>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::vector<int32_t>& value)
{
    rapidjson::Value arr(rapidjson::kArrayType);
    for (const auto v : value)
        arr.PushBack(v, allocator);
    document.AddMember(rapidjson::StringRef("default"), arr, allocator);
}

template <>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::vector<uint32_t>& value)
{
    rapidjson::Value arr(rapidjson::kArrayType);
    for (const auto v : value)
        arr.PushBack(v, allocator);
    document.AddMember(rapidjson::StringRef("default"), arr, allocator);
}

template <>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::vector<uint64_t>& value)
{
    rapidjson::Value arr(rapidjson::kArrayType);
    for (const auto v : value)
        arr.PushBack(v, allocator);
    document.AddMember(rapidjson::StringRef("default"), arr, allocator);
}

template <>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::vector<char>& value)
{
    rapidjson::Value arr(rapidjson::kArrayType);
    for (const auto v : value)
        arr.PushBack(v, allocator);
    document.AddMember(rapidjson::StringRef("default"), arr, allocator);
}

template <>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::vector<std::string>& value)
{
    rapidjson::Value arr(rapidjson::kArrayType);
    for (const auto& v : value)
        arr.PushBack(make_value_string(v, allocator).Move(), allocator);
    document.AddMember(rapidjson::StringRef("default"), arr, allocator);
}

// Create JSON schema for given property and add it to the given properties
// parent.
template <typename T>
void _addPropertySchema(const Property& prop, rapidjson::Value& properties,
                        rapidjson::Document::AllocatorType& allocator)
{
    using namespace rapidjson;

    Document jsonSchema;
    if (!prop.is<EnumProperty>())
    {
        auto value = prop.as<T>();
        jsonSchema = staticjson::export_json_schema(&value, &allocator);
        jsonSchema.AddMember(StringRef("title"),
                             StringRef(prop.getLabel().c_str()), allocator);
        addDefaultValue(jsonSchema, allocator, value);
        jsonSchema.AddMember(StringRef("readOnly"), prop.isReadOnly(),
                             allocator);

        // Removed min and max from properties
        if (jsonSchema.HasMember("minimum"))
            jsonSchema.RemoveMember("minimum");
        if (jsonSchema.HasMember("maximum"))
            jsonSchema.RemoveMember("maximum");
    }
    else
    {
        jsonSchema.SetObject();
        jsonSchema.AddMember(StringRef("type"), StringRef("string"), allocator);
        jsonSchema.AddMember(StringRef("title"),
                             StringRef(prop.getLabel().c_str()), allocator);
        jsonSchema.AddMember(StringRef("readOnly"), prop.isReadOnly(),
                             allocator);

        // Specialized enum code
        auto& enumValue = prop.as<EnumProperty>();
        addDefaultValue(jsonSchema, allocator, enumValue.toString());

        Value enumerations(kArrayType);
        for (const auto& name : enumValue.getValues())
            enumerations.PushBack(StringRef(name.data(), name.size()),
                                  allocator);
        jsonSchema.AddMember(StringRef("enum"), enumerations, allocator);
    }
    // Add description, if any
    if (!prop.getDescription().empty() &&
        prop.getDescription() != "no-description")
        jsonSchema.AddMember(StringRef("description"),
                             StringRef(prop.getDescription().c_str()),
                             allocator);

    properties.AddMember(make_json_string(prop.getName(), allocator).Move(),
                         jsonSchema, allocator);
}

// Create JSON schema for given bool property and add it to the given properties
// parent.
template <>
void _addPropertySchema<bool>(const Property& prop,
                              rapidjson::Value& properties,
                              rapidjson::Document::AllocatorType& allocator)
{
    using namespace rapidjson;
    auto value = prop.as<bool>();
    auto jsonSchema = staticjson::export_json_schema(&value, &allocator);
    jsonSchema.AddMember(StringRef("title"), StringRef(prop.getLabel().c_str()),
                         allocator);
    if (!prop.getDescription().empty() &&
        prop.getDescription() != "no-description")
        jsonSchema.AddMember(StringRef("description"),
                             StringRef(prop.getDescription().c_str()),
                             allocator);
    addDefaultValue(jsonSchema, allocator, value);
    jsonSchema.AddMember(StringRef("readOnly"), prop.isReadOnly(), allocator);
    properties.AddMember(make_json_string(prop.getName(), allocator).Move(),
                         jsonSchema, allocator);
}

// Create JSON schema for string property and add it to the given properties
// parent.
template <>
void _addPropertySchema<std::string>(
    const Property& prop, rapidjson::Value& properties,
    rapidjson::Document::AllocatorType& allocator)
{
    using namespace rapidjson;

    const bool isEnum = prop.is<EnumProperty>();
    auto value = prop.to<std::string>();

    Document jsonSchema;

    if (isEnum)
    {
        jsonSchema.SetObject();
        jsonSchema.AddMember(StringRef("type"), StringRef("string"), allocator);
        Value enumerations(kArrayType);
        for (const auto& name : prop.as<EnumProperty>().getValues())
            enumerations.PushBack(StringRef(name.data(), name.size()),
                                  allocator);
        jsonSchema.AddMember(StringRef("enum"), enumerations, allocator);
    }
    else
    {
        jsonSchema = staticjson::export_json_schema(&value, &allocator);
    }

    addDefaultValue(jsonSchema, allocator, value);
    jsonSchema.AddMember(StringRef("readOnly"), prop.isReadOnly(), allocator);
    jsonSchema.AddMember(StringRef("title"), StringRef(prop.getLabel().c_str()),
                         allocator);
    if (!prop.getDescription().empty() &&
        prop.getDescription() != "no-description")
        jsonSchema.AddMember(StringRef("description"),
                             StringRef(prop.getDescription().c_str()),
                             allocator);
    properties.AddMember(make_json_string(prop.getName(), allocator).Move(),
                         jsonSchema, allocator);
}

// Create JSON schema for given array property and add it to the given
// properties parent.
template <typename T, int S>
void _addGlmPropertySchema(const Property& prop, rapidjson::Value& properties,
                             rapidjson::Document::AllocatorType& allocator)
{
    using namespace rapidjson;
    auto value = prop.as<glm::vec<S, T>>();
    auto jsonSchema = staticjson::export_json_schema(toArray(value), &allocator);
    jsonSchema.AddMember(StringRef("title"), StringRef(prop.getLabel().c_str()),
                         allocator);
    if (!prop.getDescription().empty() &&
        prop.getDescription() != "no-description")
        jsonSchema.AddMember(StringRef("description"),
                             StringRef(prop.getDescription().c_str()),
                             allocator);
    addDefaultValue(jsonSchema, allocator, value);

    properties.AddMember(make_json_string(prop.getName(), allocator).Move(),
                         jsonSchema, allocator);
}

template <typename T>
void _addVectorPropertySchema(const Property& prop,
                              rapidjson::Value& properties,
                              rapidjson::Document::AllocatorType& allocator)
{
    using namespace rapidjson;
    auto value = prop.as<std::vector<T>>();
    auto jsonSchema = staticjson::export_json_schema(&value, &allocator);
    jsonSchema.AddMember(StringRef("title"), StringRef(prop.getLabel().c_str()),
                         allocator);
    if (!prop.getDescription().empty() &&
        prop.getDescription() != "no-description")
        jsonSchema.AddMember(StringRef("description"),
                             StringRef(prop.getDescription().c_str()),
                             allocator);
    addDefaultValue(jsonSchema, allocator, value);

    properties.AddMember(make_json_string(prop.getName(), allocator).Move(),
                         jsonSchema, allocator);
}

void _addBoolVectorPropertySchema(const Property& prop,
                                  rapidjson::Value& properties,
                                  rapidjson::Document::AllocatorType& allocator)
{
    using namespace rapidjson;
    const auto& boolValue = prop.as<std::vector<bool>>();
    std::vector<char> value;
    value.reserve(boolValue.size());
    for (const auto b : boolValue)
        value.push_back(b ? 1 : 0);
    auto jsonSchema = staticjson::export_json_schema(&value, &allocator);
    jsonSchema.AddMember(StringRef("title"), StringRef(prop.getLabel().c_str()),
                         allocator);
    if (!prop.getDescription().empty() &&
        prop.getDescription() != "no-description")
        jsonSchema.AddMember(StringRef("description"),
                             StringRef(prop.getDescription().c_str()),
                             allocator);
    addDefaultValue(jsonSchema, allocator, value);

    properties.AddMember(make_json_string(prop.getName(), allocator).Move(),
                         jsonSchema, allocator);
}

// Serialize given vec property to JSON.
template <typename T>
void _arrayPropertyToJson(rapidjson::Document& document, Property& prop)
{
    rapidjson::Value array(rapidjson::kArrayType);
    auto& value = prop.as<T>();
    for (int i = 0; i < value.length(); ++i)
        array.PushBack(value[i], document.GetAllocator());

    document.AddMember(
        make_json_string(prop.getName(), document.GetAllocator()).Move(), array,
        document.GetAllocator());
}

template <typename T>
void _vectorPropertyToJson(rapidjson::Document& document, Property& prop)
{
    rapidjson::Value array(rapidjson::kArrayType);
    const auto vec = prop.as<std::vector<T>>();
    for (const auto& v : vec)
        array.PushBack(v, document.GetAllocator());

    document.AddMember(
        make_json_string(prop.getName(), document.GetAllocator()).Move(), array,
        document.GetAllocator());
}

void _stringVectorPropertyToJson(rapidjson::Document& document, Property& prop)
{
    rapidjson::Value array(rapidjson::kArrayType);
    const auto vec = prop.as<std::vector<std::string>>();
    for (const auto& v : vec)
        array.PushBack(make_value_string(v, document.GetAllocator()).Move(),
                       document.GetAllocator());

    document.AddMember(
        make_json_string(prop.getName(), document.GetAllocator()).Move(), array,
        document.GetAllocator());
}

template<typename T, int S>
constexpr int _getGlmVecSize(const glm::vec<S, T>&)
{
    return S;
}

} // namespace

// Create JSON schema for a property map and add it to the given propSchema
// parent.
void _addPropertyMapSchema(const PropertyMap& propertyMap,
                           const std::string& title,
                           rapidjson::Document::AllocatorType& allocator,
                           rapidjson::Value& propSchema,
                           bool addRequired = false)
{
    using namespace rapidjson;
    propSchema.AddMember(StringRef("title"), StringRef(title.c_str()),
                         allocator);
    propSchema.AddMember(StringRef("type"), StringRef("object"), allocator);

    Value properties(kObjectType);
    Value required(kArrayType);
    for (const auto &prop : propertyMap)
    {
        auto addScalar = [&](const auto& value)
        {
            using T = std::decay_t<decltype(value)>;
            _addPropertySchema<T>(prop, properties, allocator);
        };

        auto addGlm = [&](const auto& value)
        {
            using T = typename std::decay_t<decltype(value)>::value_type;
            constexpr auto size = _getGlmVecSize(value);
            _addGlmPropertySchema<T, size>(prop, properties, allocator);
        };

        auto addVector = [&](const auto& value)
        {
            using T = typename std::decay_t<decltype(value)>::value_type;
            _addVectorPropertySchema<T>(prop, properties, allocator);
        };

        auto addBoolVector = [&](const auto& value)
        { return _addBoolVectorPropertySchema(prop, properties, allocator); };

        prop.visit<double>(addScalar);
        prop.visit<int32_t>(addScalar);
        prop.visit<uint32_t>(addScalar);
        prop.visit<uint64_t>(addScalar);
        prop.visit<std::string>(addScalar);
        prop.visit<bool>(addScalar);
        prop.visit<Vector2d>(addGlm);
        prop.visit<Vector2i>(addGlm);
        prop.visit<Vector3d>(addGlm);
        prop.visit<Vector3i>(addGlm);
        prop.visit<Vector4d>(addGlm);
        prop.visit<std::vector<double>>(addVector);
        prop.visit<std::vector<float>>(addVector);
        prop.visit<std::vector<int32_t>>(addVector);
        prop.visit<std::vector<uint32_t>>(addVector);
        prop.visit<std::vector<uint64_t>>(addVector);
        prop.visit<std::vector<std::string>>(addVector);
        prop.visit<std::vector<bool>>(addBoolVector);

        if (addRequired && !prop.isReadOnly())
            required.PushBack(make_json_string(prop.getName(), allocator),
                              allocator);
    }

    propSchema.AddMember(StringRef("properties"), properties, allocator);
    if (addRequired)
        propSchema.AddMember(StringRef("required"), required, allocator);
}

// Create JSON schema for a list of property maps and add it to the given oneOf
// parent.
void _addPropertyMapOneOfSchema(
    const std::vector<std::pair<std::string, PropertyMap>>& objs,
    rapidjson::Document::AllocatorType& allocator, rapidjson::Value& oneOf)
{
    using namespace rapidjson;
    for (const auto& obj : objs)
    {
        Value propSchema(kObjectType);
        _addPropertyMapSchema(obj.second, obj.first, allocator, propSchema);
        oneOf.PushBack(propSchema, allocator);
    }
}

// Create JSON schema for an RPC request returning one of the provided property
// maps.
std::string buildJsonRpcSchemaRequestPropertyMaps(
    const RpcDescription& desc,
    const std::vector<std::pair<std::string, PropertyMap>>& objs)
{
    using namespace rapidjson;
    auto schema = _buildJsonRpcSchema(desc);
    auto& allocator = schema.GetAllocator();

    Value returns(kObjectType);
    Value oneOf(kArrayType);
    _addPropertyMapOneOfSchema(objs, allocator, oneOf);
    returns.AddMember(StringRef("oneOf"), oneOf, allocator);
    schema.AddMember(StringRef("returns"), returns, allocator);

    Value params(kArrayType);
    schema.AddMember(StringRef("params"), params, allocator);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    schema.Accept(writer);
    return buffer.GetString();
}

// Create JSON schema for an RPC request returning the provided property map.
std::string buildJsonRpcSchemaRequestPropertyMap(const RpcDescription& desc,
                                                 const PropertyMap& obj)
{
    using namespace rapidjson;
    auto schema = _buildJsonRpcSchema(desc);
    auto& allocator = schema.GetAllocator();

    Value returns(kObjectType);
    _addPropertyMapSchema(obj, "", allocator, returns);
    schema.AddMember(StringRef("returns"), returns, allocator);

    Value params(kArrayType);
    schema.AddMember(StringRef("params"), params, allocator);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    schema.Accept(writer);
    return buffer.GetString();
}

// Create JSON schema for an RPC request accepting the input property map and
// returning the output property map.
std::string buildJsonRpcSchemaRequestPropertyMap(
    const RpcParameterDescription& desc, const PropertyMap& input,
    const PropertyMap& output)
{
    using namespace rapidjson;
    auto schema = _buildJsonRpcSchema(desc);
    auto& allocator = schema.GetAllocator();

    Value returns(kObjectType);
    _addPropertyMapSchema(output, "", allocator, returns);
    schema.AddMember(StringRef("returns"), returns, allocator);

    Value params(kArrayType);
    Value param(kObjectType);
    _addPropertyMapSchema(input, desc.paramName, allocator, param, true);
    param.AddMember(StringRef("name"), StringRef(desc.paramName.c_str()),
                    allocator);
    param.AddMember(StringRef("description"),
                    StringRef(desc.paramDescription.c_str()), allocator);
    params.PushBack(param, allocator);
    schema.AddMember(StringRef("params"), params, allocator);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    schema.Accept(writer);
    return buffer.GetString();
}

// Create JSON schema for an RPC notification accepting one of the provided
// property maps.
std::string buildJsonRpcSchemaNotifyPropertyMaps(
    const RpcParameterDescription& desc,
    const std::vector<std::pair<std::string, PropertyMap>>& objs)
{
    using namespace rapidjson;
    auto schema = _buildJsonRpcSchema(desc);
    auto& allocator = schema.GetAllocator();

    bool retVal;
    auto retSchema = staticjson::export_json_schema(&retVal);
    schema.AddMember(StringRef("returns"), retSchema, allocator);

    Value params(kArrayType);
    Value oneOf(kArrayType);
    Value returns(kObjectType);
    _addPropertyMapOneOfSchema(objs, allocator, oneOf);
    returns.AddMember(StringRef("oneOf"), oneOf, allocator);
    params.PushBack(returns, allocator);
    schema.AddMember(StringRef("params"), params, allocator);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    schema.Accept(writer);
    return buffer.GetString();
}

// Create JSON schema for an RPC notification accepting the provided property
// map.
std::string buildJsonRpcSchemaNotifyPropertyMap(
    const RpcParameterDescription& desc, const PropertyMap& properties)
{
    using namespace rapidjson;
    auto schema = _buildJsonRpcSchema(desc);
    auto& allocator = schema.GetAllocator();

    bool retVal;
    auto retSchema = staticjson::export_json_schema(&retVal);
    schema.AddMember(StringRef("returns"), retSchema, allocator);

    Value params(kArrayType);
    Value param(kObjectType);
    _addPropertyMapSchema(properties, desc.paramName, allocator, param, true);
    param.AddMember(StringRef("name"), StringRef(desc.paramName.c_str()),
                    allocator);
    param.AddMember(StringRef("description"),
                    StringRef(desc.paramDescription.c_str()), allocator);
    params.PushBack(param, allocator);
    schema.AddMember(StringRef("params"), params, allocator);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    schema.Accept(writer);
    return buffer.GetString();
}

// Create JSON schema for an object where its properties is a oneOf list of the
// given property maps.
template <>
std::string buildJsonSchema(
    std::vector<std::pair<std::string, PropertyMap>>& objs,
    const std::string& title)
{
    using namespace rapidjson;

    Document schema(kObjectType);
    auto& allocator = schema.GetAllocator();
    schema.AddMember(StringRef("type"), StringRef("object"), allocator);
    schema.AddMember(StringRef("title"), StringRef(title.c_str()), allocator);
    Value oneOf(kArrayType);
    _addPropertyMapOneOfSchema(objs, allocator, oneOf);
    schema.AddMember(StringRef("oneOf"), oneOf, allocator);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    schema.Accept(writer);
    return buffer.GetString();
}

// Create JSON schema for the given property map.
template <>
std::string buildJsonSchema(const PropertyMap& property,
                            const std::string& title)
{
    using namespace rapidjson;

    Document schema(kObjectType);
    auto& allocator = schema.GetAllocator();
    _addPropertyMapSchema(property, title, allocator, schema);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    schema.Accept(writer);
    return buffer.GetString();
}
} // namespace brayns

template <>
inline std::string to_json(const brayns::PropertyMap& obj)
{
    using namespace rapidjson;
    using brayns::Property;
    Document json(kObjectType);
    auto& allocator = json.GetAllocator();

    for (auto& prop : obj)
    {
        auto& key = prop.getName();

        auto addScalar = [&](const auto& value)
        {
            using T = std::decay_t<decltype(value)>;
            auto jsonKey = brayns::make_json_string(key, allocator);
            auto& jsonValue = prop.as<T>();
            json.AddMember(jsonKey.Move(), jsonValue, allocator);
        };

        auto addString = [&](const auto& value)
        {
            auto jsonKey = brayns::make_json_string(key, allocator);
            auto jsonValue = brayns::make_value_string(value, allocator);
            json.AddMember(jsonKey.Move(), jsonValue.Move(), allocator);
        };

        auto addEnum = [&](const auto& value) { addString(value.toString()); };

        auto addGlm = [&](const auto& value)
        {
            rapidjson::Value array(rapidjson::kArrayType);
            for (int i = 0; i < value.length(); ++i)
            {
                array.PushBack(value[i], allocator);
            }
            auto jsonKey = brayns::make_json_string(key, allocator);
            json.AddMember(jsonKey.Move(), array.Move(), allocator);
        };

        auto addVector = [&](const auto& value)
        {
            rapidjson::Value array(rapidjson::kArrayType);
            for (const auto& item : value)
            {
                array.PushBack(item, allocator);
            }
            auto jsonKey = brayns::make_json_string(key, allocator);
            json.AddMember(jsonKey.Move(), array.Move(), allocator);
        };

        auto addStringVector = [&](const auto& value)
        {
            rapidjson::Value array(rapidjson::kArrayType);
            for (const auto& item : value)
            {
                auto jsonValue = brayns::make_value_string(item, allocator);
                array.PushBack(jsonValue.Move(), allocator);
            }
            auto jsonKey = brayns::make_json_string(key, allocator);
            json.AddMember(jsonKey.Move(), array.Move(), allocator);
        };

        prop.visit<brayns::EnumProperty>(addEnum);
        prop.visit<double>(addScalar);
        prop.visit<int32_t>(addScalar);
        prop.visit<uint32_t>(addScalar);
        prop.visit<uint64_t>(addScalar);
        prop.visit<bool>(addScalar);
        prop.visit<std::string>(addString);
        prop.visit<brayns::Vector2d>(addGlm);
        prop.visit<brayns::Vector2i>(addGlm);
        prop.visit<brayns::Vector3d>(addGlm);
        prop.visit<brayns::Vector3i>(addGlm);
        prop.visit<brayns::Vector4d>(addGlm);
        prop.visit<std::vector<double>>(addVector);
        prop.visit<std::vector<float>>(addVector);
        prop.visit<std::vector<int32_t>>(addVector);
        prop.visit<std::vector<bool>>(addVector);
        prop.visit<std::vector<uint32_t>>(addVector);
        prop.visit<std::vector<uint64_t>>(addVector);
        prop.visit<std::vector<std::string>>(addStringVector);
    }

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    json.Accept(writer);
    return buffer.GetString();
}

/////////////////////////////////////////////////////////////////////////////

template <typename T>
T getValue(const rapidjson::GenericValue<rapidjson::UTF8<>>& v);
template <>
float getValue(const rapidjson::GenericValue<rapidjson::UTF8<>>& v)
{
    return v.GetFloat();
}
template <>
double getValue(const rapidjson::GenericValue<rapidjson::UTF8<>>& v)
{
    return v.GetDouble();
}
template <>
int32_t getValue(const rapidjson::GenericValue<rapidjson::UTF8<>>& v)
{
    return v.GetInt();
}
template <>
std::string getValue(const rapidjson::GenericValue<rapidjson::UTF8<>>& v)
{
    return v.GetString();
}
template <>
bool getValue(const rapidjson::GenericValue<rapidjson::UTF8<>>& v)
{
    return v.GetBool();
}
template <>
uint32_t getValue(const rapidjson::GenericValue<rapidjson::UTF8<>>& v)
{
    return v.GetUint();
}
template <>
uint64_t getValue(const rapidjson::GenericValue<rapidjson::UTF8<>>& v)
{
    return v.GetUint64();
}
template <>
char getValue(const rapidjson::GenericValue<rapidjson::UTF8<>>& v)
{
    return static_cast<char>(v.GetInt());
}

//////////////////////////////////////////////////////////////////////

template <typename T>
bool isValue(const rapidjson::GenericValue<rapidjson::UTF8<>>& v);
template <>
bool isValue<float>(const rapidjson::GenericValue<rapidjson::UTF8<>>& v)
{
    return v.IsFloat();
}
template <>
bool isValue<double>(const rapidjson::GenericValue<rapidjson::UTF8<>>& v)
{
    return v.IsDouble();
}
template <>
bool isValue<int>(const rapidjson::GenericValue<rapidjson::UTF8<>>& v)
{
    return v.IsInt();
}
template <>
bool isValue<std::string>(const rapidjson::GenericValue<rapidjson::UTF8<>>& v)
{
    return v.IsString();
}
template <>
bool isValue<bool>(const rapidjson::GenericValue<rapidjson::UTF8<>>& v)
{
    return v.IsBool();
}
template <>
bool isValue<uint32_t>(const rapidjson::GenericValue<rapidjson::UTF8<>>& v)
{
    return v.IsUint();
}
template <>
bool isValue<uint64_t>(const rapidjson::GenericValue<rapidjson::UTF8<>>& v)
{
    return v.IsUint64();
}
template <>
bool isValue<char>(const rapidjson::GenericValue<rapidjson::UTF8<>>& v)
{
    return v.IsInt();
}

//////////////////////////////////////////////////////////////////////

template <int S, typename T>
bool get_glm(const rapidjson::Value& v, glm::vec<S, T>& val)
{
    if (!v.IsArray() || v.Size() != S)
    {
        return false;
    }
    auto array = v.GetArray();
    for (int i = 0; i < S; ++i)
    {
        auto& item = array[i];
        if (!isValue<T>(item))
        {
            return false;
        }
        val[i] = getValue<T>(item);
    }
    return true;
}

template <typename T>
bool get_vector(const rapidjson::Value& v, std::vector<T>& val)
{
    if (!v.IsArray())
        return false;
    const auto jsonArray = v.GetArray();
    val.reserve(jsonArray.Size());
    for (const auto& i : jsonArray)
    {
        if (!isValue<T>(i))
            return false;
        val.push_back(getValue<T>(i));
    }

    return true;
}

bool get_bool_vector(const rapidjson::Value& v, std::vector<bool>& val)
{
    if (!v.IsArray())
        return false;
    const auto jsonArray = v.GetArray();
    val.reserve(jsonArray.Size());
    for (const auto& i : jsonArray)
    {
        if (!isValue<bool>(i))
            return false;
        val.push_back(getValue<bool>(i));
    }

    return true;
}

template <typename T>
bool get_property(const rapidjson::Value& v, T& val)
{
    if (!isValue<T>(v))
        return false;
    val = getValue<T>(v);

    return true;
}

template <>
inline bool from_json(brayns::PropertyMap& obj, const std::string& json)
{
    obj.merge(jsonToPropertyMap(json));
    return true;
}

using RapidJSONProperty = rapidjson::GenericMember<
    rapidjson::UTF8<char>,
    rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>;

template <typename T>
inline bool trySetPropertyValue(const RapidJSONProperty& jsonProp,
                                brayns::PropertyMap& map)
{
    T val;
    if (get_property(jsonProp.value, val))
    {
        const auto propName =
            brayns::snakeCaseToCamelCase(jsonProp.name.GetString());
        map.add({propName, val, {propName}});
        return true;
    }
    return false;
}

template <typename T, int S>
inline bool trySetPropertyGlm(const RapidJSONProperty& jsonProp,
                              brayns::PropertyMap& map)
{
    glm::vec<S, T> val;
    if (get_glm(jsonProp.value, val))
    {
        auto propName = brayns::snakeCaseToCamelCase(jsonProp.name.GetString());
        map.add({propName, val, {propName}});
        return true;
    }
    return false;
}

template <typename T>
inline bool trySetPropertyVector(const RapidJSONProperty& jsonProp,
                                 brayns::PropertyMap& map)
{
    std::vector<T> val;
    if (get_vector(jsonProp.value, val))
    {
        const auto propName =
            brayns::snakeCaseToCamelCase(jsonProp.name.GetString());
        map.add({propName, val, {propName}});
        return true;
    }
    return false;
}

inline bool trySetPropertyBoolVector(const RapidJSONProperty& jsonProp,
                                     brayns::PropertyMap& map)
{
    std::vector<bool> val;
    if (get_bool_vector(jsonProp.value, val))
    {
        const auto propName =
            brayns::snakeCaseToCamelCase(jsonProp.name.GetString());
        map.add({propName, val, {propName}});
        return true;
    }
    return false;
}

inline void enforceSetProperty(const RapidJSONProperty& jsonProp,
                               brayns::PropertyMap& map,
                               const brayns::Property& prop)
{
    auto& propName = prop.getName();

    if (prop.is<int32_t>())
    {
        if (!jsonProp.value.IsNumber() || !jsonProp.value.IsInt())
            throw std::runtime_error(
                "JSON does not comply with message schema:  "
                "Cannot get " +
                propName + " as int");
        const int32_t val = jsonProp.value.GetInt();
        map.add({propName, val, {propName}});
        return;
    }
    if (prop.is<uint32_t>())
    {
        if (!jsonProp.value.IsNumber() || !jsonProp.value.IsUint())
            throw std::runtime_error(
                "JSON does not comply with message schema:  "
                "Cannot get " +
                propName + " as unsigned int");
        const uint32_t val = jsonProp.value.GetUint();
        map.add({propName, val, {propName}});
        return;
    }
    if (prop.is<uint64_t>())
    {
        if (!jsonProp.value.IsNumber() || !jsonProp.value.IsUint64())
            throw std::runtime_error(
                "JSON does not comply with message schema:  "
                "Cannot get " +
                propName + " as unsigned int 64");
        map.add({propName, jsonProp.value.GetUint64(), {propName}});
        return;
    }
    if (prop.is<double>())
    {
        if (!jsonProp.value.IsNumber())
            throw std::runtime_error(
                "JSON does not comply with message schema:  "
                "Cannot get " +
                propName + " as double");
        map.add({propName, jsonProp.value.GetDouble(), {propName}});
        return;
    }
    if (prop.is<std::string>())
    {
        if (!jsonProp.value.IsString())
            throw std::runtime_error(
                "JSON does not comply with message schema:  "
                "Cannot get " +
                propName + " as string");
        map.add(
            {propName, std::string(jsonProp.value.GetString()), {propName}});
        return;
    }
    if (prop.is<bool>())
    {
        if (!jsonProp.value.IsBool())
            throw std::runtime_error(
                "JSON does not comply with message schema:  "
                "Cannot get " +
                propName + " as bool");
        map.add({propName, jsonProp.value.GetBool(), {propName}});
        return;
    }
    if (prop.is<brayns::Vector2d>())
    {
        if (!trySetPropertyGlm<double, 2>(jsonProp, map))
            throw std::runtime_error(
                "JSON does not comply with message schema:  "
                "Cannot get " +
                propName + " as vec2");
        return;
    }
    if (prop.is<brayns::Vector3d>())
    {
        if (!trySetPropertyGlm<double, 3>(jsonProp, map))
            throw std::runtime_error(
                "JSON does not comply with message schema:  "
                "Cannot get " +
                propName + " as vec3");
        return;
    }
    if (prop.is<brayns::Vector4d>())
    {
        if (!trySetPropertyGlm<double, 4>(jsonProp, map))
            throw std::runtime_error(
                "JSON does not comply with message schema:  "
                "Cannot get " +
                propName + " as vec4");
        return;
    }
    if (prop.is<brayns::Vector2i>())
    {
        if (!trySetPropertyGlm<int32_t, 2>(jsonProp, map))
            throw std::runtime_error(
                "JSON does not comply with message schema:  "
                "Cannot get " +
                propName + " as vec2i");
        return;
    }
    if (prop.is<brayns::Vector3i>())
    {
        if (!trySetPropertyGlm<int32_t, 3>(jsonProp, map))
            throw std::runtime_error(
                "JSON does not comply with message schema:  "
                "Cannot get " +
                propName + " as vec3i");
        return;
    }
    if (prop.is<std::vector<double>>())
    {
        if (!jsonProp.value.IsArray())
            throw std::runtime_error(
                "JSON does not comply with message schema:  "
                "Cannot get " +
                propName + " as list of doubles");
        const auto arr = jsonProp.value.GetArray();
        std::vector<double> val;
        val.reserve(arr.Size());
        for (const auto& v : arr)
        {
            if (!v.IsNumber())
                throw std::runtime_error(
                    "JSON does not comply with message schema:  "
                    "Some values cannot be parsed into double");
            val.push_back(v.GetDouble());
        }
        map.add({propName, val, {propName}});
        return;
    }
    if (prop.is<std::vector<float>>())
    {
        if (!jsonProp.value.IsArray())
            throw std::runtime_error(
                "JSON does not comply with message schema:  "
                "Cannot get " +
                propName + " as list of floats");
        const auto arr = jsonProp.value.GetArray();
        std::vector<float> val;
        val.reserve(arr.Size());
        for (const auto& v : arr)
        {
            if (!v.IsNumber())
                throw std::runtime_error(
                    "JSON does not comply with message schema:  "
                    "Some values cannot be parsed into floats");
            val.push_back(v.GetFloat());
        }
        map.add({propName, val, {propName}});
        return;
    }
    if (prop.is<std::vector<int32_t>>())
    {
        if (!trySetPropertyVector<int32_t>(jsonProp, map))
            throw std::runtime_error(
                "JSON does not comply with message schema:  "
                "Cannot get " +
                propName + " as list of integers");
        return;
    }
    if (prop.is<std::vector<uint32_t>>())
    {
        if (!trySetPropertyVector<uint32_t>(jsonProp, map))
            throw std::runtime_error(
                "JSON does not comply with message schema:  "
                "Cannot get " +
                propName + " as list of unsigned integers");
        return;
    }
    if (prop.is<std::vector<uint64_t>>())
    {
        if (!trySetPropertyVector<uint64_t>(jsonProp, map))
            throw std::runtime_error(
                "JSON does not comply with message schema:  "
                "Cannot get " +
                propName + " as list of unsigned integers 64");
        return;
    }
    if (prop.is<std::vector<std::string>>())
    {
        if (!trySetPropertyVector<std::string>(jsonProp, map))
            throw std::runtime_error(
                "JSON does not comply with message schema:  "
                "Cannot get " +
                propName + " as list of strings");
        return;
    }
    if (prop.is<std::vector<bool>>())
    {
        if (!trySetPropertyBoolVector(jsonProp, map))
            throw std::runtime_error(
                "JSON does not comply with message schema:  "
                "Cannot get " +
                propName + " as list of booleans");
        return;
    }
}

inline void trySetProperty(const RapidJSONProperty& jsonProp,
                           brayns::PropertyMap& map)
{
    if (trySetPropertyValue<double>(jsonProp, map))
        return;
    if (trySetPropertyValue<int32_t>(jsonProp, map))
        return;
    if (trySetPropertyValue<uint32_t>(jsonProp, map))
        return;
    if (trySetPropertyValue<uint64_t>(jsonProp, map))
        return;
    if (trySetPropertyValue<std::string>(jsonProp, map))
        return;
    if (trySetPropertyValue<bool>(jsonProp, map))
        return;
    if (trySetPropertyGlm<double, 2>(jsonProp, map))
        return;
    if (trySetPropertyGlm<int32_t, 2>(jsonProp, map))
        return;
    if (trySetPropertyGlm<double, 3>(jsonProp, map))
        return;
    if (trySetPropertyGlm<int32_t, 3>(jsonProp, map))
        return;
    if (trySetPropertyGlm<double, 4>(jsonProp, map))
        return;
    if (trySetPropertyVector<float>(jsonProp, map))
        return;
    if (trySetPropertyVector<double>(jsonProp, map))
        return;
    if (trySetPropertyVector<int32_t>(jsonProp, map))
        return;
    if (trySetPropertyVector<uint32_t>(jsonProp, map))
        return;
    if (trySetPropertyVector<uint64_t>(jsonProp, map))
        return;
    if (trySetPropertyVector<std::string>(jsonProp, map))
        return;
    if (trySetPropertyBoolVector(jsonProp, map))
        return;
}

brayns::PropertyMap jsonToPropertyMap(const std::string& json,
                                      const brayns::PropertyMap& schema)
{
    using namespace rapidjson;
    using brayns::Property;
    Document document;
    document.Parse(json.c_str());

    brayns::PropertyMap map;

    if (!document.IsObject())
        return map;

    for (const auto& m : document.GetObject())
    {
        const auto propName = brayns::snakeCaseToCamelCase(m.name.GetString());

        // If we have the schema, we know what we have to set
        if (auto prop = schema.find(propName))
            enforceSetProperty(m, map, *prop);
        else if (schema.empty()) // Only accept non schema values when theres no
                                 // schema
            trySetProperty(m, map);
        else
            throw std::runtime_error("Unknown property \"" +
                                     std::string(m.name.GetString()) + "\"");
    }

    return map;
}
