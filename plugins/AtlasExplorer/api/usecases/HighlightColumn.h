/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <brayns/json/JsonObjectMacro.h>

#include <api/IUseCase.h>

BRAYNS_JSON_OBJECT_BEGIN(HighlightNeighbour)
BRAYNS_JSON_OBJECT_ENTRY(brayns::Vector2i, relative_xz, "Relative coordinates respect the main column")
BRAYNS_JSON_OBJECT_ENTRY(brayns::Vector4f, color, "Highlight color")
BRAYNS_JSON_OBJECT_END()

BRAYNS_JSON_OBJECT_BEGIN(HighlightColumParams)
BRAYNS_JSON_OBJECT_ENTRY(brayns::Vector2i, xz_coordinate, "Coordinates of the column to highlight")
BRAYNS_JSON_OBJECT_ENTRY(brayns::Vector4f, color, "Highlight  color")
BRAYNS_JSON_OBJECT_ENTRY(
    std::vector<HighlightNeighbour>,
    neighbours,
    "Optional neighbours to highlight",
    brayns::Required(false))
BRAYNS_JSON_OBJECT_END()

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
