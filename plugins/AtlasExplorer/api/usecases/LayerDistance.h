/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <api/IUseCase.h>

#include <brayns/json/Json.h>

enum class LayerDistanceType
{
    lower,
    upper
};

struct LayerDistanceParameters
{
    LayerDistanceType type = LayerDistanceType::lower;
};

namespace brayns
{
template<>
struct EnumReflector<LayerDistanceType>
{
    static EnumMap<LayerDistanceType> reflect()
    {
        return {{"lower", LayerDistanceType::lower}, {"upper", LayerDistanceType::upper}};
    }
};

template<>
struct JsonAdapter<LayerDistanceType> : EnumAdapter<LayerDistanceType>
{
};

template<>
struct JsonAdapter<LayerDistanceParameters> : ObjectAdapter<LayerDistanceParameters>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("LayerDistanceParameters");
        builder
            .getset(
                "xz_coordinate",
                [](auto &object) { return object.type; },
                [](auto &object, auto value) { object.type = value; })
            .description("Type of distance to show");
        return builder.build();
    }
};
} // namespace brayns

class LayerDistance final : public IUseCase
{
public:
    std::string getName() const override;
    brayns::JsonSchema getParamsSchema() const override;
    bool isValidAtlas(const Atlas &atlas) const override;
    std::shared_ptr<brayns::Model> run(const Atlas &atlas, const brayns::JsonValue &payload) const override;

private:
    brayns::JsonSchema _paramsSchema = brayns::Json::getSchema<LayerDistanceParameters>();
};
