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

#include <brayns/common/PropertyMap.h>
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
}

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

template <typename T, size_t S>
void addDefaultValueArray(rapidjson::Document& document,
                          rapidjson::Document::AllocatorType& allocator,
                          const std::array<T, S>& value)
{
    rapidjson::Value arr(rapidjson::kArrayType);
    for (const auto v : value)
        arr.PushBack(v, allocator);
    document.AddMember(rapidjson::StringRef("default"), arr, allocator);
}

template <>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::array<double, 2>& value)
{
    addDefaultValueArray(document, allocator, value);
}

template <>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::array<int32_t, 2>& value)
{
    addDefaultValueArray(document, allocator, value);
}
template <>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::array<double, 3>& value)
{
    addDefaultValueArray(document, allocator, value);
}
template <>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::array<int32_t, 3>& value)
{
    addDefaultValueArray(document, allocator, value);
}
template <>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::array<double, 4>& value)
{
    addDefaultValueArray(document, allocator, value);
}

template<>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::vector<double>& value)
{
    rapidjson::Value arr(rapidjson::kArrayType);
    for (const auto v : value)
        arr.PushBack(v, allocator);
    document.AddMember(rapidjson::StringRef("default"), arr, allocator);
}

template<>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::vector<float>& value)
{
    rapidjson::Value arr(rapidjson::kArrayType);
    for (const auto v : value)
        arr.PushBack(v, allocator);
    document.AddMember(rapidjson::StringRef("default"), arr, allocator);
}

template<>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::vector<int32_t>& value)
{
    rapidjson::Value arr(rapidjson::kArrayType);
    for (const auto v : value)
        arr.PushBack(v, allocator);
    document.AddMember(rapidjson::StringRef("default"), arr, allocator);
}

template<>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::vector<uint32_t>& value)
{
    rapidjson::Value arr(rapidjson::kArrayType);
    for (const auto v : value)
        arr.PushBack(v, allocator);
    document.AddMember(rapidjson::StringRef("default"), arr, allocator);
}

template<>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::vector<uint64_t>& value)
{
    rapidjson::Value arr(rapidjson::kArrayType);
    for (const auto v : value)
        arr.PushBack(v, allocator);
    document.AddMember(rapidjson::StringRef("default"), arr, allocator);
}

template<>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::vector<char>& value)
{
    rapidjson::Value arr(rapidjson::kArrayType);
    for (const auto v : value)
        arr.PushBack(v, allocator);
    document.AddMember(rapidjson::StringRef("default"), arr, allocator);
}

