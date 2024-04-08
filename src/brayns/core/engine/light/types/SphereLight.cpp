/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
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

#include "SphereLight.h"

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace
{
struct SphereLightParameters
{
    static inline const std::string color = "color";
    static inline const std::string intensity = "intensity";
    static inline const std::string position = "position";
    static inline const std::string radius = "radius";
};
}

namespace brayns
{
Bounds LightTraits<SphereLight>::computeBounds(const TransformMatrix &matrix, const SphereLight &light)
{
    (void)matrix;
    (void)light;
    return {};
}

void LightTraits<SphereLight>::updateData(ospray::cpp::Light &handle, SphereLight &data)
{
    handle.setParam(SphereLightParameters::color, data.color);
    handle.setParam(SphereLightParameters::intensity, data.intensity);
    handle.setParam(SphereLightParameters::position, data.position);
    handle.setParam(SphereLightParameters::radius, data.radius);
}
}
