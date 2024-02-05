/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/common/MathTypes.h>

namespace brayns
{
struct Streamline
{
    Streamline(const std::vector<Vector3f>& positions,
               const std::vector<Vector4f>& colors,
               const std::vector<float>& radii)
        : position(positions)
        , color(colors)
        , radius(radii)
    {
    }

    // Array of vertex positions
    std::vector<Vector3f> position;

    // Array of corresponding vertex colors (RGBA)
    std::vector<Vector4f> color;

    // Array of vertex radii
    std::vector<float> radius;
};

struct StreamlinesData
{
    // Data array of all vertex position (and optional radius) for all
    // streamlines
    std::vector<Vector4f> vertex;

    // Data array of corresponding vertex colors (RGBA)
    std::vector<Vector4f> vertexColor;

    // Data array of indices to the first vertex of a link.
    //
    // A streamlines geometry can contain multiple disjoint streamlines, each
    // streamline is specified as a list of segments (or links) referenced via
    // index: each entry e of the index array points the first vertex of a link
    // (vertex[index[e]]) and the second vertex of the link is implicitly the
    // directly following one (vertex[index[e]+1]). For example, two streamlines
    // of vertices (A-B-C-D) and (E-F-G), respectively, would internally
    // correspond to five links (A-B, B-C, C-D, E-F, and F-G), and would be
    // specified via an array of vertices [A,B,C,D,E,F,G], plus an array of link
    // indices [0,1,2,4,5].
    std::vector<int32_t> indices;
};
} // namespace brayns