template<>
void addDefaultValue(rapidjson::Document& document,
                     rapidjson::Document::AllocatorType& allocator,
                     const std::vector<std::string>& value)
{
    rapidjson::Value arr(rapidjson::kArrayType);
    for (const auto v : value)
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
    if (prop.enums.empty())
    {
        auto value = prop.get<T>();
        jsonSchema = staticjson::export_json_schema(&value, &allocator);
        jsonSchema.AddMember(StringRef("title"),
                             StringRef(prop.metaData.label.c_str()), allocator);
        addDefaultValue(jsonSchema, allocator, value);
        jsonSchema.AddMember(StringRef("readOnly"), prop.readOnly(), allocator);
        const auto minValue = prop.min<T>();
        const auto maxValue = prop.max<T>();
        const bool hasLimits = maxValue - minValue != 0;
        if (hasLimits)
        {
            if (jsonSchema.HasMember("minimum"))
                jsonSchema["minimum"] = minValue;
            else
                jsonSchema.AddMember(StringRef("minimum"), minValue, allocator);

            if (jsonSchema.HasMember("maximum"))
                jsonSchema["maximum"] = maxValue;
            else
                jsonSchema.AddMember(StringRef("maximum"), maxValue, allocator);
        }
        else
        {
            if (jsonSchema.HasMember("minimum"))
                jsonSchema.RemoveMember("minimum");
            if (jsonSchema.HasMember("maximum"))
                jsonSchema.RemoveMember("maximum");
        }
    }
    else
    {
        jsonSchema.SetObject();
        jsonSchema.AddMember(StringRef("type"), StringRef("string"), allocator);
        jsonSchema.AddMember(StringRef("title"),
                             StringRef(prop.metaData.label.c_str()), allocator);
        jsonSchema.AddMember(StringRef("readOnly"), prop.readOnly(), allocator);

        // Specialized enum code
        auto value = prop.get<int32_t>();
        const auto valueStr = matchEnum(prop.enums, value);
        addDefaultValue(jsonSchema, allocator, valueStr);

        Value enumerations(kArrayType);
        for (const auto& name : prop.enums)
            enumerations.PushBack(StringRef(name.data(), name.size()),
                                  allocator);
        jsonSchema.AddMember(StringRef("enum"), enumerations, allocator);
    }
    properties.AddMember(make_json_string(prop.name, allocator).Move(),
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
    auto value = prop.get<bool>();
    auto jsonSchema = staticjson::export_json_schema(&value, &allocator);
    jsonSchema.AddMember(StringRef("title"),
                         StringRef(prop.metaData.label.c_str()), allocator);
    addDefaultValue(jsonSchema, allocator, value);
    jsonSchema.AddMember(StringRef("readOnly"), prop.readOnly(), allocator);
    properties.AddMember(make_json_string(prop.name, allocator).Move(),
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

    const bool isEnum = !prop.enums.empty();
    auto value = prop.get<std::string>();

    Document jsonSchema;

    if (isEnum)
    {
        jsonSchema.SetObject();
        jsonSchema.AddMember(StringRef("type"), StringRef("string"), allocator);
        Value enumerations(kArrayType);
        for (const auto& name : prop.enums)
            enumerations.PushBack(StringRef(name.data(), name.size()),
                                  allocator);
        jsonSchema.AddMember(StringRef("enum"), enumerations, allocator);
    }
    else
    {
        jsonSchema = staticjson::export_json_schema(&value, &allocator);
    }

    addDefaultValue(jsonSchema, allocator, value);
    jsonSchema.AddMember(StringRef("readOnly"), prop.readOnly(), allocator);
    jsonSchema.AddMember(StringRef("title"),
                         StringRef(prop.metaData.label.c_str()), allocator);
    properties.AddMember(make_json_string(prop.name, allocator).Move(),
                         jsonSchema, allocator);
}

// Create JSON schema for given array property and add it to the given
// properties parent.
template <typename T, int S>
void _addArrayPropertySchema(const Property& prop, rapidjson::Value& properties,
                             rapidjson::Document::AllocatorType& allocator)
{
    using namespace rapidjson;
    auto value = prop.get<std::array<T, S>>();
    auto jsonSchema = staticjson::export_json_schema(&value, &allocator);
    jsonSchema.AddMember(StringRef("title"),
                         StringRef(prop.metaData.label.c_str()), allocator);
    addDefaultValue(jsonSchema, allocator, value);

    properties.AddMember(make_json_string(prop.name, allocator).Move(),
                         jsonSchema, allocator);
}

template <typename T>
void _addVectorPropertySchema(const Property& prop, rapidjson::Value& properties,
                             rapidjson::Document::AllocatorType& allocator)
{
    using namespace rapidjson;
    auto value = prop.get<std::vector<T>>();
    auto jsonSchema = staticjson::export_json_schema(&value, &allocator);
    jsonSchema.AddMember(StringRef("title"),
                         StringRef(prop.metaData.label.c_str()), allocator);
    addDefaultValue(jsonSchema, allocator, value);

    properties.AddMember(make_json_string(prop.name, allocator).Move(),
                         jsonSchema, allocator);
}

void _addBoolVectorPropertySchema(const Property& prop, rapidjson::Value& properties,
                                  rapidjson::Document::AllocatorType& allocator)
{
    using namespace rapidjson;
    const auto& boolValue = prop.getRef<std::vector<bool>>();
    std::vector<char> value;
    value.reserve(boolValue.size());
    for(const auto b : boolValue)
        value.push_back(b? 1 : 0);
    auto jsonSchema = staticjson::export_json_schema(&value, &allocator);
    jsonSchema.AddMember(StringRef("title"),
                         StringRef(prop.metaData.label.c_str()), allocator);
    addDefaultValue(jsonSchema, allocator, value);

    properties.AddMember(make_json_string(prop.name, allocator).Move(),
                         jsonSchema, allocator);
}

// Serialize given array property to JSON.
template <typename T>
void _arrayPropertyToJson(rapidjson::Document& document, Property& prop)
{
    rapidjson::Value array(rapidjson::kArrayType);
    for (const auto& val : prop.get<T>())
        array.PushBack(val, document.GetAllocator());

    document.AddMember(
        make_json_string(prop.name, document.GetAllocator()).Move(), array,
        document.GetAllocator());
}

template <typename T>
void _vectorPropertyToJson(rapidjson::Document& document, Property& prop)
{
    rapidjson::Value array(rapidjson::kArrayType);
    const auto vec = prop.get<std::vector<T>>();
    for(const auto& v : vec)
        array.PushBack(v, document.GetAllocator());

    document.AddMember(
        make_json_string(prop.name, document.GetAllocator()).Move(), array,
        document.GetAllocator());
}

void _stringVectorPropertyToJson(rapidjson::Document& document, Property& prop)
{
    rapidjson::Value array(rapidjson::kArrayType);
    const auto vec = prop.get<std::vector<std::string>>();
    for(const auto& v : vec)
        array.PushBack(make_value_string(v, document.GetAllocator()).Move(),
                       document.GetAllocator());

    document.AddMember(
        make_json_string(prop.name, document.GetAllocator()).Move(), array,
        document.GetAllocator());
}

}

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
    for (auto prop : propertyMap.getProperties())
    {
        switch (prop->type)
        {
        case Property::Type::Double:
            _addPropertySchema<double>(*prop, properties, allocator);
            break;
        case Property::Type::Int:
            _addPropertySchema<int32_t>(*prop, properties, allocator);
            break;
        case Property::Type::UInt:
            _addPropertySchema<uint32_t>(*prop, properties, allocator);
            break;
        case Property::Type::UInt64:
            _addPropertySchema<uint64_t>(*prop, properties, allocator);
            break;
        case Property::Type::String:
            _addPropertySchema<std::string>(*prop, properties, allocator);
            break;
        case Property::Type::Bool:
            _addPropertySchema<bool>(*prop, properties, allocator);
            break;
        case Property::Type::Vec2d:
            _addArrayPropertySchema<double, 2>(*prop, properties, allocator);
            break;
        case Property::Type::Vec2i:
            _addArrayPropertySchema<int32_t, 2>(*prop, properties, allocator);
            break;
        case Property::Type::Vec3d:
            _addArrayPropertySchema<double, 3>(*prop, properties, allocator);
            break;
        case Property::Type::Vec3i:
            _addArrayPropertySchema<int32_t, 3>(*prop, properties, allocator);
            break;
        case Property::Type::Vec4d:
            _addArrayPropertySchema<double, 4>(*prop, properties, allocator);
            break;
        case Property::Type::DoubleVector:
            _addVectorPropertySchema<double>(*prop, properties, allocator);
            break;
        case Property::Type::FloatVector:
            _addVectorPropertySchema<float>(*prop, properties, allocator);
            break;
        case Property::Type::IntVector:
            _addVectorPropertySchema<int32_t>(*prop, properties, allocator);
            break;
        case Property::Type::UIntVector:
            _addVectorPropertySchema<uint32_t>(*prop, properties, allocator);
            break;
        case Property::Type::UInt64Vector:
            _addVectorPropertySchema<uint64_t>(*prop, properties, allocator);
            break;
        case Property::Type::StringVector:
            _addVectorPropertySchema<std::string>(*prop, properties, allocator);
            break;
        case Property::Type::BoolVector:
            _addBoolVectorPropertySchema(*prop, properties, allocator);
            //_addVectorPropertySchema<char>(*prop, properties, allocator);
            break;
        }

        if(addRequired && !prop->readOnly())
            required.PushBack(make_json_string(prop->name, allocator), allocator);
    }

    propSchema.AddMember(StringRef("properties"), properties, allocator);
    if(addRequired)
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
    param.AddMember(StringRef("name"), StringRef(desc.paramName.c_str()), allocator);
    param.AddMember(StringRef("description"), StringRef(desc.paramDescription.c_str()), allocator);
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
    param.AddMember(StringRef("name"), StringRef(desc.paramName.c_str()), allocator);
    param.AddMember(StringRef("description"), StringRef(desc.paramDescription.c_str()), allocator);
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
}

