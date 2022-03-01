/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/engine/Camera.h>
#include <brayns/engine/EngineObject.h>
#include <brayns/engine/Light.h>
#include <brayns/engine/Material.h>
#include <brayns/engine/Renderer.h>
#include <brayns/json/Json.h>
#include <brayns/json/JsonSchemaValidator.h>
#include <brayns/utils/StringUtils.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace brayns
{
template<typename SuperT>
class EngineObjectFactory
{
private:
    struct IEngineObjectHandler
    {
        using Ptr = std::unique_ptr<IEngineObjectHandler>;

        virtual std::unique_ptr<SuperT> instantiate() const noexcept = 0;
        virtual JsonSchema getSchema() const noexcept = 0;
        virtual JsonValue serialize(const SuperT& object) const noexcept = 0;
        virtual void deserialize(const JsonValue& data, SuperT& dst) const = 0;
    };

    template<typename DerivedT>
    struct EngineObjectHandler : public IEngineObjectHandler
    {
        std::unique_ptr<SuperT> instantiate() const noexcept final
        {
            return std::make_unique<DerivedT>();
        }

        JsonSchema getSchema() const noexcept
        {
            return Json::getSchema<DerivedT>();
        }

        JsonValue serialize(const SuperT& object) const noexcept final
        {
            const auto& derived = static_cast<const DerivedT&>(object);
            return Json::serialize<DerivedT>(derived);
        }

        void deserialize(const JsonValue& data, SuperT& dst) const
        {
            const auto schema = Json::getSchema<DerivedT>();
            const auto errors = JsonSchemaValidator::validate(data, schema);
            if(!errors.empty())
            {
                throw std::invalid_argument("Could not parse parameters: " + string_utils::join(errors, ", "));
            }

            auto& derived = static_cast<DerivedT&>(dst);
            Json::deserialize(data, derived);
        }
    };

public:
    EngineObjectFactory()
    {
        if constexpr (!std::is_base_of_v<SerializableEngineObject, SuperT>)
        {
            throw std::runtime_error("Only SerializableEngineObject sub-types can be factorized");
        }
    }

    template<typename DerivedT>
    void registerType(std::string name)
    {
        // Lets fail with more legible error messages...
        // Correct type
        if constexpr (!std::is_base_of_v<SuperT, DerivedT>)
        {
            const std::string superTypeName (typeid(SuperT).name());
            throw std::invalid_argument("Cannot register a type which is not derived from " + superTypeName);
        }
        // It must be constructible and then de-serialized
        else if constexpr (!std::is_trivially_default_constructible_v<DerivedT>)
        {
            throw std::invalid_argument("Cannot register a type which is not trivially default constructible");
        }

        _registeredTypes[std::move(name)] = std::make_unique<EngineObjectHandler<DerivedT>>();
    }

    std::unique_ptr<SuperT> instantiate(const std::string& name) const
    {
        auto& entry = _findEntry(name);
        return entry->instantiate();
    }

    JsonSchema getFactorySchema() const noexcept
    {
        JsonSchema result;
        result.oneOf.reserve(_registeredTypes.size());
        for(const auto& [name, handle] : _registeredTypes)
        {
            JsonSchema schema = handle->getSchema();
            schema.title = name;
            result.oneOf.push_back(schema);
        }
        return result;
    }

    JsonValue serialize(const SuperT& object) const
    {
        // string view transparently overloaded for std less on the standard
        auto& entry = _findEntry(object.getName());
        return entry->serialize(object);
    }

    void deserialize(const JsonValue& data, SuperT& object) const
    {
        auto& entry = _findEntry(object.getName());
        entry->deserialize(data, object);
    }

private:
    template<typename KeyT>
    auto& _findEntry(const KeyT& name) const noexcept
    {
        auto it = _registeredTypes.find(name);
        if(it == _registeredTypes.end())
            throw std::invalid_argument("Cannot find a type registered as " + name);

        return it->second;
    }

private:
    std::map<std::string, typename IEngineObjectHandler::Ptr> _registeredTypes;

};

struct EngineFactories
{
    EngineObjectFactory<Camera> cameraFactory;
    EngineObjectFactory<Renderer> rendererFactory;
    EngineObjectFactory<Material> materialFactory;
    EngineObjectFactory<Light> lightFactory;
};
}
