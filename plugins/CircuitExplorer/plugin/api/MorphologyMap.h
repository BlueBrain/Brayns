/* Copyright (c) 2020, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Román Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/BlueBrain/Brayns>
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

#include <brayns/common/geometry/Cone.h>
#include <brayns/common/geometry/Cylinder.h>
#include <brayns/common/geometry/SDFBezier.h>
#include <brayns/common/geometry/SDFGeometry.h>
#include <brayns/common/geometry/Sphere.h>
#include <brayns/common/geometry/TriangleMesh.h>
#include <brayns/common/types.h>

#include <unordered_map>

struct MorphologyMap
{
    // Index generated when loading and processing the morphology
    size_t _linealIndex;

    // material Id -> list of spheres for this morhpology
    std::unordered_map<size_t, std::vector<size_t>> _sphereMap;

    // material Id -> list of cylinder for this morpholgy
    std::unordered_map<size_t, std::vector<size_t>> _cylinderMap;

    // material Id -> list of cones for this morphology
    std::unordered_map<size_t, std::vector<size_t>> _coneMap;

    // material Id -> list of bezier sdfs for this morhology
    std::unordered_map<size_t, std::vector<size_t>> _sdfBezierMap;

    // material Id -> list of triangle meshes for this morphology
    size_t _triangleIndx;
    bool _hasMesh{false};

    // material Id -> list of geometry sdfs for this morphology
    std::unordered_map<size_t, std::vector<size_t>> _sdfGeometryMap;
};

