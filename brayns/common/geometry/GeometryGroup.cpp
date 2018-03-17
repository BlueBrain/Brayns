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

#include "GeometryGroup.h"

#include <brayns/common/log.h>
#include <brayns/common/material/MaterialManager.h>

namespace brayns
{
GeometryGroup::GeometryGroup(MaterialManager& materialManager)
    : _materialManager(materialManager)
{
}

GeometryGroup::~GeometryGroup()
{
}

void GeometryGroup::unload()
{
    _spheres.clear();
    _spheresDirty = true;
    _cylinders.clear();
    _cylindersDirty = true;
    _cones.clear();
    _conesDirty = true;
    _trianglesMeshes.clear();
    _trianglesMeshesDirty = true;
}

bool GeometryGroup::empty() const
{
    return _spheres.empty() && _cylinders.empty() && _cones.empty() &&
           _trianglesMeshes.empty();
}

uint64_t GeometryGroup::addSphere(const size_t materialId, const Sphere& sphere)
{
    _spheresDirty = true;
    _materialManager.buildMissingMaterials(materialId);
    _spheres[materialId].push_back(sphere);
    _bounds.merge(sphere.center);
    return _spheres[materialId].size() - 1;
}

void GeometryGroup::setSphere(const size_t materialId, const uint64_t index,
                              const Sphere& sphere)
{
    _spheresDirty = true;
    auto& spheres = _spheres[materialId];
    if (index < spheres.size())
    {
        _materialManager.buildMissingMaterials(materialId);
        spheres[index] = sphere;
        _bounds.merge(sphere.center);
    }
    else
        BRAYNS_ERROR << "Invalid index " << index << std::endl;
}

uint64_t GeometryGroup::addCylinder(const size_t materialId,
                                    const Cylinder& cylinder)
{
    _cylindersDirty = true;
    _materialManager.buildMissingMaterials(materialId);
    _cylinders[materialId].push_back(cylinder);
    _bounds.merge(cylinder.center);
    _bounds.merge(cylinder.up);
    return _cylinders[materialId].size() - 1;
}

void GeometryGroup::setCylinder(const size_t materialId, const uint64_t index,
                                const Cylinder& cylinder)
{
    _cylindersDirty = true;
    auto& cylinders = _cylinders[materialId];
    if (index < cylinders.size())
    {
        _materialManager.buildMissingMaterials(materialId);
        cylinders[index] = cylinder;
        _bounds.merge(cylinder.center);
        _bounds.merge(cylinder.up);
    }
    else
        BRAYNS_ERROR << "Invalid index " << index << std::endl;
}

uint64_t GeometryGroup::addCone(const size_t materialId, const Cone& cone)
{
    _conesDirty = true;
    _materialManager.buildMissingMaterials(materialId);
    _cones[materialId].push_back(cone);
    _bounds.merge(cone.center);
    _bounds.merge(cone.up);
    return _cones[materialId].size() - 1;
}

void GeometryGroup::setCone(const size_t materialId, const uint64_t index,
                            const Cone& cone)
{
    _conesDirty = true;
    auto& cones = _cones[materialId];
    if (index < cones.size())
    {
        _materialManager.buildMissingMaterials(materialId);
        cones[index] = cone;
        _bounds.merge(cone.center);
        _bounds.merge(cone.up);
    }
    else
        BRAYNS_ERROR << "Invalid index " << index << std::endl;
}
}
