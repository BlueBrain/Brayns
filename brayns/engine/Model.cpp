/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/common/DefaultMaterialIds.h>
#include <brayns/common/Log.h>
#include <brayns/common/Transformation.h>
#include <brayns/common/material/Texture2D.h>
#include <brayns/engine/Material.h>
#include <brayns/engine/Volume.h>

#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/parameters/AnimationParameters.h>

#include <filesystem>
#include <set>

namespace brayns
{
ModelInstance::ModelInstance(const bool visible, const bool boundingBox, const Transformation &transformation)
    : _visible(visible)
    , _boundingBox(boundingBox)
    , _transformation(transformation)
{
}

bool ModelInstance::getVisible() const
{
    return _visible;
}

void ModelInstance::setVisible(const bool visible)
{
    _updateValue(_visible, visible);
}

bool ModelInstance::getBoundingBox() const
{
    return _boundingBox;
}

void ModelInstance::setBoundingBox(const bool enabled)
{
    _updateValue(_boundingBox, enabled);
}

const Transformation &ModelInstance::getTransformation() const
{
    return _transformation;
}

void ModelInstance::setTransformation(const Transformation &transformation)
{
    _updateValue(_transformation, transformation);
}

void ModelInstance::setModelID(const size_t id)
{
    _updateValue(_modelID, id);
}

size_t ModelInstance::getModelID() const
{
    return _modelID;
}

void ModelInstance::setInstanceID(const size_t id)
{
    _updateValue(_instanceID, id);
}

size_t ModelInstance::getInstanceID() const
{
    return _instanceID;
}

ModelParams::ModelParams(const std::string &path)
    : _name(std::filesystem::path(path).stem().string())
    , _path(path)
{
}

ModelParams::ModelParams(const std::string &name, const std::string &path)
    : _name(name)
    , _path(path)
{
}

ModelParams::ModelParams(const std::string &name, const std::string &path, const JsonValue &loadParameters)
    : _name(name)
    , _path(path)
    , _loadParameters(loadParameters)
{
}

void ModelParams::setName(const std::string &name)
{
    _updateValue(_name, name);
}

const std::string &ModelParams::getName() const
{
    return _name;
}

void ModelParams::setPath(const std::string &path)
{
    _updateValue(_path, path);
}

const std::string &ModelParams::getPath() const
{
    return _path;
}

void ModelParams::setLoaderName(const std::string &loaderName)
{
    _updateValue(_loaderName, loaderName);
}

const std::string &ModelParams::getLoaderName() const
{
    return _loaderName;
}

const JsonValue &ModelParams::getLoadParameters() const
{
    return _loadParameters;
}

void ModelParams::setLoadParameters(const JsonValue &pm)
{
    _loadParameters = pm;
}

ModelDescriptor::ModelDescriptor(ModelPtr model, const std::string &path)
    : ModelParams(path)
    , _model(std::move(model))
{
}

ModelDescriptor::ModelDescriptor(ModelPtr model, const std::string &path, const ModelMetadata &metadata)
    : ModelParams(path)
    , _metadata(metadata)
    , _model(std::move(model))
{
}

ModelDescriptor::ModelDescriptor(
    ModelPtr model,
    const std::string &name,
    const std::string &path,
    const ModelMetadata &metadata)
    : ModelParams(name, path)
    , _metadata(metadata)
    , _model(std::move(model))
{
}

ModelDescriptor &ModelDescriptor::operator=(const ModelParams &rhs)
{
    if (this == &rhs)
        return *this;

    _updateValue(_boundingBox, rhs.getBoundingBox());
    if (rhs.getName().empty())
        _updateValue(_name, std::filesystem::path(rhs.getPath()).stem().string());
    else
    {
        // TEMP HACK FFS: Append whatever is on name to the model params name
        const auto extraName = _name.empty() ? "" : " - " + _name;
        _updateValue(_name, rhs.getName() + extraName);
    }

    _updateValue(_path, rhs.getPath());
    _updateValue(_visible, rhs.getVisible());

    // Transformation
    const auto oldRotationCenter = _transformation.getRotationCenter();
    const auto newRotationCenter = rhs.getTransformation().getRotationCenter();
    _updateValue(_transformation, rhs.getTransformation());
    if (newRotationCenter == Vector3d(0))
        // If no rotation center is specified in the model params, the one set
        // by the model loader is used
        _transformation.setRotationCenter(oldRotationCenter);

    _loadParameters = rhs.getLoadParameters();
    _loaderName = rhs.getLoaderName();

    return *this;
}

bool ModelDescriptor::getEnabled() const
{
    return _visible || _boundingBox;
}

void ModelDescriptor::setMetadata(const ModelMetadata &metadata)
{
    _metadata = metadata;
    markModified();
}

const ModelMetadata &ModelDescriptor::getMetadata() const
{
    return _metadata;
}

const Model &ModelDescriptor::getModel() const
{
    return *_model;
}

Model &ModelDescriptor::getModel()
{
    return *_model;
}

void ModelDescriptor::addInstance(const ModelInstance &instance)
{
    _instances.push_back(instance);
    _instances.rbegin()->setInstanceID(_nextInstanceID++);
    if (_model)
        _model->markInstancesDirty();
}

void ModelDescriptor::removeInstance(const size_t id)
{
    auto i = std::remove_if(
        _instances.begin(),
        _instances.end(),
        [id](const auto &instance) { return id == instance.getInstanceID(); });
    if (i == _instances.end())
        return;

    _instances.erase(i, _instances.end());

    if (_model)
        _model->markInstancesDirty();
}

ModelInstance *ModelDescriptor::getInstance(const size_t id)
{
    auto i = std::find_if(
        _instances.begin(),
        _instances.end(),
        [id](const auto &instance) { return id == instance.getInstanceID(); });
    return i == _instances.end() ? nullptr : &(*i);
}

const std::vector<ModelInstance> &ModelDescriptor::getInstances() const
{
    return _instances;
}

Boxd ModelDescriptor::getBounds() const
{
    return _bounds;
}

void ModelDescriptor::computeBounds()
{
    _bounds.reset();
    if (!_model)
        return;
    for (const auto &instance : getInstances())
    {
        if (!instance.getVisible())
            continue;

        _bounds.merge(transformBox(getModel().getBounds(), getTransformation() * instance.getTransformation()));
    }
}

void ModelDescriptor::setProperties(const PropertyMap &properties)
{
    _properties = properties;
    markModified();
}

const PropertyMap &ModelDescriptor::getProperties() const
{
    return _properties;
}

void ModelDescriptor::addOnRemoved(const RemovedCallback &callback)
{
    _onRemovedCallback.push_back(callback);
}

void ModelDescriptor::callOnRemoved()
{
    if (!_onRemovedCallback.empty())
        for (const auto &callback : _onRemovedCallback)
            callback(*this);
}

void ModelDescriptor::markForRemoval()
{
    _markedForRemoval = true;
}

bool ModelDescriptor::isMarkedForRemoval() const
{
    return _markedForRemoval;
}

ModelDescriptorPtr ModelDescriptor::clone(ModelPtr model) const
{
    auto newModelDesc = std::make_shared<ModelDescriptor>(std::move(model), getPath());

    *newModelDesc = static_cast<const ModelParams &>(*this);

    newModelDesc->_bounds = _bounds;
    newModelDesc->_metadata = _metadata;
    newModelDesc->_model->copyFrom(getModel());
    newModelDesc->_instances = _instances;
    newModelDesc->_properties = _properties;
    newModelDesc->_model->buildBoundingBox();
    newModelDesc->_loadParameters = _loadParameters;
    return newModelDesc;
}

Model::Model(AnimationParameters &animationParameters, VolumeParameters &volumeParameters)
    : _animationParameters(animationParameters)
    , _volumeParameters(volumeParameters)
{
}

Model::~Model()
{
}

bool Model::empty() const
{
    return _geometries->isEmpty() && _bounds.isEmpty();
}

const Boxd &Model::getBounds() const
{
    return _bounds;
}

const std::map<size_t, std::vector<Sphere>> &Model::getSpheres() const
{
    return _geometries->_spheres;
}

std::map<size_t, std::vector<Sphere>> &Model::getSpheres()
{
    _spheresDirty = true;
    return _geometries->_spheres;
}

uint64_t Model::addSphere(const size_t materialId, const Sphere &sphere)
{
    _spheresDirty = true;
    _geometries->_spheres[materialId].push_back(sphere);
    return _geometries->_spheres[materialId].size() - 1;
}

const std::map<size_t, std::vector<Cylinder>> &Model::getCylinders() const
{
    return _geometries->_cylinders;
}

std::map<size_t, std::vector<Cylinder>> &Model::getCylinders()
{
    _cylindersDirty = true;
    return _geometries->_cylinders;
}

uint64_t Model::addCylinder(const size_t materialId, const Cylinder &cylinder)
{
    _cylindersDirty = true;
    _geometries->_cylinders[materialId].push_back(cylinder);
    return _geometries->_cylinders[materialId].size() - 1;
}

const std::map<size_t, std::vector<Cone>> &Model::getCones() const
{
    return _geometries->_cones;
}

std::map<size_t, std::vector<Cone>> &Model::getCones()
{
    _conesDirty = true;
    return _geometries->_cones;
}

uint64_t Model::addCone(const size_t materialId, const Cone &cone)
{
    _conesDirty = true;
    _geometries->_cones[materialId].push_back(cone);
    return _geometries->_cones[materialId].size() - 1;
}

void Model::addStreamline(const size_t materialId, const Streamline &streamline)
{
    if (streamline.position.size() < 2)
        throw std::runtime_error(
            "Number of vertices is less than two which is minimum needed for a "
            "streamline.");

    if (streamline.position.size() != streamline.color.size())
        throw std::runtime_error("Number of vertices and colors do not match.");

    if (streamline.position.size() != streamline.radius.size())
        throw std::runtime_error("Number of vertices and radii do not match.");

    auto &streamlinesData = _geometries->_streamlines[materialId];

    const size_t startIndex = streamlinesData.vertex.size();
    const size_t endIndex = startIndex + streamline.position.size() - 1;

    for (size_t index = startIndex; index < endIndex; ++index)
        streamlinesData.indices.push_back(index);

    for (size_t i = 0; i < streamline.position.size(); i++)
        streamlinesData.vertex.push_back(Vector4f(streamline.position[i], streamline.radius[i]));

    for (const auto &color : streamline.color)
        streamlinesData.vertexColor.push_back(color);

    _streamlinesDirty = true;
}

std::map<size_t, StreamlinesData> &Model::getStreamlines()
{
    _streamlinesDirty = true;
    return _geometries->_streamlines;
}

uint64_t
    Model::addSDFGeometry(const size_t materialId, const SDFGeometry &geom, const std::vector<size_t> &neighbourIndices)
{
    const uint64_t geomIdx = _geometries->_sdf.geometries.size();
    _geometries->_sdf.geometryIndices[materialId].push_back(geomIdx);
    _geometries->_sdf.neighbours.push_back(neighbourIndices);
    _geometries->_sdf.geometries.push_back(geom);
    _sdfGeometriesDirty = true;
    return geomIdx;
}

SDFGeometryData &Model::getSDFGeometryData()
{
    _sdfGeometriesDirty = true;
    return _geometries->_sdf;
}

void Model::updateSDFGeometryNeighbours(size_t geometryIdx, const std::vector<size_t> &neighbourIndices)
{
    _geometries->_sdf.neighbours[geometryIdx] = neighbourIndices;
    _sdfGeometriesDirty = true;
}

const std::map<size_t, TriangleMesh> &Model::getTriangleMeshes() const
{
    return _geometries->_triangleMeshes;
}

std::map<size_t, TriangleMesh> &Model::getTriangleMeshes()
{
    _triangleMeshesDirty = true;
    return _geometries->_triangleMeshes;
}

void Model::addVolume(VolumePtr volume)
{
    _geometries->_volumes.push_back(volume);
    _volumesDirty = true;
}

void Model::removeVolume(VolumePtr volume)
{
    auto i = std::find(_geometries->_volumes.begin(), _geometries->_volumes.end(), volume);
    if (i == _geometries->_volumes.end())
        return;

    _geometries->_volumes.erase(i);
    _volumesDirty = true;
}

bool Model::isDirty() const
{
    return _areGeometriesDirty() || _instancesDirty;
}

void Model::setMaterialsColorMap(const MaterialsColorMap colorMap)
{
    size_t index = 0;
    for (auto &material : _materials)
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
            material.second->setDiffuseColor(Vector3f(a * a, std::sqrt(a), 1.f - a));
            break;
        }
        case MaterialsColorMap::pastel:
            material.second->setDiffuseColor(Vector3f(
                0.5f + float(std::rand() % 127) / 255.f,
                0.5f + float(std::rand() % 127) / 255.f,
                0.5f + float(std::rand() % 127) / 255.f));
            break;
        case MaterialsColorMap::random:
            material.second->setDiffuseColor(
                Vector3f(float(rand() % 255) / 255.f, float(rand() % 255) / 255.f, float(rand() % 255) / 255.f));
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
                material.second->setReflectionIndex(float(std::rand() % 100) / 100.f);
                material.second->setSpecularColor(Vector3f(1.f));
                material.second->setSpecularExponent(10.f);
                break;
            case 3:
                // Reflection and refraction
                material.second->setReflectionIndex(float(std::rand() % 100) / 100.f);
                material.second->setOpacity(float(std::rand() % 100) / 100.f);
                material.second->setRefractionIndex(1.2f);
                material.second->setSpecularColor(Vector3f(1.f));
                material.second->setSpecularExponent(10.f);
                break;
            case 4:
                // Reflection and glossiness
                material.second->setReflectionIndex(float(std::rand() % 100) / 100.f);
                material.second->setSpecularColor(Vector3f(1.f));
                material.second->setSpecularExponent(10.f);
                material.second->setGlossiness(float(std::rand() % 100) / 100.f);
                break;
            case 5:
                // Transparency and glossiness
                material.second->setOpacity(float(std::rand() % 100) / 100.f);
                material.second->setRefractionIndex(1.2f);
                material.second->setSpecularColor(Vector3f(1.f));
                material.second->setSpecularExponent(10.f);
                material.second->setGlossiness(float(std::rand() % 100) / 100.f);
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

const std::map<size_t, MaterialPtr> &Model::getMaterials() const
{
    return _materials;
}

void Model::logInformation()
{
    _updateSizeInBytes();

    uint64_t nbSpheres = 0;
    uint64_t nbCylinders = 0;
    uint64_t nbCones = 0;
    uint64_t nbSdfBeziers = 0;
    uint64_t nbSdfGeoms = 0;
    uint64_t nbMeshes = _geometries->_triangleMeshes.size();
    uint64_t nbMetaObjects = 0;
    for (const auto &spheres : _geometries->_spheres)
        nbSpheres += spheres.second.size();
    for (const auto &cylinders : _geometries->_cylinders)
        nbCylinders += cylinders.second.size();
    for (const auto &cones : _geometries->_cones)
        nbCones += cones.second.size();
    for (const auto &sdfGeoms : _geometries->_sdf.geometryIndices)
        nbSdfGeoms += sdfGeoms.second.size();

    Log::debug(
        "Spheres: {}, Cylinders: {}, Cones: {}, SDFBeziers: {}, SDFGeometries: "
        "{}, Meshes: {}, Meta Objects: {}, Memory: {} bytes ({} MB), Bounds: "
        "{}.",
        nbSpheres,
        nbCylinders,
        nbCones,
        nbSdfBeziers,
        nbSdfGeoms,
        nbMeshes,
        nbMetaObjects,
        _sizeInBytes,
        _sizeInBytes / 1048576,
        _bounds);
}

MaterialPtr Model::getMaterial(const size_t materialId) const
{
    const auto it = _materials.find(materialId);
    if (it == _materials.end())
        throw std::runtime_error("Material " + std::to_string(materialId) + " is not registered in the model");
    return it->second;
}

void Model::_updateSizeInBytes()
{
    _sizeInBytes = 0;
    for (const auto &spheres : _geometries->_spheres)
        _sizeInBytes += spheres.second.size() * sizeof(Sphere);
    for (const auto &cylinders : _geometries->_cylinders)
        _sizeInBytes += cylinders.second.size() * sizeof(Cylinder);
    for (const auto &cones : _geometries->_cones)
        _sizeInBytes += cones.second.size() * sizeof(Cone);
    for (const auto &triangleMesh : _geometries->_triangleMeshes)
    {
        const auto &mesh = triangleMesh.second;
        _sizeInBytes += mesh.indices.size() * sizeof(Vector3f);
        _sizeInBytes += mesh.normals.size() * sizeof(Vector3f);
        _sizeInBytes += mesh.colors.size() * sizeof(Vector4f);
        _sizeInBytes += mesh.indices.size() * sizeof(Vector3ui);
        _sizeInBytes += mesh.textureCoordinates.size() * sizeof(Vector2f);
    }
    for (const auto &streamline : _geometries->_streamlines)
    {
        _sizeInBytes += streamline.second.indices.size() * sizeof(int32_t);
        _sizeInBytes += streamline.second.vertex.size() * sizeof(Vector4f);
        _sizeInBytes += streamline.second.vertexColor.size() * sizeof(Vector4f);
    }

    _sizeInBytes += _geometries->_sdf.geometries.size() * sizeof(SDFGeometry);
    _sizeInBytes += _geometries->_sdf.neighboursFlat.size() * sizeof(uint64_t);
    for (const auto &sdfIndices : _geometries->_sdf.geometryIndices)
        _sizeInBytes += sdfIndices.second.size() * sizeof(uint64_t);
    for (const auto &sdfNeighbours : _geometries->_sdf.neighbours)
        _sizeInBytes += sdfNeighbours.size() * sizeof(size_t);
}

void Model::markInstancesDirty()
{
    _instancesDirty = true;
}

void Model::markInstancesClean()
{
    _instancesDirty = false;
}

const std::vector<VolumePtr> &Model::getVolumes() const
{
    return _geometries->_volumes;
}

bool Model::isVolumesDirty() const
{
    return _volumesDirty;
}

void Model::resetVolumesDirty()
{
    _volumesDirty = false;
}

void Model::setSimulationEnabled(const bool v)
{
    _simulationEnabled = v;
    _simulationEnabledDirty = true;
}

bool Model::isSimulationEnabled() const
{
    return _simulationEnabled;
}

void Model::copyFrom(const Model &rhs)
{
    if (this == &rhs)
        return;

    if (rhs._simulationHandler)
    {
        _simulationHandler = rhs._simulationHandler->clone();
        setSimulationEnabled(true);
        // Reset simulation handler current frame so the simulation data gets
        // commited (current frame != animation params current frame)
        _simulationHandler->setCurrentFrame(std::numeric_limits<uint32_t>::max());
    }

    _materials.clear();
    for (const auto &material : rhs._materials)
    {
        auto newMaterial = createMaterialImpl(material.second->getPropertyMap());
        *newMaterial = *material.second;
        _materials[material.first] = newMaterial;
    }
    _bounds = rhs._bounds;
    _sizeInBytes = rhs._sizeInBytes;

    // reference only to save memory
    _geometries = rhs._geometries;

    _spheresDirty = !_geometries->_spheres.empty();
    _cylindersDirty = !_geometries->_cylinders.empty();
    _conesDirty = !_geometries->_cones.empty();
    _triangleMeshesDirty = !_geometries->_triangleMeshes.empty();
    _streamlinesDirty = !_geometries->_streamlines.empty();
    _sdfGeometriesDirty = !_geometries->_sdf.geometries.empty();
    _volumesDirty = !_geometries->_volumes.empty();
}

void Model::updateBounds()
{
    if (_spheresDirty)
    {
        _geometries->_sphereBounds.reset();
        for (const auto &spheres : _geometries->_spheres)
            if (spheres.first != BOUNDINGBOX_MATERIAL_ID)
                for (const auto &sphere : spheres.second)
                {
                    _geometries->_sphereBounds.merge(sphere.center + sphere.radius);
                    _geometries->_sphereBounds.merge(sphere.center - sphere.radius);
                }
    }

    if (_cylindersDirty)
    {
        _geometries->_cylindersBounds.reset();
        for (const auto &cylinders : _geometries->_cylinders)
            if (cylinders.first != BOUNDINGBOX_MATERIAL_ID)
                for (const auto &cylinder : cylinders.second)
                {
                    _geometries->_cylindersBounds.merge(cylinder.center);
                    _geometries->_cylindersBounds.merge(cylinder.up);
                }
    }

    if (_conesDirty)
    {
        _geometries->_conesBounds.reset();
        for (const auto &cones : _geometries->_cones)
            if (cones.first != BOUNDINGBOX_MATERIAL_ID)
                for (const auto &cone : cones.second)
                {
                    _geometries->_conesBounds.merge(cone.center);
                    _geometries->_conesBounds.merge(cone.up);
                }
    }

    if (_triangleMeshesDirty)
    {
        _geometries->_triangleMeshesBounds.reset();
        for (const auto &mesh : _geometries->_triangleMeshes)
            if (mesh.first != BOUNDINGBOX_MATERIAL_ID)
                for (const auto &vertex : mesh.second.vertices)
                    _geometries->_triangleMeshesBounds.merge(vertex);
    }

    if (_streamlinesDirty)
    {
        _geometries->_streamlinesBounds.reset();
        for (const auto &streamline : _geometries->_streamlines)
            for (size_t index = 0; index < streamline.second.vertex.size(); ++index)
            {
                const auto &pos = Vector3f(streamline.second.vertex[index]);
                const float radius = streamline.second.vertex[index][3];
                const auto radiusVec = Vector3f(radius, radius, radius);
                _geometries->_streamlinesBounds.merge(pos + radiusVec);
                _geometries->_streamlinesBounds.merge(pos - radiusVec);
            }
    }

    if (_sdfGeometriesDirty)
    {
        _geometries->_sdfGeometriesBounds.reset();
        for (const auto &geom : _geometries->_sdf.geometries)
            _geometries->_sdfGeometriesBounds.merge(getSDFBoundingBox(geom));
    }

    if (_volumesDirty)
    {
        _geometries->_volumesBounds.reset();
        for (const auto &volume : _geometries->_volumes)
            _geometries->_volumesBounds.merge(volume->getBounds());
    }

    _bounds.reset();
    _bounds.merge(_geometries->_sphereBounds);
    _bounds.merge(_geometries->_cylindersBounds);
    _bounds.merge(_geometries->_conesBounds);
    _bounds.merge(_geometries->_triangleMeshesBounds);
    _bounds.merge(_geometries->_streamlinesBounds);
    _bounds.merge(_geometries->_sdfGeometriesBounds);
    _bounds.merge(_geometries->_volumesBounds);
}

void Model::_markGeometriesClean()
{
    _spheresDirty = false;
    _cylindersDirty = false;
    _conesDirty = false;
    _triangleMeshesDirty = false;
    _streamlinesDirty = false;
    _sdfGeometriesDirty = false;
    _volumesDirty = false;
}

MaterialPtr Model::createMaterial(const size_t materialId, const std::string &name, const PropertyMap &properties)
{
    auto material = _materials[materialId] = createMaterialImpl(properties);
    material->setName(name);
    return material;
}

void Model::setSimulationHandler(AbstractSimulationHandlerPtr handler)
{
    setSimulationEnabled(handler != nullptr);
    _simulationHandler = handler;
}

size_t Model::getSizeInBytes() const
{
    size_t volumeSizeInBytes = 0;
    for (const auto &volume : _geometries->_volumes)
        volumeSizeInBytes += volume->getSizeInBytes();
    return _sizeInBytes + volumeSizeInBytes;
}

AbstractSimulationHandlerPtr Model::getSimulationHandler() const
{
    return _simulationHandler;
}

bool Model::Geometries::isEmpty() const
{
    return _spheres.empty() && _cylinders.empty() && _cones.empty() && _triangleMeshes.empty()
        && _sdf.geometries.empty() && _streamlines.empty() && _volumes.empty();
}

bool Model::_areGeometriesDirty() const
{
    return _spheresDirty || _cylindersDirty || _conesDirty || _triangleMeshesDirty || _sdfGeometriesDirty;
}
} // namespace brayns
