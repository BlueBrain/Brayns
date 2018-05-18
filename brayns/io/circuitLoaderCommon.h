/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <brayns/common/geometry/Cone.h>
#include <brayns/common/geometry/Cylinder.h>
#include <brayns/common/geometry/SDFGeometry.h>
#include <brayns/common/geometry/Sphere.h>
#include <brayns/common/types.h>

namespace brayns
{
struct ParallelModelContainer
{
public:
    ParallelModelContainer(SpheresMap& s, CylindersMap& cy, ConesMap& co,
                           TrianglesMeshMap& tm, Boxf& wb, Model& m)
        : spheres(s)
        , cylinders(cy)
        , cones(co)
        , trianglesMeshes(tm)
        , bounds(wb)
        , model(m)
    {
    }

    void addSphere(const size_t materialId, const Sphere& sphere)
    {
        spheres[materialId].push_back(sphere);
        bounds.merge(sphere.center);
    }

    void addCylinder(const size_t materialId, const Cylinder& cylinder)
    {
        cylinders[materialId].push_back(cylinder);
        bounds.merge(cylinder.center);
        bounds.merge(cylinder.up);
    }

    void addCone(const size_t materialId, const Cone& cone)
    {
        cones[materialId].push_back(cone);
        bounds.merge(cone.center);
        bounds.merge(cone.up);
    }

    SpheresMap& spheres;
    CylindersMap& cylinders;
    ConesMap& cones;
    TrianglesMeshMap& trianglesMeshes;
    Boxf& bounds;
    Model& model;
};
}
