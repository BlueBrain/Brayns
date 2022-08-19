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

#include <brayns/engine/light/LightTraits.h>

namespace brayns
{
struct AmbientLight
{
    Vector3f color = Vector3f(1.f);
    float intensity = 0.1;
};

template<>
struct LightName<AmbientLight>
{
    inline static const std::string osprayValue = "ambient";
    inline static const std::string value = "ambient";
};

template<>
class LightData<AmbientLight>
{
public:
    static void update(ospray::cpp::Light &handle, AmbientLight &lightData);
};
}
