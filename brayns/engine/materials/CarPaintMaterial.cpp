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

#include "CarPaintMaterial.h"

#include <brayns/engine/common/MathTypesOsprayTraits.h>

namespace
{
struct CarPaintParameters
{
    inline static const std::string osprayName = "carPaint";
    inline static const std::string baseColor = "baseColor";
    inline static const std::string flakeDensity = "flakeDensity";
};
}

namespace brayns
{
CarPaintMaterial::CarPaintMaterial()
    : Material(CarPaintParameters::osprayName)
{
}

std::string CarPaintMaterial::getName() const noexcept
{
    return "carpaint";
}

void CarPaintMaterial::setFlakesDesnity(const float flakeDensity) noexcept
{
    getModifiedFlag().update(_flakeDensity, glm::clamp(flakeDensity, 0.f, 1.f));
}

float CarPaintMaterial::getFlakesDensity() const noexcept
{
    return _flakeDensity;
}

void CarPaintMaterial::commitMaterialSpecificParams()
{
    const auto overridedColorWhite = brayns::Vector3f(1.f);

    const auto &osprayMaterial = getOsprayMaterial();
    osprayMaterial.setParam(CarPaintParameters::baseColor, overridedColorWhite);
    osprayMaterial.setParam(CarPaintParameters::flakeDensity, _flakeDensity);
}
}
