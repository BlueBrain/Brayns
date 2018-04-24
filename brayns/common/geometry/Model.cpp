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
#include <brayns/common/material/Material.h>
#include <brayns/common/material/Texture2D.h>

namespace brayns
{
ModelDescriptor::~ModelDescriptor()
{
    if (_model)
    {
        _model->unload();
        _model.reset();
    }
}

Model::Model()
{
    _bounds.reset();
}

Model::~Model()
{
    unload();
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

void Model::setMaterialsColorMap(const MaterialsColorMap colorMap)
{
    size_t index = 0;
    for (auto material : _materials)
    {
        material.second->setSpecularColor(Vector3f(0.f));
        material.second->setOpacity(1.f);
        material.second->setReflectionIndex(0.f);

        switch (colorMap)
        {
        case MaterialsColorMap::none:
            switch (index)
            {
            case 0: // Default
            case 1: // Soma
                material.second->setDiffuseColor(Vector3f(0.9f, 0.9f, 0.9f));
                break;
            case 2: // Axon
                material.second->setDiffuseColor(Vector3f(0.2f, 0.2f, 0.8f));
                break;
            case 3: // Dendrite
                material.second->setDiffuseColor(Vector3f(0.8f, 0.2f, 0.2f));
                break;
            case 4: // Apical dendrite
                material.second->setDiffuseColor(Vector3f(0.8f, 0.2f, 0.8f));
                break;
            default:
                material.second->setDiffuseColor(
                    Vector3f(float(std::rand() % 255) / 255.f,
                             float(std::rand() % 255) / 255.f,
                             float(std::rand() % 255) / 255.f));
            }
            break;
        case MaterialsColorMap::gradient:
        {
            const float a = float(index) / float(_materials.size());
            material.second->setDiffuseColor(Vector3f(a, 0.f, 1.f - a));
            break;
        }
        case MaterialsColorMap::pastel:
            material.second->setDiffuseColor(
                Vector3f(0.5f + float(std::rand() % 127) / 255.f,
                         0.5f + float(std::rand() % 127) / 255.f,
                         0.5f + float(std::rand() % 127) / 255.f));
            break;
        case MaterialsColorMap::random:
            material.second->setDiffuseColor(
                Vector3f(float(rand() % 255) / 255.f,
                         float(rand() % 255) / 255.f,
                         float(rand() % 255) / 255.f));
            switch (rand() % 4)
            {
            case 0:
                // Transparent
                material.second->setOpacity(float(std::rand() % 100) / 100.f);
                material.second->setRefractionIndex(0.98f);
                material.second->setSpecularColor(
                    Vector3f(0.01f, 0.01f, 0.01f));
                material.second->setSpecularExponent(10.f);
            case 1:
                // Light emmitter
                material.second->setEmission(1.f);
            case 2:
                // Reflector
                material.second->setReflectionIndex(float(std::rand() % 100) /
                                                    100.f);
                material.second->setSpecularColor(
                    Vector3f(0.01f, 0.01f, 0.01f));
                material.second->setSpecularExponent(10.f);
            }
            break;
        case MaterialsColorMap::shades_of_grey:
            float value = float(std::rand() % 255) / 255.f;
            material.second->setDiffuseColor(Vector3f(value, value, value));
            break;
        }
        material.second->commit();
        ++index;
    }
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

    BRAYNS_DEBUG << "Model " << std::endl;
    BRAYNS_DEBUG << "- Spheres  : " << nbSpheres << std::endl;
    BRAYNS_DEBUG << "- Cylinders: " << nbCylinders << std::endl;
    BRAYNS_DEBUG << "- Cones    : " << nbCones << std::endl;
    BRAYNS_DEBUG << "- Meshes   : " << nbMeshes << std::endl;
    BRAYNS_DEBUG << "- Total    : " << sizeInBytes << " bytes ("
                 << sizeInBytes / 1048576 << " MB)" << std::endl;
}

MaterialPtr Model::getMaterial(const size_t materialId)
{
    if (_materials.find(materialId) == _materials.end())
        return nullptr;
    return _materials[materialId];
}
}
