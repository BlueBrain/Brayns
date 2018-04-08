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

namespace brayns
{
GroupTransformation::GroupTransformation(const GroupTransformation& rhs)
{
    this->_translation = rhs._translation;
    this->_scale = rhs._scale;
    this->_rotation = rhs._rotation;
}

GroupTransformation& GroupTransformation::operator=(
    const GroupTransformation& rhs)
{
    this->_translation = rhs._translation;
    this->_scale = rhs._scale;
    this->_rotation = rhs._rotation;
    return *this;
}

GroupAttributes::GroupAttributes(const GroupAttributes& rhs)
{
    this->_name = rhs._name;
    this->_uri = rhs._uri;
    this->_enabled = rhs._enabled;
    this->_boundingBox = rhs._boundingBox;
    this->_transformations = rhs._transformations;
}

GroupAttributes& GroupAttributes::operator=(const GroupAttributes& rhs)
{
    this->_name = rhs._name;
    this->_uri = rhs._uri;
    this->_enabled = rhs._enabled;
    this->_boundingBox = rhs._boundingBox;
    this->_transformations = rhs._transformations;
    return *this;
}

GeometryGroup::GeometryGroup()
    : _materialManager(nullptr)
{
}

GeometryGroup::GeometryGroup(const GeometryGroup& rhs)
{
    this->_materialManager = rhs._materialManager;
    this->_spheres = rhs._spheres;
    this->_spheresDirty = rhs._spheresDirty;
    this->_cylinders = rhs._cylinders;
    this->_cylindersDirty = rhs._cylindersDirty;
    this->_cones = rhs._cones;
    this->_conesDirty = rhs._conesDirty;
    this->_trianglesMeshes = rhs._trianglesMeshes;
    this->_trianglesMeshesDirty = rhs._trianglesMeshesDirty;
    this->_bounds = rhs._bounds;
}

GeometryGroup::GeometryGroup(MaterialManagerPtr materialManager)
    : _materialManager(materialManager)
{
}

GeometryGroup::~GeometryGroup()
{
}

GeometryGroup& GeometryGroup::operator=(const GeometryGroup& rhs)
{
    if (this == &rhs)
        return *this;

    this->_materialManager = rhs._materialManager;
    this->_spheres = rhs._spheres;
    this->_spheresDirty = rhs._spheresDirty;
    this->_cylinders = rhs._cylinders;
    this->_cylindersDirty = rhs._cylindersDirty;
    this->_cones = rhs._cones;
    this->_conesDirty = rhs._conesDirty;
    this->_trianglesMeshes = rhs._trianglesMeshes;
    this->_trianglesMeshesDirty = rhs._trianglesMeshesDirty;
    this->_bounds = rhs._bounds;
    return *this;
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
    _materialManager->check(materialId);
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
    _materialManager->check(materialId);
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
    _materialManager->check(materialId);
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
        cones[index] = cone;
        _bounds.merge(cone.center);
        _bounds.merge(cone.up);
    }
    else
        BRAYNS_ERROR << "Invalid index " << index << std::endl;
}

bool GeometryGroup::dirty() const
{
    return _spheresDirty || _cylindersDirty || _conesDirty ||
           _trianglesMeshesDirty;
}

void GeometryGroup::logInformation()
{
    const uint64_t sizeInBytes =
        _spheres.size() * sizeof(Sphere) +
        _cylinders.size() * sizeof(Cylinder) + _cones.size() * sizeof(Cone) +
        _trianglesMeshes.size() * sizeof(TrianglesMesh);
    BRAYNS_INFO << "---------------------------------------------------"
                << std::endl;
    BRAYNS_INFO << "Geometry group information:" << std::endl;
    BRAYNS_INFO << "Spheres  : " << _spheres.size() << std::endl;
    BRAYNS_INFO << "Cylinders: " << _cylinders.size() << std::endl;
    BRAYNS_INFO << "Cones    : " << _cones.size() << std::endl;
    BRAYNS_INFO << "Meshes   : " << _trianglesMeshes.size() << std::endl;
    BRAYNS_INFO << "Total    : " << sizeInBytes << " bytes ("
                << sizeInBytes / 1048576 << " MB)" << std::endl;
}

Boxf& GeometryGroup::getBounds()
{
    return _bounds;
}
}
