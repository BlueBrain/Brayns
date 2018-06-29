/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jonas Karlsson <jonas.karlsson@epfl.ch>
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

#include <brayns/common/types.h>

namespace brayns
{
struct Streamline
{
    Streamline(const Vector3fs& position_in, const Vector4fs& color_in,
               const std::vector<float>& radius_in)
        : position(position_in)
        , color(color_in)
        , radius(radius_in)
    {
    }

    // Array of vertex positions
    Vector3fs position;

    // Array of corresponding vertex colors (RGBA)
    Vector4fs color;

    // Array of vertex radii
    std::vector<float> radius;
};
}
