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

#include "jsonSerialization.h"
#include <brayns/common/PropertyMap.h>

namespace brayns
{
namespace
{
std::string camelCaseToSnakeCase(const std::string& camelCase)
{
    if (camelCase.empty())
        return camelCase;

    std::string str(1, tolower(camelCase[0]));
    for (auto it = camelCase.begin() + 1; it != camelCase.end(); ++it)
    {
        if (isupper(*it) && *(it - 1) != '-' && islower(*(it - 1)))
            str += "_";
        str += *it;
    }

    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

std::string snakeCaseToCamelCase(const std::string& hyphenated)
{
    std::string camel = hyphenated;

    for (size_t x = 0; x < camel.length(); x++)
    {
        if (camel[x] == '_')
        {
            std::string tempString = camel.substr(x + 1, 1);

            transform(tempString.begin(), tempString.end(), tempString.begin(),
                      toupper);

            camel.erase(x, 2);
            camel.insert(x, tempString);
        }
    }
    return camel;
}

rapidjson::Value make_json_string(const std::string& camelCase,
                                  rapidjson::Document::AllocatorType& allocator)
{
    const auto snake_case = camelCaseToSnakeCase(camelCase);
    rapidjson::Value val;
    val.SetString(snake_case.c_str(), snake_case.length(), allocator);
    return val;
}

template <typename T>
void _addPropertySchema(const PropertyMap::Property& prop,
                        rapidjson::Value& properties,
                        rapidjson::Document::AllocatorType& allocator)
{
    using namespace rapidjson;

    Document jsonSchema;
    if (prop.enums.empty())
    {
        auto value = prop.get<T>();
        jsonSchema = staticjson::export_json_schema(&value, &allocator);
        jsonSchema.AddMember(StringRef("title"), StringRef(prop.label.c_str()),
                             allocator);
        jsonSchema.AddMember(StringRef("readOnly"), prop.readOnly(), allocator);
        if (jsonSchema.HasMember("minimum"))
            jsonSchema["minimum"] = prop.min<T>();
        else
            jsonSchema.AddMember(StringRef("minimum"), prop.min<T>(),
                                 allocator);
        if (jsonSchema.HasMember("maximum"))
            jsonSchema["maximum"] = prop.max<T>();
        else
            jsonSchema.AddMember(StringRef("maximum"), prop.max<T>(),
                                 allocator);
    }
    else
    {
        jsonSchema.SetObject();
        jsonSchema.AddMember(StringRef("type"), StringRef("string"), allocator);
        Value enumerations(kArrayType);
        for (const auto& name : prop.enums)
        {
            enumerations.PushBack(StringRef(name.data(), name.size()),
                                  allocator);
        }
        jsonSchema.AddMember(StringRef("enum"), enumerations, allocator);
    }
    properties.AddMember(make_json_string(prop.name, allocator).Move(),
                         jsonSchema, allocator);
}

template <>
void _addPropertySchema<bool>(const PropertyMap::Property& prop,
                              rapidjson::Value& properties,
                              rapidjson::Document::AllocatorType& allocator)
{
    using namespace rapidjson;
    auto value = prop.get<bool>();
    auto jsonSchema = staticjson::export_json_schema(&value, &allocator);
    jsonSchema.AddMember(StringRef("title"), StringRef(prop.label.c_str()),
                         allocator);
    jsonSchema.AddMember(StringRef("readOnly"), prop.readOnly(), allocator);
    properties.AddMember(make_json_string(prop.name, allocator).Move(),
                         jsonSchema, allocator);
}

template <>
void _addPropertySchema<std::string>(
    const PropertyMap::Property& prop, rapidjson::Value& properties,
    rapidjson::Document::AllocatorType& allocator)
{
    using namespace rapidjson;
    auto value = prop.get<std::string>();
    auto jsonSchema = staticjson::export_json_schema(&value, &allocator);
    jsonSchema.AddMember(StringRef("title"), StringRef(prop.label.c_str()),
                         allocator);
    jsonSchema.AddMember(StringRef("readOnly"), prop.readOnly(), allocator);
    properties.AddMember(make_json_string(prop.name, allocator).Move(),
                         jsonSchema, allocator);
}

template <typename T, int S>
void _addArrayPropertySchema(const PropertyMap::Property& prop,
                             rapidjson::Value& properties,
                             rapidjson::Document::AllocatorType& allocator)
{
    using namespace rapidjson;
    auto value = prop.get<std::array<T, S>>();
    auto jsonSchema = staticjson::export_json_schema(&value, &allocator);
    jsonSchema.AddMember(StringRef("title"), StringRef(prop.label.c_str()),
                         allocator);
    properties.AddMember(make_json_string(prop.name, allocator).Move(),
                         jsonSchema, allocator);
}

template <typename T>
void _addArrayPropertyJSON(rapidjson::Document& document,
                           PropertyMap::Property& prop)
{
    rapidjson::Value array(rapidjson::kArrayType);
    for (const auto& val : prop.get<T>())
        array.PushBack(val, document.GetAllocator());

    document.AddMember(
        make_json_string(prop.name, document.GetAllocator()).Move(), array,
        document.GetAllocator());
}
}

void getPropsSchema(
    const std::vector<std::pair<std::string, PropertyMap>>& objs,
    rapidjson::Document::AllocatorType& allocator, rapidjson::Value& oneOf)
{
    using namespace rapidjson;
    for (const auto& obj : objs)
    {
        Value propSchema(kObjectType);
        propSchema.AddMember(StringRef("title"), StringRef(obj.first.c_str()),
                             allocator);
        propSchema.AddMember(StringRef("type"), StringRef("object"), allocator);

        Value properties(kObjectType);
        for (auto prop : obj.second.getProperties())
        {
            switch (prop->type)
            {
            case PropertyMap::Property::Type::Float:
                _addPropertySchema<float>(*prop, properties, allocator);
                break;
            case PropertyMap::Property::Type::Int:
                _addPropertySchema<int32_t>(*prop, properties, allocator);
                break;
            case PropertyMap::Property::Type::String:
                _addPropertySchema<std::string>(*prop, properties, allocator);
                break;
            case PropertyMap::Property::Type::Bool:
                _addPropertySchema<bool>(*prop, properties, allocator);
                break;
            case PropertyMap::Property::Type::Vec2f:
                _addArrayPropertySchema<float, 2>(*prop, properties, allocator);
                break;
            case PropertyMap::Property::Type::Vec2i:
                _addArrayPropertySchema<int32_t, 2>(*prop, properties,
                                                    allocator);
                break;
            case PropertyMap::Property::Type::Vec3f:
                _addArrayPropertySchema<float, 3>(*prop, properties, allocator);
                break;
            case PropertyMap::Property::Type::Vec3i:
                _addArrayPropertySchema<int32_t, 3>(*prop, properties,
                                                    allocator);
                break;
            case PropertyMap::Property::Type::Vec4f:
                _addArrayPropertySchema<float, 4>(*prop, properties, allocator);
                break;
            }
        }

        propSchema.AddMember(StringRef("properties"), properties, allocator);

        oneOf.PushBack(propSchema, allocator);
    }
}

std::string buildJsonRpcSchemaGetProperties(
    const std::string& title, const std::string& description,
    const std::vector<std::pair<std::string, PropertyMap>>& objs)
{
    using namespace rapidjson;
    Document schema(kObjectType);
    auto& allocator = schema.GetAllocator();
    schema.AddMember(StringRef("title"), StringRef(title.c_str()), allocator);
    schema.AddMember(StringRef("description"), StringRef(description.c_str()),
                     allocator);
    schema.AddMember(StringRef("type"), StringRef("method"), allocator);

    Value returns(kObjectType);
    Value oneOf(kArrayType);
    getPropsSchema(objs, allocator, oneOf);
    returns.AddMember(StringRef("oneOf"), oneOf, allocator);
    schema.AddMember(StringRef("returns"), returns, allocator);

    Value params(kArrayType);
    schema.AddMember(StringRef("params"), params, allocator);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    schema.Accept(writer);
    return buffer.GetString();
}

std::string buildJsonRpcSchemaSetProperties(
    const std::string& title, const RpcDocumentation& doc,
    const std::vector<std::pair<std::string, PropertyMap>>& objs)
{
    using namespace rapidjson;
    Document schema(kObjectType);
    auto& allocator = schema.GetAllocator();
    schema.AddMember(StringRef("title"), StringRef(title.c_str()), allocator);
    schema.AddMember(StringRef("description"),
                     StringRef(doc.functionDescription.c_str()), allocator);
    schema.AddMember(StringRef("type"), StringRef("method"), allocator);

    bool retVal;
    auto retSchema = staticjson::export_json_schema(&retVal);
    schema.AddMember(StringRef("returns"), retSchema, allocator);

    Value params(kArrayType);
    Value oneOf(kArrayType);
    Value returns(kObjectType);
    getPropsSchema(objs, allocator, oneOf);
    returns.AddMember(StringRef("oneOf"), oneOf, allocator);
    params.PushBack(returns, allocator);
    schema.AddMember(StringRef("params"), params, allocator);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    schema.Accept(writer);
    return buffer.GetString();
}

template <>
std::string getSchema(std::vector<std::pair<std::string, PropertyMap>>& objs,
                      const std::string& title)
{
    using namespace rapidjson;

    Document schema(kObjectType);
    auto& allocator = schema.GetAllocator();
    schema.AddMember(StringRef("type"), StringRef("object"), allocator);
    schema.AddMember(StringRef("title"), StringRef(title.c_str()), allocator);
    Value oneOf(kArrayType);
    getPropsSchema(objs, allocator, oneOf);
    schema.AddMember(StringRef("oneOf"), oneOf, allocator);

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
    using brayns::PropertyMap;
    Document json(kObjectType);
    auto& allocator = json.GetAllocator();

    for (auto prop : obj.getProperties())
    {
        switch (prop->type)
        {
        case PropertyMap::Property::Type::Float:
            json.AddMember(
                brayns::make_json_string(prop->name, allocator).Move(),
                prop->get<float>(), allocator);
            break;
        case PropertyMap::Property::Type::Int:
            if (prop->enums.empty())
            {
                json.AddMember(
                    brayns::make_json_string(prop->name, allocator).Move(),
                    prop->get<int32_t>(), allocator);
            }
            else
            {
                json.AddMember(
                    brayns::make_json_string(prop->name, allocator).Move(),
                    StringRef(prop->enums[prop->get<int32_t>()].c_str()),
                    allocator);
            }
            break;
        case PropertyMap::Property::Type::String:
            json.AddMember(
                brayns::make_json_string(prop->name, allocator).Move(),
                StringRef(prop->get<std::string>().c_str()), allocator);
            break;
        case PropertyMap::Property::Type::Bool:
            json.AddMember(
                brayns::make_json_string(prop->name, allocator).Move(),
                prop->get<bool>(), allocator);
            break;
        case PropertyMap::Property::Type::Vec2f:
            brayns::_addArrayPropertyJSON<std::array<float, 2>>(json, *prop);
            break;
        case PropertyMap::Property::Type::Vec2i:
            brayns::_addArrayPropertyJSON<std::array<int32_t, 2>>(json, *prop);
            break;
        case PropertyMap::Property::Type::Vec3f:
            brayns::_addArrayPropertyJSON<std::array<float, 3>>(json, *prop);
            break;
        case PropertyMap::Property::Type::Vec3i:
            brayns::_addArrayPropertyJSON<std::array<int32_t, 3>>(json, *prop);
            break;
        case PropertyMap::Property::Type::Vec4f:
            brayns::_addArrayPropertyJSON<std::array<float, 4>>(json, *prop);
            break;
        }
    }

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    json.Accept(writer);
    return buffer.GetString();
}

#define SET_PROPERTY_NUMBER(P)                      \
    if (!m.value.IsNumber())                        \
        return false;                               \
    obj.updateProperty(propName, m.value.Get##P()); \
    break;

#define SET_PROPERTY(P)                                                     \
    {                                                                       \
        const auto& enums = obj.getEnums(propName);                         \
        if (enums.empty())                                                  \
        {                                                                   \
            if (!m.value.Is##P())                                           \
                return false;                                               \
            obj.updateProperty(propName, m.value.Get##P());                 \
        }                                                                   \
        else                                                                \
        {                                                                   \
            if (!m.value.IsString())                                        \
                return false;                                               \
            obj.updateProperty(                                             \
                propName,                                                   \
                int32_t(std::distance(enums.begin(),                        \
                                      std::find(enums.begin(), enums.end(), \
                                                m.value.GetString()))));    \
        }                                                                   \
        break;                                                              \
    }

#define SET_ARRAY(T, P, S)                       \
    {                                            \
        std::array<T, S> val;                    \
        int j = 0;                               \
        for (const auto& i : m.value.GetArray()) \
        {                                        \
            if (!i.IsNumber())                   \
                return false;                    \
            val[j++] = i.Get##P();               \
        }                                        \
        obj.updateProperty(propName, val);       \
        break;                                   \
    }

template <>
inline bool from_json(brayns::PropertyMap& obj, const std::string& json)
{
    using namespace rapidjson;
    using brayns::PropertyMap;
    Document document;
    document.Parse(json.c_str());

    if (!document.IsObject())
        return false;

    for (const auto& m : document.GetObject())
    {
        const auto propName = brayns::snakeCaseToCamelCase(m.name.GetString());
        if (!obj.hasProperty(propName))
            return false;
        switch (obj.getPropertyType(propName))
        {
        case PropertyMap::Property::Type::Float:
            SET_PROPERTY_NUMBER(Float)
        case PropertyMap::Property::Type::Int:
            SET_PROPERTY(Int)
        case PropertyMap::Property::Type::String:
            SET_PROPERTY(String)
        case PropertyMap::Property::Type::Bool:
            SET_PROPERTY(Bool)
        case PropertyMap::Property::Type::Vec2f:
            SET_ARRAY(float, Float, 2)
        case PropertyMap::Property::Type::Vec2i:
            SET_ARRAY(int32_t, Int, 2)
        case PropertyMap::Property::Type::Vec3f:
            SET_ARRAY(float, Float, 3)
        case PropertyMap::Property::Type::Vec3i:
            SET_ARRAY(int32_t, Int, 3)
        case PropertyMap::Property::Type::Vec4f:
            SET_ARRAY(float, Float, 4)
        }
    }
    return true;
}
