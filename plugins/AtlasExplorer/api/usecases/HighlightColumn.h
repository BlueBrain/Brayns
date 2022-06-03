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

#include <brayns/json/JsonObjectMacro.h>

#include <api/IUseCase.h>
#include <api/VisualizationUseCase.h>

BRAYNS_JSON_OBJECT_BEGIN(HighlightNeighbour)
BRAYNS_JSON_OBJECT_ENTRY(brayns::Vector2ui, relative_xz, "Relative coordinates respect the main column")
BRAYNS_JSON_OBJECT_ENTRY(brayns::Vector4f, color, "Highlight color")
BRAYNS_JSON_OBJECT_END()

BRAYNS_JSON_OBJECT_BEGIN(HighlighColumParams)
BRAYNS_JSON_OBJECT_ENTRY(brayns::Vector2ui, xz_coordinate, "Coordinates of the column to highlight")
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
    inline static const VisualizationUseCase Type = VisualizationUseCase::HighlightColumn;

public:
    bool isVolumeValid(const AtlasVolume &volume) const override;
    std::unique_ptr<brayns::Model> execute(const AtlasVolume &volume, const brayns::JsonValue &payload) const override;
};
