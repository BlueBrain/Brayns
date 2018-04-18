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

#include "Model.h"

#include <brayns/common/log.h>

namespace brayns
{
ModelTransformation::ModelTransformation(const ModelTransformation& rhs)
{
    this->_translation = rhs._translation;
    this->_scale = rhs._scale;
    this->_rotation = rhs._rotation;
}

ModelTransformation& ModelTransformation::operator=(
    const ModelTransformation& rhs)
{
    this->_translation = rhs._translation;
    this->_scale = rhs._scale;
    this->_rotation = rhs._rotation;
    return *this;
}

ModelDescriptor::ModelDescriptor(const ModelDescriptor& rhs)
{
    this->_name = rhs._name;
    this->_uri = rhs._uri;
    this->_enabled = rhs._enabled;
    this->_visible = rhs._visible;
    this->_boundingBox = rhs._boundingBox;
    this->_transformations = rhs._transformations;
}

ModelDescriptor& ModelDescriptor::operator=(const ModelDescriptor& rhs)
{
    this->_name = rhs._name;
    this->_uri = rhs._uri;
    this->_enabled = rhs._enabled;
    this->_visible = rhs._visible;
    this->_boundingBox = rhs._boundingBox;
    this->_transformations = rhs._transformations;
    return *this;
}

Model::Model(const std::string& name, MaterialManager& materialManager)
    : _materialManager(materialManager)
    , _name(name)
{
    _bounds.reset();
}

Model::~Model()
{
}

void Model::unload()
{
    _spheres.clear();
    _spheresDirty = true;
    _cylinders.clear();
    _cylindersDirty = true;
    _cones.clear();
    _conesDirty = true;
    _trianglesMeshes.clear();
    _trianglesMeshesDirty = true;
    _bounds.reset();
}

bool Model::empty() const
{
    return _spheres.empty() && _cylinders.empty() && _cones.empty() &&
           _trianglesMeshes.empty();
}

uint64_t Model::addSphere(const size_t materialId, const Sphere& sphere)
{
    _spheresDirty = true;
    _materialManager.check(materialId);
    _spheres[materialId].push_back(sphere);
    _bounds.merge(sphere.center);
    return _spheres[materialId].size() - 1;
}

void Model::setSphere(const size_t materialId, const uint64_t index,
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

uint64_t Model::addCylinder(const size_t materialId, const Cylinder& cylinder)
{
    _cylindersDirty = true;
    _materialManager.check(materialId);
    _cylinders[materialId].push_back(cylinder);
    _bounds.merge(cylinder.center);
    _bounds.merge(cylinder.up);
    return _cylinders[materialId].size() - 1;
}

void Model::setCylinder(const size_t materialId, const uint64_t index,
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

uint64_t Model::addCone(const size_t materialId, const Cone& cone)
{
    _conesDirty = true;
    _materialManager.check(materialId);
    _cones[materialId].push_back(cone);
    _bounds.merge(cone.center);
    _bounds.merge(cone.up);
    return _cones[materialId].size() - 1;
}

void Model::setCone(const size_t materialId, const uint64_t index,
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

bool Model::dirty() const
{
    return _spheresDirty || _cylindersDirty || _conesDirty ||
           _trianglesMeshesDirty;
}

void Model::logInformation()
{
    uint64_t sizeInBytes = 0;
    uint64_t nbSpheres = 0;
    uint64_t nbCylinders = 0;
    uint64_t nbCones = 0;
    uint64_t nbMeshes = 0;
    for (const auto& spheres : _spheres)
    {
        sizeInBytes += spheres.second.size() * sizeof(Sphere);
        nbSpheres += spheres.second.size();
    }
    for (const auto& cylinders : _cylinders)
    {
        sizeInBytes += cylinders.second.size() * sizeof(Cylinder);
        nbCylinders += cylinders.second.size();
    }
    for (const auto& cones : _cones)
    {
        sizeInBytes += cones.second.size() * sizeof(Cones);
        nbCones += cones.second.size();
    }
    for (const auto& trianglesMesh : _trianglesMeshes)
    {
        const auto& mesh = trianglesMesh.second;
        sizeInBytes += mesh.indices.size() * sizeof(Vector3f);
        sizeInBytes += mesh.normals.size() * sizeof(Vector3f);
        sizeInBytes += mesh.colors.size() * sizeof(Vector4f);
        sizeInBytes += mesh.indices.size() * sizeof(Vector3ui);
        sizeInBytes += mesh.textureCoordinates.size() * sizeof(Vector2f);
        ++nbMeshes;
    }

    BRAYNS_DEBUG << "Model " << _name << std::endl;
    BRAYNS_DEBUG << "- Spheres  : " << nbSpheres << std::endl;
    BRAYNS_DEBUG << "- Cylinders: " << nbCylinders << std::endl;
    BRAYNS_DEBUG << "- Cones    : " << nbCones << std::endl;
    BRAYNS_DEBUG << "- Meshes   : " << nbMeshes << std::endl;
    BRAYNS_DEBUG << "- Total    : " << sizeInBytes << " bytes ("
                 << sizeInBytes / 1048576 << " MB)" << std::endl;
}

Boxf& Model::getBounds()
{
    return _bounds;
}
}