template <>
inline std::string to_json(const brayns::PropertyMap& obj)
{
    using namespace rapidjson;
    using brayns::Property;
    Document json(kObjectType);
    auto& allocator = json.GetAllocator();

    for (auto prop : obj.getProperties())
    {
        switch (prop->type)
        {
        case Property::Type::Double:
            json.AddMember(
                brayns::make_json_string(prop->name, allocator).Move(),
                prop->get<double>(), allocator);
            break;
        case Property::Type::Int:
            if (prop->enums.empty())
            {
                json.AddMember(
                    brayns::make_json_string(prop->name, allocator).Move(),
                    prop->get<int32_t>(), allocator);
            }
            else
            {
                auto enumStr = matchEnum(prop->enums, prop->get<int32_t>());
                json.AddMember(
                    brayns::make_json_string(prop->name, allocator).Move(),
                    brayns::make_value_string(enumStr, allocator).Move(),
                    allocator);
            }
            break;
        case Property::Type::UInt:
            if (prop->enums.empty())
            {
                json.AddMember(
                    brayns::make_json_string(prop->name, allocator).Move(),
                    prop->get<uint32_t>(), allocator);
            }
            else
            {
                auto enumStr = matchEnum(prop->enums, prop->get<uint32_t>());
                json.AddMember(
                    brayns::make_json_string(prop->name, allocator).Move(),
                    brayns::make_value_string(enumStr, allocator).Move(),
                    allocator);
            }
            break;
        case Property::Type::UInt64:
            if (prop->enums.empty())
            {
                json.AddMember(
                    brayns::make_json_string(prop->name, allocator).Move(),
                    prop->get<uint64_t>(), allocator);
            }
            else
            {
                auto enumStr = matchEnum(prop->enums, prop->get<uint64_t>());
                json.AddMember(
                    brayns::make_json_string(prop->name, allocator).Move(),
                    brayns::make_value_string(enumStr, allocator).Move(),
                    allocator);
            }
            break;
        case Property::Type::String:
            json.AddMember(
                brayns::make_json_string(prop->name, allocator).Move(),
                brayns::make_value_string(prop->get<std::string>(), allocator)
                    .Move(),
                allocator);
            break;
        case Property::Type::Bool:
            json.AddMember(
                brayns::make_json_string(prop->name, allocator).Move(),
                prop->get<bool>(), allocator);
            break;
        case Property::Type::Vec2d:
            brayns::_arrayPropertyToJson<std::array<double, 2>>(json, *prop);
            break;
        case Property::Type::Vec2i:
            brayns::_arrayPropertyToJson<std::array<int32_t, 2>>(json, *prop);
            break;
        case Property::Type::Vec3d:
            brayns::_arrayPropertyToJson<std::array<double, 3>>(json, *prop);
            break;
        case Property::Type::Vec3i:
            brayns::_arrayPropertyToJson<std::array<int32_t, 3>>(json, *prop);
            break;
        case Property::Type::Vec4d:
            brayns::_arrayPropertyToJson<std::array<double, 4>>(json, *prop);
            break;
        case Property::Type::DoubleVector:
            brayns::_vectorPropertyToJson<double>(json, *prop);
            break;
        case Property::Type::FloatVector:
            brayns::_vectorPropertyToJson<float>(json, *prop);
            break;
        case Property::Type::IntVector:
            brayns::_vectorPropertyToJson<int32_t>(json, *prop);
            break;
        case Property::Type::BoolVector:
            brayns::_vectorPropertyToJson<bool>(json, *prop);
            //brayns::_vectorPropertyToJson<char>(json, *prop);
            break;
        case Property::Type::UIntVector:
            brayns::_vectorPropertyToJson<uint32_t>(json, *prop);
            break;
        case Property::Type::UInt64Vector:
            brayns::_vectorPropertyToJson<uint64_t>(json, *prop);
            break;
        case Property::Type::StringVector:
            brayns::_stringVectorPropertyToJson(json, *prop);
            break;
        }
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
bool isValue<float>(const rapidjson::GenericValue<rapidjson::UTF8<>>&v)
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

template <typename T, size_t S>
bool get_array(const rapidjson::Value& v, std::array<T, S>& val)
{
    if (!(v.IsArray() && v.Size() == S))
        return false;

    int j = 0;
    for (const auto& i : v.GetArray())
    {
        if (!isValue<T>(i))
            return false;
        val[j++] = getValue<T>(i);
    }
    return true;
}

template<typename T>
bool get_vector(const rapidjson::Value& v, std::vector<T>& val)
{
    if(!v.IsArray())
        return false;
    const auto jsonArray = v.GetArray();
    val.reserve(jsonArray.Size());
    for(const auto& i : jsonArray)
    {
        if(!isValue<T>(i))
            return false;
        val.push_back(getValue<T>(i));
    }

    return true;
}

bool get_bool_vector(const rapidjson::Value& v, std::vector<bool>& val)
{
    if(!v.IsArray())
        return false;
    const auto jsonArray = v.GetArray();
    val.reserve(jsonArray.Size());
    for(const auto& i : jsonArray)
    {
        if(!isValue<bool>(i))
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

using RapidJSONProperty = rapidjson::GenericMember<rapidjson::UTF8<char>,
                                rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>;


template<typename T>
inline bool trySetPropertyValue(const RapidJSONProperty& jsonProp, brayns::PropertyMap& map)
{
    T val;
    if(get_property(jsonProp.value, val))
    {
        const auto propName = brayns::snakeCaseToCamelCase(jsonProp.name.GetString());
        map.setProperty({propName, val, {propName}});
        return true;
    }
    return false;
}

template<typename T, size_t S>
inline bool trySetPropertyArray(const RapidJSONProperty& jsonProp, brayns::PropertyMap& map)
{
    std::array<T, S> val;
    if (get_array(jsonProp.value, val))
    {
        const auto propName = brayns::snakeCaseToCamelCase(jsonProp.name.GetString());
        map.setProperty({propName, val, {propName}});
        return true;
    }
    return false;
}

template<typename T>
inline bool trySetPropertyVector(const RapidJSONProperty& jsonProp, brayns::PropertyMap& map)
{
    std::vector<T> val;
    if (get_vector(jsonProp.value, val))
    {
        const auto propName = brayns::snakeCaseToCamelCase(jsonProp.name.GetString());
        map.setProperty({propName, val, {propName}});
        return true;
    }
    return false;
}

inline bool trySetPropertyBoolVector(const RapidJSONProperty& jsonProp, brayns::PropertyMap& map)
{
    std::vector<bool> val;
    if(get_bool_vector(jsonProp.value, val))
    {
        const auto propName = brayns::snakeCaseToCamelCase(jsonProp.name.GetString());
        map.setProperty({propName, val, {propName}});
        return true;
    }
    return false;
}

inline void enforceSetProperty(const RapidJSONProperty& jsonProp, brayns::PropertyMap& map,
                               const brayns::Property::Type type)
{
    const auto propName = brayns::snakeCaseToCamelCase(jsonProp.name.GetString());
    switch(type)
    {
    case brayns::Property::Type::Int:
    {
        if(!jsonProp.value.IsNumber() || !jsonProp.value.IsInt())
            throw std::runtime_error("JSON does not comply with message schema:  "
                                     "Cannot get " + propName + " as int");

        const int32_t val = jsonProp.value.GetInt();
        map.setProperty({propName, val, {propName}});
        break;
    }
    case brayns::Property::Type::UInt:
    {
        if(!jsonProp.value.IsNumber() || !jsonProp.value.IsUint())
            throw std::runtime_error("JSON does not comply with message schema:  "
                                     "Cannot get " + propName + " as unsigned int");
        const uint32_t val = jsonProp.value.GetUint();
        map.setProperty({propName, val, {propName}});
        break;
    }
    case brayns::Property::Type::UInt64:
    {
        if(!jsonProp.value.IsNumber() || !jsonProp.value.IsUint64())
            throw std::runtime_error("JSON does not comply with message schema:  "
                                     "Cannot get " + propName + " as unsigned int 64");
        map.setProperty({propName, jsonProp.value.GetUint64(), {propName}});
        break;
    }
    case brayns::Property::Type::Double:
    {
        if(!jsonProp.value.IsNumber())
            throw std::runtime_error("JSON does not comply with message schema:  "
                                     "Cannot get " + propName + " as double");
        map.setProperty({propName, jsonProp.value.GetDouble(), {propName}});
        break;
    }
    case brayns::Property::Type::String:
    {
        if(!jsonProp.value.IsString())
            throw std::runtime_error("JSON does not comply with message schema:  "
                                     "Cannot get " + propName + " as string");
        map.setProperty({propName, std::string(jsonProp.value.GetString()), {propName}});
        break;
    }
    case brayns::Property::Type::Bool:
    {
        if(!jsonProp.value.IsBool())
            throw std::runtime_error("JSON does not comply with message schema:  "
                                     "Cannot get " + propName + " as bool");
        map.setProperty({propName, jsonProp.value.GetBool(), {propName}});
        break;
    }
    case brayns::Property::Type::Vec2d:
    {
        if(!trySetPropertyArray<double, 2>(jsonProp, map))
            throw std::runtime_error("JSON does not comply with message schema:  "
                                     "Cannot get " + propName + " as vec2");
        break;
    }
    case brayns::Property::Type::Vec3d:
    {
        if(!trySetPropertyArray<double, 3>(jsonProp, map))
            throw std::runtime_error("JSON does not comply with message schema:  "
                                     "Cannot get " + propName + " as vec3");
        break;
    }
    case brayns::Property::Type::Vec4d:
    {
        if(!trySetPropertyArray<double, 4>(jsonProp, map))
            throw std::runtime_error("JSON does not comply with message schema:  "
                                     "Cannot get " + propName + " as vec4");
        break;
    }
    case brayns::Property::Type::Vec2i:
    {
        if(!trySetPropertyArray<int32_t, 2>(jsonProp, map))
            throw std::runtime_error("JSON does not comply with message schema:  "
                                     "Cannot get " + propName + " as vec2i");
        break;
    }
    case brayns::Property::Type::Vec3i:
    {
        if(!trySetPropertyArray<int32_t, 3>(jsonProp, map))
            throw std::runtime_error("JSON does not comply with message schema:  "
                                     "Cannot get " + propName + " as vec3i");
        break;
    }
    case brayns::Property::Type::DoubleVector:
    {
        if(!jsonProp.value.IsArray())
            throw std::runtime_error("JSON does not comply with message schema:  "
                                     "Cannot get " + propName + " as list of doubles");
        const auto arr = jsonProp.value.GetArray();
        std::vector<double> val;
        val.reserve(arr.Size());
        for(const auto& v : arr)
        {
            if(!v.IsNumber())
                throw std::runtime_error("JSON does not comply with message schema:  "
                                         "Some values cannot be parsed into double");
            val.push_back(v.GetDouble());
        }
        map.setProperty({propName, val, {propName}});
        break;
    }
    case brayns::Property::Type::FloatVector:
    {
        if(!jsonProp.value.IsArray())
            throw std::runtime_error("JSON does not comply with message schema:  "
                                     "Cannot get " + propName + " as list of floats");
        const auto arr = jsonProp.value.GetArray();
        std::vector<float> val;
        val.reserve(arr.Size());
        for(const auto& v : arr)
        {
            if(!v.IsNumber())
                throw std::runtime_error("JSON does not comply with message schema:  "
                                         "Some values cannot be parsed into floats");
            val.push_back(v.GetFloat());
        }
        map.setProperty({propName, val, {propName}});
        break;
    }
    case brayns::Property::Type::IntVector:
    {
        if(!trySetPropertyVector<int32_t>(jsonProp, map))
            throw std::runtime_error("JSON does not comply with message schema:  "
                                     "Cannot get " + propName + " as list of integers");
        break;
    }
    case brayns::Property::Type::UIntVector:
    {
        if(!trySetPropertyVector<uint32_t>(jsonProp, map))
            throw std::runtime_error("JSON does not comply with message schema:  "
                                     "Cannot get " + propName + " as list of unsigned integers");
        break;
    }
    case brayns::Property::Type::UInt64Vector:
    {
        if(!trySetPropertyVector<uint64_t>(jsonProp, map))
            throw std::runtime_error("JSON does not comply with message schema:  "
                                     "Cannot get " + propName + " as list of unsigned integers 64");
        break;
    }
    case brayns::Property::Type::StringVector:
    {
        if(!trySetPropertyVector<std::string>(jsonProp, map))
            throw std::runtime_error("JSON does not comply with message schema:  "
                                     "Cannot get " + propName + " as list of strings");
        break;
    }
    case brayns::Property::Type::BoolVector:
    {
        if(!trySetPropertyBoolVector(jsonProp, map))
            throw std::runtime_error("JSON does not comply with message schema:  "
                                     "Cannot get " + propName + " as list of booleans");
        break;
    }
    }
}

inline void trySetProperty(const RapidJSONProperty& jsonProp, brayns::PropertyMap& map)
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
    if (trySetPropertyArray<double, 2>(jsonProp, map))
        return;
    if (trySetPropertyArray<int32_t, 2>(jsonProp, map))
        return;
    if (trySetPropertyArray<double, 3>(jsonProp, map))
        return;
    if (trySetPropertyArray<int32_t, 3>(jsonProp, map))
        return;
    if (trySetPropertyArray<double, 4>(jsonProp, map))
        return;
    if(trySetPropertyVector<float>(jsonProp, map))
        return;
    if(trySetPropertyVector<double>(jsonProp, map))
        return;
    if(trySetPropertyVector<int32_t>(jsonProp, map))
        return;
    if(trySetPropertyVector<uint32_t>(jsonProp, map))
        return;
    if(trySetPropertyVector<uint64_t>(jsonProp, map))
        return;
    if(trySetPropertyVector<std::string>(jsonProp, map))
        return;
    if(trySetPropertyBoolVector(jsonProp, map))
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
        if(schema.hasProperty(propName))
            enforceSetProperty(m, map, schema.getPropertyType(propName));
        else if(schema.empty()) // Only accept non schema values when theres no schema
            trySetProperty(m, map);
    }

    return map;
}
