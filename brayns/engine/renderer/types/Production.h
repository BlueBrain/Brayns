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

#include <brayns/common/MathTypes.h>
#include <brayns/engine/renderer/RendererTraits.h>

namespace brayns
{
struct Production
{
    int32_t samplesPerPixel = 5;
    int32_t maxRayBounces = 3;
    Vector4f backgroundColor = Vector4f(0.f, 0.f, 0.f, 1.f);
};

template<>
struct RendererName<Production>
{
    inline static const std::string osprayValue = "pathtracer";
    inline static const std::string value = "production";
};

template<>
class RendererData<Production>
{
public:
    static void update(ospray::cpp::Renderer &handle, Production &data);
};
}
