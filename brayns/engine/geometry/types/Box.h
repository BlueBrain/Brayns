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
#include <brayns/engine/geometry/Geometry.h>

#include <ospray/ospray_cpp/Traits.h>

namespace brayns
{
struct Box
{
    Vector3f min;
    Vector3f max;
};

template<>
class OsprayGeometryName<Box>
{
public:
    static const std::string &get();
};

template<>
class GeometryBoundsUpdater<Box>
{
public:
    static void update(const Box &s, const Matrix4f &t, Bounds &b);
};

template<>
class GeometryCommitter<Box>
{
public:
    static void commit(const ospray::cpp::Geometry &osparyGeometry, const std::vector<Box> &primitives);
};

} // namespace brayns
namespace ospray
{
OSPTYPEFOR_SPECIALIZATION(brayns::Box, OSP_BOX3F)
} // namespace ospray
