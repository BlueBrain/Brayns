/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/engine/Light.h>
#include <brayns/json/JsonObjectMacro.h>
#include <brayns/network/adapters/LightAdapter.h>

namespace brayns
{
class LightProperties
{
public:
    LightProperties() = default;

    LightProperties(const Light& light)
    {
        switch (light._type)
        {
        case LightType::DIRECTIONAL:
            _load<DirectionalLight>(light);
            break;
        case LightType::SPHERE:
            _load<SphereLight>(light);
            break;
        case LightType::QUAD:
            _load<QuadLight>(light);
            break;
        case LightType::SPOTLIGHT:
            _load<SpotLight>(light);
            break;
        case LightType::AMBIENT:
            _load<AmbientLight>(light);
            break;
        default:
            throw std::runtime_error("Unknown light type");
        }
    }

    JsonSchema getSchema() const
    {
        JsonSchema schema;
        schema.title = "LightProperties";
        schema.oneOf = {_getSchema<DirectionalLight>(LightType::DIRECTIONAL),
                        _getSchema<SphereLight>(LightType::SPHERE),
                        _getSchema<QuadLight>(LightType::QUAD),
                        _getSchema<SpotLight>(LightType::SPOTLIGHT),
                        _getSchema<AmbientLight>(LightType::AMBIENT)};
        return schema;
    }

    bool serialize(JsonValue& json) const
    {
        json = _json;
        return true;
    }

    bool deserialize(const JsonValue& json)
    {
        _json = json;
        return true;
    }

private:
    template <typename T>
    void _load(const Light& baseLight)
    {
        auto& light = dynamic_cast<const T&>(baseLight);
        _json = Json::serialize(light);
    }

    template <typename T>
    JsonSchema _getSchema(LightType type) const
    {
        auto schema = Json::getSchema<T>();
        schema.title = GetEnumName::of(type);
        return schema;
    }

    JsonValue _json;
};

BRAYNS_JSON_OBJECT_BEGIN(LightMessage)
BRAYNS_JSON_OBJECT_ENTRY(LightType, type, "Light type")
BRAYNS_JSON_OBJECT_ENTRY(size_t, id, "Light ID")
BRAYNS_JSON_OBJECT_ENTRY(LightProperties, properties, "Light properties")
BRAYNS_JSON_OBJECT_END()
} // namespace brayns
