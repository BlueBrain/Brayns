/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include "LightMessage.h"

namespace
{
class LightPropertiesHelper
{
public:
    template<typename T>
    static void load(const brayns::Light &baseLight, brayns::JsonValue &json)
    {
        auto &light = dynamic_cast<const T &>(baseLight);
        json = brayns::Json::serialize(light);
    }

    template<typename T>
    static brayns::JsonSchema getSchema(brayns::LightType type)
    {
        auto schema = brayns::Json::getSchema<T>();
        schema.title = brayns::GetEnumName::of(type);
        return schema;
    }
};
} // namespace

namespace brayns
{
LightProperties::LightProperties(const Light &light)
{
    switch (light._type)
    {
    case LightType::DIRECTIONAL:
        LightPropertiesHelper::load<DirectionalLight>(light, _json);
        break;
    case LightType::SPHERE:
        LightPropertiesHelper::load<SphereLight>(light, _json);
        break;
    case LightType::QUAD:
        LightPropertiesHelper::load<QuadLight>(light, _json);
        break;
    case LightType::SPOTLIGHT:
        LightPropertiesHelper::load<SpotLight>(light, _json);
        break;
    case LightType::AMBIENT:
        LightPropertiesHelper::load<AmbientLight>(light, _json);
        break;
    default:
        throw std::runtime_error("Unknown light type");
    }
}

JsonSchema LightProperties::getSchema() const
{
    JsonSchema schema;
    schema.title = "LightProperties";
    schema.oneOf = {
        LightPropertiesHelper::getSchema<DirectionalLight>(LightType::DIRECTIONAL),
        LightPropertiesHelper::getSchema<SphereLight>(LightType::SPHERE),
        LightPropertiesHelper::getSchema<QuadLight>(LightType::QUAD),
        LightPropertiesHelper::getSchema<SpotLight>(LightType::SPOTLIGHT),
        LightPropertiesHelper::getSchema<AmbientLight>(LightType::AMBIENT)};
    return schema;
}

bool LightProperties::serialize(JsonValue &json) const
{
    json = _json;
    return true;
}

bool LightProperties::deserialize(const JsonValue &json)
{
    _json = json;
    return true;
}
} // namespace brayns
