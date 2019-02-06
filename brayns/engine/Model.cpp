/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#include <brayns/common/Transformation.h>
#include <brayns/common/log.h>
#include <brayns/common/material/Texture2D.h>
#include <brayns/engine/Material.h>
#include <brayns/engine/Volume.h>

#include <brayns/common/simulation/AbstractSimulationHandler.h>

#include <boost/filesystem.hpp>

#include <set>

namespace brayns
{
namespace
{
void _bindMaterials(const AbstractSimulationHandlerPtr& simulationHandler,
                    MaterialMap& materials)
{
    if (!simulationHandler)
        return;
    for (const auto& material : materials)
        simulationHandler->bind(material.second);
}

void _unbindMaterials(const AbstractSimulationHandlerPtr& simulationHandler,
                      MaterialMap& materials)
{
    if (!simulationHandler)
        return;
    for (const auto& material : materials)
        simulationHandler->unbind(material.second);
}
}
ModelParams::ModelParams(const std::string& path)
    : _name(boost::filesystem::basename(path))
    , _path(path)
{
}

ModelParams::ModelParams(const std::string& name, const std::string& path)
    : _name(name)
    , _path(path)
{
}

ModelParams::ModelParams(const std::string& name, const std::string& path,
                         const PropertyMap& loaderProperties)
    : _name(name)
    , _path(path)
    , _loaderProperties(loaderProperties)
{
}

ModelDescriptor::ModelDescriptor(ModelPtr model, const std::string& path)
    : ModelParams(path)
    , _model(std::move(model))
{
}

ModelDescriptor::ModelDescriptor(ModelPtr model, const std::string& path,
                                 const ModelMetadata& metadata)
    : ModelParams(path)
    , _metadata(metadata)
    , _model(std::move(model))
{
}

ModelDescriptor::ModelDescriptor(ModelPtr model, const std::string& name,
                                 const std::string& path,
                                 const ModelMetadata& metadata)
    : ModelParams(name, path)
    , _metadata(metadata)
    , _model(std::move(model))
{
}

ModelDescriptor& ModelDescriptor::operator=(const ModelParams& rhs)
{
    if (this == &rhs)
        return *this;
    _updateValue(_boundingBox, rhs.getBoundingBox());
    if (rhs.getName().empty())
        _updateValue(_name, boost::filesystem::basename(rhs.getPath()));
    else
        _updateValue(_name, rhs.getName());
    _updateValue(_path, rhs.getPath());
    _updateValue(_visible, rhs.getVisible());

    // Transformation
    const auto oldRotationCenter = _transformation.getRotationCenter();
    const auto newRotationCenter = rhs.getTransformation().getRotationCenter();
    _updateValue(_transformation, rhs.getTransformation());
    if (newRotationCenter == Vector3f())
        // If no rotation center is specified in the model params, the one set
        // by the model loader is used
        _transformation.setRotationCenter(oldRotationCenter);

    return *this;
}

void ModelDescriptor::addInstance(const ModelInstance& instance)
{
    _instances.push_back(instance);
    _instances.rbegin()->setInstanceID(_nextInstanceID++);
    if (_model)
        _model->markInstancesDirty();
}

void ModelDescriptor::removeInstance(const size_t id)
{
    auto i = std::remove_if(_instances.begin(), _instances.end(),
                            [id](const auto& instance) {
                                return id == instance.getInstanceID();
                            });
    if (i == _instances.end())
        return;

    _instances.erase(i, _instances.end());

    if (_model)
        _model->markInstancesDirty();
}

ModelInstance* ModelDescriptor::getInstance(const size_t id)
{
    auto i = std::find_if(_instances.begin(), _instances.end(),
                          [id](const auto& instance) {
                              return id == instance.getInstanceID();
                          });
    return i == _instances.end() ? nullptr : &(*i);
}

void ModelDescriptor::computeBounds()
{
    _bounds.reset();
    for (const auto& instance : getInstances())
    {
        if (!instance.getVisible() || !_model)
            continue;

        _bounds.merge(
            transformBox(getModel().getBounds(),
                         getTransformation() * instance.getTransformation()));
    }
}

ModelDescriptorPtr ModelDescriptor::clone(ModelPtr model) const
{
    auto newModelDesc =
        std::make_shared<ModelDescriptor>(std::move(model), getPath());

    *newModelDesc = static_cast<const ModelParams&>(*this);

    newModelDesc->_bounds = _bounds;
    newModelDesc->_metadata = _metadata;
    newModelDesc->_model->copyFrom(getModel());
    newModelDesc->_instances = _instances;
    newModelDesc->_properties = _properties;
    newModelDesc->_model->buildBoundingBox();
    return newModelDesc;
}

Model::Model(AnimationParameters& animationParameters,
             VolumeParameters& volumeParameters)
    : _animationParameters(animationParameters)
    , _volumeParameters(volumeParameters)
{
}

bool Model::empty() const
{
    return _geometries->isEmpty() && _bounds.isEmpty();
}

uint64_t Model::addSphere(const size_t materialId, const Sphere& sphere)
{
    _spheresDirty = true;
    _geometries->_spheres[materialId].push_back(sphere);
    return _geometries->_spheres[materialId].size() - 1;
}

uint64_t Model::addCylinder(const size_t materialId, const Cylinder& cylinder)
{
    _cylindersDirty = true;
    _geometries->_cylinders[materialId].push_back(cylinder);
    return _geometries->_cylinders[materialId].size() - 1;
}

uint64_t Model::addCone(const size_t materialId, const Cone& cone)
{
    _conesDirty = true;
    _geometries->_cones[materialId].push_back(cone);
    return _geometries->_cones[materialId].size() - 1;
}

void Model::addStreamline(const size_t materialId, const Streamline& streamline)
{
    if (streamline.position.size() < 2)
        throw std::runtime_error(
            "Number of vertices is less than two which is minimum needed for a "
            "streamline.");

    if (streamline.position.size() != streamline.color.size())
        throw std::runtime_error("Number of vertices and colors do not match.");

    if (streamline.position.size() != streamline.radius.size())
        throw std::runtime_error("Number of vertices and radii do not match.");

    auto& streamlinesData = _geometries->_streamlines[materialId];

    const size_t startIndex = streamlinesData.vertex.size();
    const size_t endIndex = startIndex + streamline.position.size() - 1;

    for (size_t index = startIndex; index < endIndex; ++index)
        streamlinesData.indices.push_back(index);

    for (size_t i = 0; i < streamline.position.size(); i++)
        streamlinesData.vertex.push_back(
            Vector4f(streamline.position[i], streamline.radius[i]));

    for (const auto& color : streamline.color)
        streamlinesData.vertexColor.push_back(color);

    _streamlinesDirty = true;
}

uint64_t Model::addSDFGeometry(const size_t materialId, const SDFGeometry& geom,
                               const std::vector<size_t>& neighbourIndices)
{
    const uint64_t geomIdx = _geometries->_sdf.geometries.size();
    _geometries->_sdf.geometryIndices[materialId].push_back(geomIdx);
    _geometries->_sdf.neighbours.push_back(neighbourIndices);
    _geometries->_sdf.geometries.push_back(geom);
    _sdfGeometriesDirty = true;
    return geomIdx;
}

void Model::updateSDFGeometryNeighbours(
    size_t geometryIdx, const std::vector<size_t>& neighbourIndices)
{
    _geometries->_sdf.neighbours[geometryIdx] = neighbourIndices;
    _sdfGeometriesDirty = true;
}

void Model::addVolume(VolumePtr volume)
{
    _geometries->_volumes.push_back(volume);
    _volumesDirty = true;
}

void Model::removeVolume(VolumePtr volume)
{
    auto i = std::find(_geometries->_volumes.begin(),
                       _geometries->_volumes.end(), volume);
    if (i == _geometries->_volumes.end())
        return;

    _geometries->_volumes.erase(i);
    _volumesDirty = true;
}

bool Model::isDirty() const
{
    return _areGeomtriesDirty() || _instancesDirty;
}

void Model::setMaterialsColorMap(const MaterialsColorMap colorMap)
{
    size_t index = 0;
    for (auto material : _materials)
    {
        material.second->setSpecularColor(Vector3f(0.f));
        material.second->setOpacity(1.f);
        material.second->setReflectionIndex(0.f);
        material.second->setEmission(0.f);

        switch (colorMap)
        {
        case MaterialsColorMap::gradient:
        {
            const float a = float(index) / float(_materials.size() - 1);
            material.second->setDiffuseColor(
                Vector3f(a * a, std::sqrt(a), 1.f - a));
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
            switch (rand() % 10)
            {
            case 0:
                // Transparency only
                material.second->setOpacity(float(std::rand() % 100) / 100.f);
                material.second->setRefractionIndex(1.2f);
                material.second->setSpecularColor(Vector3f(1.f));
                material.second->setSpecularExponent(10.f);
                break;
            case 1:
                // Light emission
                material.second->setEmission(std::rand() % 20);
                break;
            case 2:
                // Reflection only
                material.second->setReflectionIndex(float(std::rand() % 100) /
                                                    100.f);
                material.second->setSpecularColor(Vector3f(1.f));
                material.second->setSpecularExponent(10.f);
                break;
            case 3:
                // Reflection and refraction
                material.second->setReflectionIndex(float(std::rand() % 100) /
                                                    100.f);
                material.second->setOpacity(float(std::rand() % 100) / 100.f);
                material.second->setRefractionIndex(1.2f);
                material.second->setSpecularColor(Vector3f(1.f));
                material.second->setSpecularExponent(10.f);
                break;
            case 4:
                // Reflection and glossiness
                material.second->setReflectionIndex(float(std::rand() % 100) /
                                                    100.f);
                material.second->setSpecularColor(Vector3f(1.f));
                material.second->setSpecularExponent(10.f);
                material.second->setGlossiness(float(std::rand() % 100) /
                                               100.f);
                break;
            case 5:
                // Transparency and glossiness
                material.second->setOpacity(float(std::rand() % 100) / 100.f);
                material.second->setRefractionIndex(1.2f);
                material.second->setSpecularColor(Vector3f(1.f));
                material.second->setSpecularExponent(10.f);
                material.second->setGlossiness(float(std::rand() % 100) /
                                               100.f);
                break;
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
    _updateSizeInBytes();

    uint64_t nbSpheres = 0;
    uint64_t nbCylinders = 0;
    uint64_t nbCones = 0;
    uint64_t nbMeshes = _geometries->_trianglesMeshes.size();
    for (const auto& spheres : _geometries->_spheres)
        nbSpheres += spheres.second.size();
    for (const auto& cylinders : _geometries->_cylinders)
        nbCylinders += cylinders.second.size();
    for (const auto& cones : _geometries->_cones)
        nbCones += cones.second.size();

    BRAYNS_DEBUG << "Spheres: " << nbSpheres << ", Cylinders: " << nbCylinders
                 << ", Cones: " << nbCones << ", Meshes: " << nbMeshes
                 << ", Memory: " << _sizeInBytes << " bytes ("
                 << _sizeInBytes / 1048576 << " MB), Bounds: " << _bounds
                 << std::endl;
}

MaterialPtr Model::getMaterial(const size_t materialId) const
{
    const auto it = _materials.find(materialId);
    if (it == _materials.end())
        throw std::runtime_error("Material " + std::to_string(materialId) +
                                 " is not registered in the model");
    return it->second;
}

void Model::_updateSizeInBytes()
{
    _sizeInBytes = 0;
    for (const auto& spheres : _geometries->_spheres)
        _sizeInBytes += spheres.second.size() * sizeof(Sphere);
    for (const auto& cylinders : _geometries->_cylinders)
        _sizeInBytes += cylinders.second.size() * sizeof(Cylinder);
    for (const auto& cones : _geometries->_cones)
        _sizeInBytes += cones.second.size() * sizeof(Cones);
    for (const auto& trianglesMesh : _geometries->_trianglesMeshes)
    {
        const auto& mesh = trianglesMesh.second;
        _sizeInBytes += mesh.indices.size() * sizeof(Vector3f);
        _sizeInBytes += mesh.normals.size() * sizeof(Vector3f);
        _sizeInBytes += mesh.colors.size() * sizeof(Vector4f);
        _sizeInBytes += mesh.indices.size() * sizeof(Vector3ui);
        _sizeInBytes += mesh.textureCoordinates.size() * sizeof(Vector2f);
    }
    for (const auto& streamline : _geometries->_streamlines)
    {
        _sizeInBytes += streamline.second.indices.size() * sizeof(int32_t);
        _sizeInBytes += streamline.second.vertex.size() * sizeof(Vector4f);
        _sizeInBytes += streamline.second.vertexColor.size() * sizeof(Vector4f);
    }

    _sizeInBytes += _geometries->_sdf.geometries.size() * sizeof(SDFGeometry);
    _sizeInBytes += _geometries->_sdf.neighboursFlat.size() * sizeof(uint64_t);
    for (const auto& sdfIndices : _geometries->_sdf.geometryIndices)
        _sizeInBytes += sdfIndices.second.size() * sizeof(uint64_t);
    for (const auto& sdfNeighbours : _geometries->_sdf.neighbours)
        _sizeInBytes += sdfNeighbours.size() * sizeof(size_t);
}

void Model::copyFrom(const Model& rhs)
{
    if (this == &rhs)
        return;

    if (rhs._simulationHandler)
        _simulationHandler = rhs._simulationHandler->clone();

    _transferFunction = rhs._transferFunction;
    _materials.clear();
    for (const auto& material : rhs._materials)
    {
        auto newMaterial =
            createMaterialImpl(material.second->getPropertyMap());
        *newMaterial = *material.second;
        _materials[material.first] = newMaterial;
    }
    _bounds = rhs._bounds;
    _bvhFlags = rhs._bvhFlags;
    _sizeInBytes = rhs._sizeInBytes;

    // reference only to save memory
    _geometries = rhs._geometries;
}

void Model::updateBounds()
{
    if (_spheresDirty)
    {
        _geometries->_sphereBounds.reset();
        for (const auto& spheres : _geometries->_spheres)
            if (spheres.first != BOUNDINGBOX_MATERIAL_ID)
                for (const auto& sphere : spheres.second)
                {
                    _geometries->_sphereBounds.merge(sphere.center +
                                                     sphere.radius);
                    _geometries->_sphereBounds.merge(sphere.center -
                                                     sphere.radius);
                }
    }

    if (_cylindersDirty)
    {
        _geometries->_cylindersBounds.reset();
        for (const auto& cylinders : _geometries->_cylinders)
            if (cylinders.first != BOUNDINGBOX_MATERIAL_ID)
                for (const auto& cylinder : cylinders.second)
                {
                    _geometries->_cylindersBounds.merge(cylinder.center);
                    _geometries->_cylindersBounds.merge(cylinder.up);
                }
    }

    if (_conesDirty)
    {
        _geometries->_conesBounds.reset();
        for (const auto& cones : _geometries->_cones)
            if (cones.first != BOUNDINGBOX_MATERIAL_ID)
                for (const auto& cone : cones.second)
                {
                    _geometries->_conesBounds.merge(cone.center);
                    _geometries->_conesBounds.merge(cone.up);
                }
    }

    if (_trianglesMeshesDirty)
    {
        _geometries->_trianglesMeshesBounds.reset();
        for (const auto& mesh : _geometries->_trianglesMeshes)
            if (mesh.first != BOUNDINGBOX_MATERIAL_ID)
                for (const auto& vertex : mesh.second.vertices)
                    _geometries->_trianglesMeshesBounds.merge(vertex);
    }

    if (_streamlinesDirty)
    {
        _geometries->_streamlinesBounds.reset();
        for (const auto& streamline : _geometries->_streamlines)
            for (size_t index = 0; index < streamline.second.vertex.size();
                 ++index)
            {
                const auto& pos =
                    streamline.second.vertex[index].get_sub_vector<3, 0>();
                const float radius = streamline.second.vertex[index][3];
                const auto radiusVec = Vector3f(radius, radius, radius);
                _geometries->_streamlinesBounds.merge(pos + radiusVec);
                _geometries->_streamlinesBounds.merge(pos - radiusVec);
            }
    }

    if (_sdfGeometriesDirty)
    {
        _geometries->_sdfGeometriesBounds.reset();
        for (const auto& geom : _geometries->_sdf.geometries)
            _geometries->_sdfGeometriesBounds.merge(getSDFBoundingBox(geom));
    }

    if (_volumesDirty)
    {
        _geometries->_volumesBounds.reset();
        for (const auto& volume : _geometries->_volumes)
            _geometries->_volumesBounds.merge(volume->getBounds());
    }

    _bounds.reset();
    _bounds.merge(_geometries->_sphereBounds);
    _bounds.merge(_geometries->_cylindersBounds);
    _bounds.merge(_geometries->_conesBounds);
    _bounds.merge(_geometries->_trianglesMeshesBounds);
    _bounds.merge(_geometries->_streamlinesBounds);
    _bounds.merge(_geometries->_sdfGeometriesBounds);
    _bounds.merge(_geometries->_volumesBounds);
}

void Model::_markGeometriesClean()
{
    _spheresDirty = false;
    _cylindersDirty = false;
    _conesDirty = false;
    _trianglesMeshesDirty = false;
    _streamlinesDirty = false;
    _sdfGeometriesDirty = false;
    _volumesDirty = false;
}

MaterialPtr Model::createMaterial(const size_t materialId,
                                  const std::string& name,
                                  const PropertyMap& properties)
{
    auto material = _materials[materialId] = createMaterialImpl(properties);
    material->setName(name);
    if (_simulationHandler && materialId != BOUNDINGBOX_MATERIAL_ID)
        _simulationHandler->bind(material);
    return material;
}

void Model::setSimulationHandler(AbstractSimulationHandlerPtr handler)
{
    if (_simulationHandler != handler)
        _unbindMaterials(_simulationHandler, _materials);
    _simulationHandler = handler;
    _bindMaterials(_simulationHandler, _materials);
}

size_t Model::getSizeInBytes() const
{
    size_t volumeSizeInBytes = 0;
    for (const auto& volume : _geometries->_volumes)
        volumeSizeInBytes += volume->getSizeInBytes();
    return _sizeInBytes + volumeSizeInBytes;
}

AbstractSimulationHandlerPtr Model::getSimulationHandler() const
{
    return _simulationHandler;
}
}
