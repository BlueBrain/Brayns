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

#include "QuadLight.h"

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace
{
struct QuadLightParameters
{
    static inline const std::string color = "color";
    static inline const std::string intensity = "intensity";
    static inline const std::string position = "position";
    static inline const std::string edge1 = "edge1";
    static inline const std::string edge2 = "edge2";
};
}

namespace brayns
{
Bounds LightTraits<QuadLight>::computeBounds(const TransformMatrix &matrix, const QuadLight &light)
{
    Bounds bounds;
    bounds.expand(matrix.transformPoint(light.position));
    bounds.expand(matrix.transformPoint(light.position + light.edge1));
    bounds.expand(matrix.transformPoint(light.position + light.edge2));
    bounds.expand(matrix.transformPoint(light.position + light.edge1 + light.edge2));
    return bounds;
}

void LightTraits<QuadLight>::updateData(ospray::cpp::Light &handle, QuadLight &data)
{
    handle.setParam(QuadLightParameters::color, data.color);
    handle.setParam(QuadLightParameters::intensity, data.intensity);
    handle.setParam(QuadLightParameters::position, data.position);
    handle.setParam(QuadLightParameters::edge1, data.edge1);
    handle.setParam(QuadLightParameters::edge2, data.edge2);
}
}
