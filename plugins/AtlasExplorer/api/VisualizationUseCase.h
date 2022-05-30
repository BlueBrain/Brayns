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

#include <brayns/utils/EnumUtils.h>

enum class VisualizationUseCase
{
    OUTLINE_SHELL,
    ORIENTATION_FIELD,
    DENSITY,
    LAYER_DISTANCE,
    XY_COLUMN,
    XY_COLUMN_NEIGHBOURHOOD,
    AREAS_WITH_SHARED_COORDINATES_BORDERS,
    AREAS_WITH_SHARED_COORDINATES_COLLAGE,
    COLORS,
};

namespace brayns
{
template<>
inline std::vector<std::pair<std::string, VisualizationUseCase>> enumMap()
{
    return {
        {"Outline shell mesh", VisualizationUseCase::OUTLINE_SHELL},
        {"Orientation field", VisualizationUseCase::ORIENTATION_FIELD},
        {"Density", VisualizationUseCase::DENSITY},
        {"Layer distance", VisualizationUseCase::LAYER_DISTANCE},
        {"2D coordinate column", VisualizationUseCase::XY_COLUMN},
        {"2D coordinate column neighbourhood", VisualizationUseCase::XY_COLUMN_NEIGHBOURHOOD},
        {"Borders between areas with shared coordinates", VisualizationUseCase::AREAS_WITH_SHARED_COORDINATES_BORDERS},
        {"Collage of areas with shared coordinates", VisualizationUseCase::AREAS_WITH_SHARED_COORDINATES_COLLAGE},
        {"Colored voxels", VisualizationUseCase::COLORS}};
}
}
