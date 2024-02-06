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

#include <brayns/json/Json.h>

#include <api/IUseCase.h>

struct HighlightNeighbour
{
    brayns::Vector2i relative_xz{0};
    brayns::Vector4f color{0};
};

struct HighlightColumParams
{
    brayns::Vector2i xz_coordinate{0};
    brayns::Vector4f color{0};
    std::vector<HighlightNeighbour> neighbours;
};

namespace brayns
{
template<>
struct JsonAdapter<HighlightNeighbour> : ObjectAdapter<HighlightNeighbour>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("HighlightNeighbour");
        builder
            .getset(
                "relative_xz",
                [](auto &object) -> auto & { return object.relative_xz; },
                [](auto &object, const auto &value) { object.relative_xz = value; })
            .description("Relative coordinates respect the main column");
        builder
            .getset(
                "color",
                [](auto &object) -> auto & { return object.color; },
                [](auto &object, const auto &value) { object.color = value; })
            .description("Highlight color");
        return builder.build();
    }
};

template<>
struct JsonAdapter<HighlightColumParams> : ObjectAdapter<HighlightColumParams>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("HighlightColumParams");
        builder
            .getset(
                "xz_coordinate",
                [](auto &object) -> auto & { return object.xz_coordinate; },
                [](auto &object, const auto &value) { object.xz_coordinate = value; })
            .description("Coordinates of the column to highlight");
        builder
            .getset(
                "color",
                [](auto &object) -> auto & { return object.color; },
                [](auto &object, const auto &value) { object.color = value; })
            .description("Highlight color");
        builder
            .getset(
                "neighbours",
                [](auto &object) -> auto & { return object.neighbours; },
                [](auto &object, auto value) { object.neighbours = std::move(value); })
            .description("Optional neighbours to highlight")
            .required(false);
        return builder.build();
    }
};
} // namespace brayns

class HighlightColumn final : public IUseCase
{
public:
    std::string getName() const override;
    brayns::JsonSchema getParamsSchema() const override;
    bool isValidAtlas(const Atlas &atlas) const override;
    std::shared_ptr<brayns::Model> run(const Atlas &atlas, const brayns::JsonValue &payload) const override;

private:
    brayns::JsonSchema _paramsSchema = brayns::Json::getSchema<HighlightColumParams>();
};
