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
#include <brayns/common/geometry/Sphere.h>

namespace brayns
{
struct ParallelSceneContainer
{
public:
    ParallelSceneContainer(SpheresMap& s, CylindersMap& cy, ConesMap& co,
                           TrianglesMeshMap& tm, Materials& m, Boxf& wb)
        : spheres(s)
        , cylinders(cy)
        , cones(co)
        , trianglesMeshes(tm)
        , materials(m)
        , worldBounds(wb)
    {
    }

    void _buildMissingMaterials(const size_t materialId)
    {
        if (materialId >= materials.size())
            materials.resize(materialId + 1);
    }

    void addSphere(const size_t materialId, const Sphere& sphere)
    {
        _buildMissingMaterials(materialId);
        spheres[materialId].push_back(sphere);
        worldBounds.merge(sphere.center);
    }

    void addCylinder(const size_t materialId, const Cylinder& cylinder)
    {
        _buildMissingMaterials(materialId);
        cylinders[materialId].push_back(cylinder);
        worldBounds.merge(cylinder.center);
        worldBounds.merge(cylinder.up);
    }

    void addCone(const size_t materialId, const Cone& cone)
    {
        _buildMissingMaterials(materialId);
        cones[materialId].push_back(cone);
        worldBounds.merge(cone.center);
        worldBounds.merge(cone.up);
    }

    SpheresMap& spheres;
    CylindersMap& cylinders;
    ConesMap& cones;
    TrianglesMeshMap& trianglesMeshes;
    Materials& materials;
    Boxf& worldBounds;
};
}
