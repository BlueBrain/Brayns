/* Copyright (c) 2021 EPFL/Blue Brain Project
 *
 * Responsible Author: adrien.fleury@epfl.ch
 *
 * PropertyMaphis file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * PropertyMaphis library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 3.0 as published by the Free Software Foundation.
 *
 * PropertyMaphis library is distributed in the hope that it will be useful, but
 * WIPropertyMapHOUPropertyMap ANY WARRANPropertyMapY; without even the implied
 * warranty of MERCHANPropertyMapABILIPropertyMapY or FIPropertyMapNESS FOR A
 * PARPropertyMapICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#pragma once

#include <brayns/common/propertymap/PropertyMap.h>

#include <brayns/network/json/Json.h>

namespace brayns
{
template <>
struct JsonAdapter<EnumProperty>
{
    static JsonSchema getSchema(const EnumProperty& value)
    {
        JsonSchema schema;
        schema.type = JsonType::String;
        auto& enums = schema.enums;
        auto& values = value.getValues();
        enums.reserve(values.size());
        for (const auto& key : values)
        {
            enums.emplace_back(key);
        }
        return schema;
    }

    static bool serialize(const EnumProperty& value, JsonValue& json)
    {
        json = value.toString();
        return true;
    }

    static bool deserialize(const JsonValue& json, EnumProperty& value)
    {
        if (json.isString())
        {
            auto& key = json.extract<std::string>();
            if (!value.find(key))
            {
                return false;
            }
            value = key;
            return true;
        }
        if (json.isNumeric() && !json.isBoolean())
        {
            auto index = json.convert<int>();
            if (!value.isValidIndex(index))
            {
                return false;
            }
            value = index;
            return true;
        }
        return false;
    }
};

class PropertyVisitor
{
public:
    template <typename T, typename FunctorType>
    static bool visit(T& property, FunctorType functor)
    {
        return property.template visit<EnumProperty>(functor) ||
               property.template visit<float>(functor) ||
               property.template visit<double>(functor) ||
               property.template visit<int32_t>(functor) ||
               property.template visit<uint32_t>(functor) ||
               property.template visit<int64_t>(functor) ||
               property.template visit<uint64_t>(functor) ||
               property.template visit<std::string>(functor) ||
               property.template visit<bool>(functor) ||
               property.template visit<Vector2d>(functor) ||
               property.template visit<Vector2i>(functor) ||
               property.template visit<Vector3d>(functor) ||
               property.template visit<Vector3i>(functor) ||
               property.template visit<Vector4d>(functor) ||
               property.template visit<std::vector<double>>(functor) ||
               property.template visit<std::vector<float>>(functor) ||
               property.template visit<std::vector<int32_t>>(functor) ||
               property.template visit<std::vector<uint32_t>>(functor) ||
               property.template visit<std::vector<int64_t>>(functor) ||
               property.template visit<std::vector<uint64_t>>(functor) ||
               property.template visit<std::vector<std::string>>(functor) ||
               property.template visit<std::vector<bool>>(functor);
    }
};

class PropertyMapSchema
{
public:
    static JsonSchema create(const PropertyMap& properties)
    {
        JsonSchema schema;
        schema.type = JsonType::Object;
        for (const auto& property : properties)
        {
            add(property, schema);
        }
        if (properties.empty())
        {
            JsonSchemaHelper::allowAnyAdditionalProperty(schema);
        }
        return schema;
    }

private:
    static void add(const Property& property, JsonSchema& schema)
    {
        auto functor = [&](const auto& value)
        {
            auto& name = property.getName();
            auto& properties = schema.properties;
            properties[name] = getSchema(property, value);
        };
        PropertyVisitor::visit(property, functor);
    }

    template <typename T>
    static JsonSchema getSchema(const Property& property, const T& value)
    {
        auto schema = Json::getSchema(value);
        schema.title = property.getLabel();
        auto& description = property.getDescription();
        if (!description.empty() && description != "no-description")
        {
            schema.description = description;
        }
        Json::serialize(value, schema.defaultValue);
        schema.readOnly = property.isReadOnly();
        return schema;
    }
};

class PropertyMapSerializer
{
public:
    static bool serialize(const PropertyMap& value, JsonValue& json)
    {
        auto object = Poco::makeShared<JsonObject>();
        for (const auto& property : value)
        {
            JsonValue child;
            if (!_serialize(property, child))
            {
                return false;
            }
            auto& name = property.getName();
            object->set(name, child);
        }
        json = object;
        return true;
    }

private:
    static bool _serialize(const Property& property, JsonValue& json)
    {
        bool success = false;
        auto functor = [&](const auto& value)
        { success = Json::serialize(value, json); };
        PropertyVisitor::visit(property, functor);
        return success;
    }
};

class PropertyFactory
{
public:
    static bool create(const std::string& name, const JsonValue& json,
                       PropertyMap& properties)
    {
        return _createPrimitive(name, json, properties) ||
               _createArray(name, json, properties);
    }

private:
    static bool _createPrimitive(const std::string& name, const JsonValue& json,
                                 PropertyMap& properties)
    {
        if (json.isBoolean())
        {
            properties.add({name, json.extract<bool>()});
            return true;
        }
        if (json.isInteger())
        {
            properties.add({name, json.convert<int64_t>()});
            return true;
        }
        if (json.isNumeric())
        {
            properties.add({name, json.convert<double>()});
            return true;
        }
        if (json.isString())
        {
            properties.add({name, json.extract<std::string>()});
            return true;
        }
        return false;
    }

    static bool _createArray(const std::string& name, const JsonValue& json,
                             PropertyMap& properties)
    {
        auto array = JsonHelper::extractArray(json);
        if (!array)
        {
            return false;
        }
        return _createGlm(name, *array, properties) ||
               _createVector(name, *array, properties);
    }

    static bool _createGlm(const std::string& name, const JsonArray& array,
                           PropertyMap& properties)
    {
        return _createGlm<2, int32_t>(name, array, properties) ||
               _createGlm<2, double>(name, array, properties) ||
               _createGlm<3, int32_t>(name, array, properties) ||
               _createGlm<3, double>(name, array, properties) ||
               _createGlm<4, double>(name, array, properties);
    }

    static bool _createVector(const std::string& name, const JsonArray& array,
                              PropertyMap& properties)
    {
        return _createVector<bool>(name, array, properties) ||
               _createVector<int64_t>(name, array, properties) ||
               _createVector<double>(name, array, properties) ||
               _createVector<std::string>(name, array, properties);
    }

    template <glm::length_t S, typename T>
    static bool _createGlm(const std::string& name, const JsonArray& array,
                           PropertyMap& properties)
    {
        if (array.size() != S)
        {
            return false;
        }
        glm::vec<S, T> value = {};
        glm::length_t i = 0;
        for (const auto& item : array)
        {
            if (!item.isNumeric())
            {
                return false;
            }
            if (std::is_integral<T>() && !item.isInteger())
            {
                return false;
            }
            value[i++] = item.convert<T>();
        }
        properties.add({name, value});
        return true;
    }

    template <typename T>
    static bool _createVector(const std::string& name, const JsonArray& array,
                              PropertyMap& properties)
    {
        auto required = GetJsonType::fromPrimitive<T>();
        std::vector<T> value;
        value.reserve(array.size());
        for (const auto& item : array)
        {
            auto itemType = GetJsonType::fromJson(item);
            if (!JsonTypeHelper::check(required, itemType))
            {
                return false;
            }
            value.push_back(item.convert<T>());
        }
        properties.add({name, std::move(value)});
        return true;
    }
};

class PropertyUpdater
{
public:
    static bool update(Property& property, const JsonValue& json)
    {
        bool success = false;
        auto functor = [&](auto& value)
        { success = Json::deserialize(json, value); };
        PropertyVisitor::visit(property, functor);
        return success;
    }
};

class PropertyMapDeserializer
{
public:
    static bool deserialize(const JsonValue& json, PropertyMap& value)
    {
        auto object = JsonHelper::extractObject(json);
        if (!object)
        {
            return false;
        }
        PropertyMap buffer;
        for (const auto& pair : *object)
        {
            auto& name = pair.first;
            auto& child = pair.second;
            if (!_deserialize(name, child, buffer))
            {
                return false;
            }
        }
        value.merge(buffer);
        return true;
    }

private:
    static bool _deserialize(const std::string& name, const JsonValue& json,
                             PropertyMap& properties)
    {
        auto property = properties.find(name);
        if (!property)
        {
            return PropertyFactory::create(name, json, properties);
        }
        return PropertyUpdater::update(*property, json);
    }
};

/**
 * @brief JSON handling of PropertyMap.
 *
 */
template <>
struct JsonAdapter<PropertyMap>
{
    /**
     * @brief Convert a PropertyMap to a JSON schema.
     *
     * @param value Input value.
     * @return JsonSchema JSON schema.
     */
    static JsonSchema getSchema(const PropertyMap& value)
    {
        return PropertyMapSchema::create(value);
    }

    /**
     * @brief Serialize a PropertyMap to JSON.
     *
     * @param value Input value.
     * @param json Output JSON.
     * @return true Success
     * @return false Failure.
     */
    static bool serialize(const PropertyMap& value, JsonValue& json)
    {
        return PropertyMapSerializer::serialize(value, json);
    }

    /**
     * @brief Deserialize a PropertyMap from JSON.
     *
     * @param json Input JSON.
     * @param value Output value.
     * @return true Success.
     * @return false Failure.
     */
    static bool deserialize(const JsonValue& json, PropertyMap& value)
    {
        return PropertyMapDeserializer::deserialize(json, value);
    }
};
} // namespace brayns