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

#include <brayns/engine/geometry/GeometryTraits.h>

#include <ospray/ospray_cpp/Traits.h>

namespace brayns
{
struct Box
{
    Vector3f min;
    Vector3f max;
};

template<>
class GeometryTraits<Box>
{
public:
    static inline const std::string handleName = "box";
    static inline const std::string name = "box";

    static Bounds computeBounds(const TransformMatrix &matrix, const Box &data);
    static void updateData(ospray::cpp::Geometry &handle, std::vector<Box> &data);
};

} // namespace brayns

namespace ospray
{
OSPTYPEFOR_SPECIALIZATION(brayns::Box, OSP_BOX3F)
} // namespace ospray
