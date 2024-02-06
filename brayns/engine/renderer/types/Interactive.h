/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/engine/renderer/RendererTraits.h>
#include <brayns/utils/MathTypes.h>

namespace brayns
{
struct Interactive
{
    size_t samplesPerPixel = 1;
    size_t maxRayBounces = 3;
    Vector4f backgroundColor = Vector4f(.004f, .016f, .102f, 0.f);
    bool shadowsEnabled = true;
    size_t aoSamples = 0;
};

template<>
class RendererTraits<Interactive>
{
public:
    static inline const std::string handleName = "scivis";
    static inline const std::string name = "interactive";

    static void checkParameters(const Interactive &data);
    static void updateData(ospray::cpp::Renderer &handle, Interactive &data);
};
}
