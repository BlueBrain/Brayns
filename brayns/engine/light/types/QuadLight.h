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

#include <brayns/engine/light/LightTraits.h>

namespace brayns
{
struct QuadLight
{
    float intensity = 1.f;
    Vector3f color = Vector3f(1.f);
    Vector3f position = Vector3f(0.f);
    Vector3f edge1 = Vector3f(1.f, 0.f, 0.f);
    Vector3f edge2 = Vector3f(0.f, 0.f, 1.f);
};

template<>
class LightTraits<QuadLight>
{
public:
    inline static const std::string handleName = "quad";
    inline static const std::string name = "quad";

    static Bounds computeBounds(const Matrix4f &matrix, const QuadLight &light);

    static void updateData(ospray::cpp::Light &handle, QuadLight &data);
};
}
