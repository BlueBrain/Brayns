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

#include "VisualizationUseCase.h"

namespace
{
class ScalarUseCaseResolver
{
public:
    static std::vector<VisualizationUseCase> resolve()
    {
        return {
            VisualizationUseCase::AREAS_WITH_SHARED_COORDINATES_BORDERS,
            VisualizationUseCase::AREAS_WITH_SHARED_COORDINATES_COLLAGE,
            VisualizationUseCase::DENSITY,
            VisualizationUseCase::LAYER_DISTANCE,
            VisualizationUseCase::XY_COLUMN,
            VisualizationUseCase::XY_COLUMN_NEIGHBOURHOOD,
            VisualizationUseCase::COLORS};
    }
};

class NDimensionalUseCaseResolver
{
public:
    static std::vector<VisualizationUseCase> resolve(size_t lastDimensionSize)
    {
        std::vector<VisualizationUseCase> result;

        if (lastDimensionSize < 4)
        {
            result.push_back(VisualizationUseCase::XY_COLUMN);
            result.push_back(VisualizationUseCase::XY_COLUMN_NEIGHBOURHOOD);
            result.push_back(VisualizationUseCase::COLORS);
        }

        if (lastDimensionSize == 4)
        {
            result.push_back(VisualizationUseCase::ORIENTATION_FIELD);
        }
    }
};

class KindUseCaseResolver
{
public:
    static std::vector<VisualizationUseCase> resolve(NRRDKind kind)
    {
        switch (kind)
        {
        case NONE:
        case DOMAIN:
        case SPACE:
        case SCALAR:
            return ScalarUseCaseResolver::resolve();
        case VECTOR:
        case VECTOR2D:
        case VECTOR3D:
        case NORMAL3D:
        case GRADIENT3:
        case RGBCOLOR:
        case HSVCOLOR:
        case XYZCOLOR:
        case RGBACOLOR:
            return NDimensionalUseCaseResolver::resolve(3);
        case QUATERNION:
            return NDimensionalUseCaseResolver::resolve(4);
        default:
            throw std::runtime_error("Unhandled kind type");
        };
    }
};
}

std::vector<VisualizationUseCase> VisualizationUseCaseQuery::availableUseCases(const NRRDHeader &header)
{
    if (header.kinds)
    {
        return KindUseCaseResolver::resolve((*header.kinds)[0]);
    }

    std::vector<VisualizationUseCase> result;

    const auto dimension = header.dimensions;
    const auto &sizes = header.sizes;

    if (dimension < 4 || sizes[0] == 1)
    {
        result = ScalarUseCaseResolver::resolve();
    }

    if (sizes[0] > 1)
    {
        auto nDimensionals = NDimensionalUseCaseResolver::resolve(sizes[0]);
        result.insert(result.end(), nDimensionals.begin(), nDimensionals.end());
    }

    return result;
}
