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

#include "QuadLight.h"

#include <brayns/engine/common/MathTypesOsprayTraits.h>

namespace
{
struct QuadLightParameters
{
    inline static const std::string color = "color";
    inline static const std::string intensity = "intensity";
    inline static const std::string position = "position";
    inline static const std::string edge1 = "edge1";
    inline static const std::string edge2 = "edge2";
    inline static const std::string visible = "visible";
};
}

namespace brayns
{
Bounds LightTraits<QuadLight>::computeBounds(const Matrix4f &matrix, const QuadLight &light)
{
    Bounds bounds;
    bounds.expand(matrix * Vector4f(light.position, 1.f));
    bounds.expand(matrix * Vector4f(light.position + light.edge1, 1.f));
    bounds.expand(matrix * Vector4f(light.position + light.edge2, 1.f));
    bounds.expand(matrix * Vector4f(light.position + light.edge1 + light.edge2, 1.f));
    return bounds;
}

void LightTraits<QuadLight>::updateData(ospray::cpp::Light &handle, QuadLight &data)
{
    handle.setParam(QuadLightParameters::color, data.color);
    handle.setParam(QuadLightParameters::intensity, data.intensity);
    handle.setParam(QuadLightParameters::position, data.position);
    handle.setParam(QuadLightParameters::edge1, data.edge1);
    handle.setParam(QuadLightParameters::edge2, data.edge2);
    handle.setParam(QuadLightParameters::visible, data.visible);
}
}
