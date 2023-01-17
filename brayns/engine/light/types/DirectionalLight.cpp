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

#include "DirectionalLight.h"

#include <brayns/engine/common/MathTypesOsprayTraits.h>

namespace
{
struct DirectionalLightParameters
{
    inline static const std::string color = "color";
    inline static const std::string intensity = "intensity";
    inline static const std::string direction = "direction";
    inline static const std::string angularDiameter = "angularDiameter";
};
}

namespace brayns
{
void LightTraits<DirectionalLight>::updateData(ospray::cpp::Light &handle, DirectionalLight &data)
{
    data.direction = glm::normalize(data.direction);
    handle.setParam(DirectionalLightParameters::color, data.color);
    handle.setParam(DirectionalLightParameters::intensity, data.intensity);
    handle.setParam(DirectionalLightParameters::direction, data.direction);
    handle.setParam(DirectionalLightParameters::angularDiameter, 0.53f);
}
}
