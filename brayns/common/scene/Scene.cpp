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

#include "Scene.h"

#include <brayns/common/Transformation.h>
#include <brayns/common/log.h>
#include <brayns/common/material/Material.h>
#include <brayns/common/scene/ClipPlane.h>
#include <brayns/common/scene/Model.h>
#include <brayns/common/utils/Utils.h>
#include <brayns/io/simulation/CADiffusionSimulationHandler.h>
#include <brayns/parameters/ParametersManager.h>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#include <fstream>

namespace
{
const size_t CACHE_VERSION = 10;

template <typename T, typename U = T> // U seems to be needed when getID is a
                                      // member function of a base of T.
std::shared_ptr<T> _find(const std::vector<std::shared_ptr<T>>& list,
                         const size_t id,
                         size_t (U::*getID)() const = &T::getID)
{
    auto i = std::find_if(list.begin(), list.end(), [id, getID](auto x) {
        return id == ((*x).*getID)();
    });
    return i == list.end() ? std::shared_ptr<T>{} : *i;
}

template <typename T, typename U = T>
std::shared_ptr<T> _remove(std::vector<std::shared_ptr<T>>& list,
                           const size_t id,
                           size_t (U::*getID)() const = &T::getID)
{
    auto i = std::find_if(list.begin(), list.end(), [id, getID](auto x) {
        return id == ((*x).*getID)();
    });
    if (i == list.end())
        return std::shared_ptr<T>{};
    auto result = *i;
    list.erase(i);
    return result;
}
}

namespace brayns
{
Scene::Scene(ParametersManager& parametersManager)
    : _parametersManager(parametersManager)
{
}

Scene& Scene::operator=(const Scene& rhs)
{
    if (this == &rhs)
        return *this;

    {
        std::unique_lock<std::shared_timed_mutex> lock(_modelMutex);
        std::shared_lock<std::shared_timed_mutex> rhsLock(rhs._modelMutex);
        _modelDescriptors = rhs._modelDescriptors;
    }

    *_backgroundMaterial = *rhs._backgroundMaterial;
    _backgroundMaterial->markModified();

    _lights = rhs._lights;
    _clipPlanes = rhs._clipPlanes;

    if (rhs._simulationHandler)
    {
        _simulationHandler = std::make_shared<AbstractSimulationHandler>(
            _parametersManager.getGeometryParameters());
        *_simulationHandler = *rhs._simulationHandler;
    }
    _transferFunction = rhs._transferFunction;
    _transferFunction.markModified();

    if (rhs._caDiffusionSimulationHandler)
    {
        _caDiffusionSimulationHandler =
            std::make_shared<CADiffusionSimulationHandler>();
        *_caDiffusionSimulationHandler = *rhs._caDiffusionSimulationHandler;
    }

    markModified();
    return *this;
}

size_t Scene::getSizeInBytes() const
{
    auto lock = acquireReadAccess();
    size_t sizeInBytes = 0;
    for (auto modelDescriptor : _modelDescriptors)
        sizeInBytes += modelDescriptor->getModel().getSizeInBytes();
    return sizeInBytes;
}

size_t Scene::getNumModels() const
{
    auto lock = acquireReadAccess();
    return _modelDescriptors.size();
}

void Scene::addLight(LightPtr light)
{
    removeLight(light);
    _lights.push_back(light);
}

void Scene::removeLight(LightPtr light)
{
    Lights::iterator it = std::find(_lights.begin(), _lights.end(), light);
    if (it != _lights.end())
        _lights.erase(it);
}

LightPtr Scene::getLight(const size_t index)
{
    if (index < _lights.size())
        return _lights[index];
    return 0;
}

void Scene::clearLights()
{
    _lights.clear();
}

size_t Scene::addModel(ModelDescriptorPtr model)
{
    if (model->getModel().empty())
        throw std::runtime_error("Empty models not supported.");

    model->getModel().buildBoundingBox();
    model->getModel().commit();

    {
        std::unique_lock<std::shared_timed_mutex> lock(_modelMutex);
        model->setModelID(_modelID++);
        _modelDescriptors.push_back(model);

        // add default instance of this model to render something
        if (model->getInstances().empty())
            model->addInstance({true, true, model->getTransformation()});
    }

    markModified();
    return model->getModelID();
}

void Scene::removeModel(const size_t id)
{
    bool removed = false;
    {
        std::unique_lock<std::shared_timed_mutex> lock(_modelMutex);
        auto model =
            _remove(_modelDescriptors, id, &ModelDescriptor::getModelID);
        if (model)
        {
            model->callOnRemoved();
            removed = true;
        }
    }
    if (removed)
        markModified();
}

ModelDescriptorPtr Scene::getModel(const size_t id) const
{
    auto lock = acquireReadAccess();
    return _find(_modelDescriptors, id, &ModelDescriptor::getModelID);
}

void Scene::setSimulationHandler(AbstractSimulationHandlerPtr handler)
{
    auto& ap = _parametersManager.getAnimationParameters();
    _simulationHandler = handler;
    if (_simulationHandler)
    {
        ap.setDt(_simulationHandler->getDt());
        ap.setUnit(_simulationHandler->getUnit());
        ap.setEnd(_simulationHandler->getNbFrames());
    }
    else
        ap.reset();
}

AbstractSimulationHandlerPtr Scene::getSimulationHandler() const
{
    return _simulationHandler;
}

void Scene::setCADiffusionSimulationHandler(
    CADiffusionSimulationHandlerPtr handler)
{
    _caDiffusionSimulationHandler = handler;
    if (_caDiffusionSimulationHandler)
        _parametersManager.getAnimationParameters().setEnd(
            _caDiffusionSimulationHandler->getNbFrames());
    else
        _parametersManager.getAnimationParameters().reset();
}

CADiffusionSimulationHandlerPtr Scene::getCADiffusionSimulationHandler() const
{
    return _caDiffusionSimulationHandler;
}

bool Scene::empty() const
{
    auto lock = acquireReadAccess();
    for (auto modelDescriptor : _modelDescriptors)
        if (!modelDescriptor->getModel().empty())
            return false;
    return true;
}

size_t Scene::addClipPlane(const Plane& plane)
{
    auto clipPlane = std::make_shared<ClipPlane>(plane);
    clipPlane->onModified([&](const BaseObject&){ markModified(); });
    _clipPlanes.emplace_back(std::move(clipPlane));
    markModified();
    return _clipPlanes.back()->getID();
}

ClipPlanePtr Scene::getClipPlane(const size_t id) const
{
    return _find(_clipPlanes, id);
}

void Scene::removeClipPlane(const size_t id)
{
    if (_remove(_clipPlanes, id))
        markModified();
}

ModelDescriptorPtr Scene::load(Blob&& blob, const size_t materialID,
                               Loader::UpdateCallback cb)
{
    auto loader = _loaderRegistry.createLoader(blob.type);
    loader->setProgressCallback(cb);
    auto modelDescriptor =
        loader->importFromBlob(std::move(blob), 0, materialID);
    if (!modelDescriptor)
        throw std::runtime_error("No model returned by loader");
    addModel(modelDescriptor);
    saveToCacheFile();
    return modelDescriptor;
}

ModelDescriptorPtr Scene::load(const std::string& path, const size_t materialID,
                               Loader::UpdateCallback cb)
{
    ModelDescriptorPtr modelDescriptor;
    if (fs::is_directory(path))
    {
        fs::directory_iterator begin(path), end;
        const int numFiles =
            std::count_if(begin, end,
                          [& registry = _loaderRegistry](const auto& d) {
                              return !fs::is_directory(d.path()) &&
                                     registry.isSupported(d.path().string());
                          });

        if (numFiles == 0)
            throw std::runtime_error("No supported file found to load");

        float totalProgress = 0.f;

        size_t index = 0;
        for (const auto& i :
             boost::make_iterator_range(fs::directory_iterator(path), {}))
        {
            const auto& currentPath = i.path().string();
            if (fs::is_directory(i.path()) ||
                !_loaderRegistry.isSupported(currentPath))
            {
                continue;
            }
            auto loader = _loaderRegistry.createLoader(currentPath);

            auto progressCb = [cb, numFiles, totalProgress](auto msg,
                                                            auto amount) {
                cb(msg, totalProgress + (amount / numFiles));
            };

            loader->setProgressCallback(progressCb);
            modelDescriptor =
                loader->importFromFile(currentPath, index++, materialID);
            if (!modelDescriptor)
                throw std::runtime_error("No model returned by loader");
            addModel(modelDescriptor);

            totalProgress += 1.f / numFiles;
        }
    }
    else
    {
        auto loader = _loaderRegistry.createLoader(path);
        loader->setProgressCallback(cb);
        modelDescriptor = loader->importFromFile(path, 0, materialID);
        if (!modelDescriptor)
            throw std::runtime_error("No model returned by loader");
        addModel(modelDescriptor);
    }
    saveToCacheFile();
    buildEnvironmentMap();
    return modelDescriptor;
}

void Scene::saveToCacheFile()
{
    const auto& geometryParameters = _parametersManager.getGeometryParameters();
    if (!geometryParameters.getLoadCacheFile().empty() ||
        geometryParameters.getSaveCacheFile().empty())
    {
        return;
    }

    const auto& filename = geometryParameters.getSaveCacheFile();
    BRAYNS_INFO << "Saving scene to binary file: " << filename << std::endl;
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file.good())
    {
        BRAYNS_ERROR << "Could not open cache file " << filename << std::endl;
        return;
    }

    const size_t version = CACHE_VERSION;
    file.write((char*)&version, sizeof(size_t));
    BRAYNS_INFO << "Version: " << version << std::endl;

    // Save geometry
    auto lock = acquireReadAccess();
    size_t nbElements = _modelDescriptors.size();
    file.write((char*)&nbElements, sizeof(size_t));
    for (auto modelDescriptor : _modelDescriptors)
    {
        uint64_t bufferSize{0};

        // Model path
        auto path = modelDescriptor->getPath();
        nbElements = path.length();
        file.write((char*)&nbElements, sizeof(size_t));
        file.write((char*)path.c_str(), nbElements * sizeof(char));

        auto& model = modelDescriptor->getModel();
        const auto& materials = model.getMaterials();
        const auto nbMaterials = materials.size();
        file.write((char*)&nbMaterials, sizeof(size_t));
        BRAYNS_INFO << nbMaterials << " materials" << std::endl;

        // Save materials
        for (auto& material : materials)
        {
            file.write((char*)&material.first, sizeof(size_t));

            auto name = material.second->getName();
            nbElements = name.length();
            file.write((char*)&nbElements, sizeof(size_t));
            file.write((char*)name.c_str(), nbElements * sizeof(char));

            Vector3f value3f;
            value3f = material.second->getDiffuseColor();
            file.write((char*)&value3f, sizeof(Vector3f));
            value3f = material.second->getSpecularColor();
            file.write((char*)&value3f, sizeof(Vector3f));
            float value = material.second->getSpecularExponent();
            file.write((char*)&value, sizeof(float));
            value = material.second->getReflectionIndex();
            file.write((char*)&value, sizeof(float));
            value = material.second->getOpacity();
            file.write((char*)&value, sizeof(float));
            value = material.second->getRefractionIndex();
            file.write((char*)&value, sizeof(float));
            value = material.second->getEmission();
            file.write((char*)&value, sizeof(float));
            value = material.second->getGlossiness();
            file.write((char*)&value, sizeof(float));
            const bool boolean = material.second->getCastSimulationData();
            file.write((char*)&boolean, sizeof(bool));
        }

        // Spheres
        nbElements = model.getSpheres().size();
        file.write((char*)&nbElements, sizeof(size_t));
        for (auto& spheres : model.getSpheres())
        {
            const auto materialId = spheres.first;
            file.write((char*)&materialId, sizeof(size_t));

            const auto& data = spheres.second;
            nbElements = data.size();
            file.write((char*)&nbElements, sizeof(size_t));
            bufferSize = nbElements * sizeof(Sphere);
            file.write((char*)data.data(), bufferSize);
            BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                         << " spheres" << std::endl;
        }

        // Cylinders
        nbElements = model.getCylinders().size();
        file.write((char*)&nbElements, sizeof(size_t));
        for (auto& cylinders : model.getCylinders())
        {
            const auto materialId = cylinders.first;
            file.write((char*)&materialId, sizeof(size_t));

            const auto& data = cylinders.second;
            nbElements = data.size();
            file.write((char*)&nbElements, sizeof(size_t));
            bufferSize = nbElements * sizeof(Cylinder);
            file.write((char*)data.data(), bufferSize);
            BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                         << " cylinders" << std::endl;
        }

        // Cones
        nbElements = model.getCones().size();
        file.write((char*)&nbElements, sizeof(size_t));
        for (auto& cones : model.getCones())
        {
            const auto materialId = cones.first;
            file.write((char*)&materialId, sizeof(size_t));

            const auto& data = cones.second;
            nbElements = data.size();
            file.write((char*)&nbElements, sizeof(size_t));
            bufferSize = nbElements * sizeof(Cone);
            file.write((char*)data.data(), bufferSize);
            BRAYNS_DEBUG << "[" << materialId << "] " << nbElements << " cones"
                         << std::endl;
        }

        // Meshes
        nbElements = model.getTrianglesMeshes().size();
        file.write((char*)&nbElements, sizeof(size_t));
        for (const auto& meshes : model.getTrianglesMeshes())
        {
            const auto materialId = meshes.first;
            file.write((char*)&materialId, sizeof(size_t));

            const auto& data = meshes.second;

            // Vertices
            nbElements = data.vertices.size();
            file.write((char*)&nbElements, sizeof(size_t));
            bufferSize = nbElements * sizeof(Vector3f);
            file.write((char*)data.vertices.data(), bufferSize);
            if (nbElements != 0)
                BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                             << " vertices" << std::endl;

            // Indices
            nbElements = data.indices.size();
            file.write((char*)&nbElements, sizeof(size_t));
            bufferSize = nbElements * sizeof(Vector3ui);
            file.write((char*)data.indices.data(), bufferSize);
            if (nbElements != 0)
                BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                             << " indices" << std::endl;

            // Normals
            nbElements = data.normals.size();
            file.write((char*)&nbElements, sizeof(size_t));
            bufferSize = nbElements * sizeof(Vector3f);
            file.write((char*)data.normals.data(), bufferSize);
            if (nbElements != 0)
                BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                             << " normals" << std::endl;

            // Texture coordinates
            nbElements = data.textureCoordinates.size();
            file.write((char*)&nbElements, sizeof(size_t));
            bufferSize = nbElements * sizeof(Vector2f);
            file.write((char*)data.textureCoordinates.data(), bufferSize);
            if (nbElements != 0)
                BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                             << " texture coordinates" << std::endl;
        }
    }

    file.close();
    BRAYNS_INFO << "Scene successfully saved" << std::endl;
}

void Scene::loadFromCacheFile()
{
    const auto& geomParams = _parametersManager.getGeometryParameters();
    const auto& filename = geomParams.getLoadCacheFile();
    BRAYNS_INFO << "Loading scene from binary file: " << filename << std::endl;
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.good())
    {
        BRAYNS_ERROR << "Could not open cache file " << filename << std::endl;
        return;
    }

    // File version
    size_t version;
    file.read((char*)&version, sizeof(size_t));
    BRAYNS_INFO << "Version: " << version << std::endl;

    if (version != CACHE_VERSION)
    {
        BRAYNS_ERROR << "Only version " << CACHE_VERSION << " is supported"
                     << std::endl;
        return;
    }

    // Geometry
    size_t nbModels = 0;
    size_t nbSpheres = 0;
    size_t nbCylinders = 0;
    size_t nbCones = 0;
    size_t nbMeshes = 0;
    size_t nbVertices = 0;
    size_t nbIndices = 0;
    size_t nbNormals = 0;
    size_t nbTexCoords = 0;

    file.read((char*)&nbModels, sizeof(size_t));
    for (size_t modelId = 0; modelId < nbModels; ++modelId)
    {
        // Model path
        size_t nbElements;
        file.read((char*)&nbElements, sizeof(size_t));
        char path[255];
        file.read((char*)&path, nbElements * sizeof(char));
        path[nbElements] = 0;

        // Create model
        auto model = createModel();

        size_t nbMaterials;
        file.read((char*)&nbMaterials, sizeof(size_t));
        BRAYNS_INFO << nbMaterials << " materials" << std::endl;

        // Materials
        size_t materialId;
        for (size_t i = 0; i < nbMaterials; ++i)
        {
            file.read((char*)&materialId, sizeof(size_t));

            char materialName[255];
            file.read((char*)&nbElements, sizeof(size_t));
            file.read((char*)&materialName, nbElements * sizeof(char));
            materialName[nbElements] = 0;

            auto material = model->createMaterial(materialId, materialName);

            Vector3f value3f;
            file.read((char*)&value3f, sizeof(Vector3f));
            material->setDiffuseColor(value3f);
            file.read((char*)&value3f, sizeof(Vector3f));
            material->setSpecularColor(value3f);
            float value;
            file.read((char*)&value, sizeof(float));
            material->setSpecularExponent(value);
            file.read((char*)&value, sizeof(float));
            material->setReflectionIndex(value);
            file.read((char*)&value, sizeof(float));
            material->setOpacity(value);
            file.read((char*)&value, sizeof(float));
            material->setRefractionIndex(value);
            file.read((char*)&value, sizeof(float));
            material->setEmission(value);
            file.read((char*)&value, sizeof(float));
            material->setGlossiness(value);
            bool boolean;
            file.read((char*)&boolean, sizeof(bool));
            material->setCastSimulationData(boolean);
        }

        uint64_t bufferSize{0};
        // Spheres
        file.read((char*)&nbSpheres, sizeof(size_t));
        for (size_t i = 0; i < nbSpheres; ++i)
        {
            file.read((char*)&materialId, sizeof(size_t));
            file.read((char*)&nbElements, sizeof(size_t));
            bufferSize = nbElements * sizeof(Sphere);
            BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                         << " spheres" << std::endl;
            auto& spheres = model->getSpheres()[materialId];
            spheres.resize(nbElements);
            file.read((char*)spheres.data(), bufferSize);
        }

        // Cylinders
        file.read((char*)&nbCylinders, sizeof(size_t));
        for (size_t i = 0; i < nbCylinders; ++i)
        {
            file.read((char*)&materialId, sizeof(size_t));
            file.read((char*)&nbElements, sizeof(size_t));
            bufferSize = nbElements * sizeof(Cylinder);
            BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                         << " cylinders" << std::endl;
            auto& cylinders = model->getCylinders()[materialId];
            cylinders.resize(nbElements);
            file.read((char*)cylinders.data(), bufferSize);
        }

        // Cones
        file.read((char*)&nbCones, sizeof(size_t));
        for (size_t i = 0; i < nbCones; ++i)
        {
            file.read((char*)&materialId, sizeof(size_t));
            file.read((char*)&nbElements, sizeof(size_t));
            bufferSize = nbElements * sizeof(Cone);
            BRAYNS_DEBUG << "[" << materialId << "] " << nbElements << " cones"
                         << std::endl;
            auto& cones = model->getCones()[materialId];
            cones.resize(nbElements);
            file.read((char*)cones.data(), bufferSize);
        }

        // Meshes
        file.read((char*)&nbMeshes, sizeof(size_t));
        for (size_t i = 0; i < nbMeshes; ++i)
        {
            file.read((char*)&materialId, sizeof(size_t));
            auto& meshes = model->getTrianglesMeshes()[materialId];
            // Vertices
            file.read((char*)&nbVertices, sizeof(size_t));
            if (nbVertices != 0)
            {
                BRAYNS_DEBUG << "[" << materialId << "] " << nbVertices
                             << " vertices" << std::endl;
                meshes.vertices.resize(nbVertices);
                bufferSize = nbVertices * sizeof(Vector3f);
                file.read((char*)meshes.vertices.data(), bufferSize);
            }

            // Indices
            file.read((char*)&nbIndices, sizeof(size_t));
            if (nbIndices != 0)
            {
                BRAYNS_DEBUG << "[" << materialId << "] " << nbIndices
                             << " indices" << std::endl;
                meshes.indices.resize(nbIndices);
                bufferSize = nbIndices * sizeof(Vector3ui);
                file.read((char*)meshes.indices.data(), bufferSize);
            }

            // Normals
            file.read((char*)&nbNormals, sizeof(size_t));
            if (nbNormals != 0)
            {
                BRAYNS_DEBUG << "[" << materialId << "] " << nbNormals
                             << " normals" << std::endl;
                meshes.normals.resize(nbNormals);
                bufferSize = nbNormals * sizeof(Vector3f);
                file.read((char*)meshes.normals.data(), bufferSize);
            }

            // Texture coordinates
            file.read((char*)&nbTexCoords, sizeof(size_t));
            if (nbTexCoords != 0)
            {
                BRAYNS_DEBUG << "Material " << materialId << ": " << nbTexCoords
                             << " texture coordinates" << std::endl;
                meshes.textureCoordinates.resize(nbTexCoords);
                bufferSize = nbTexCoords * sizeof(Vector2f);
                file.read((char*)meshes.textureCoordinates.data(), bufferSize);
            }
        }

        BRAYNS_INFO << "[" << materialId << "] " << nbSpheres << " spheres, "
                    << nbCylinders << " cylinders, " << nbCones << " cones, "
                    << nbVertices << " vertices, " << nbIndices << " indices, "
                    << nbNormals << " normals, " << nbTexCoords
                    << " texture coordinates" << std::endl;

        addModel(std::make_shared<ModelDescriptor>(std::move(model), path));
    }

    file.close();
    BRAYNS_INFO << "Scene successfully loaded" << std::endl;
}

void Scene::buildDefault()
{
    BRAYNS_INFO << "Building default Cornell Box scene" << std::endl;

    auto model = createModel();

    const Vector3f WHITE = {1.f, 1.f, 1.f};

    const Vector3f positions[8] = {
        {0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}, //    6--------7
        {0.f, 1.f, 0.f},                  //   /|       /|
        {1.f, 1.f, 0.f},                  //  2--------3 |
        {0.f, 0.f, 1.f},                  //  | |      | |
        {1.f, 0.f, 1.f},                  //  | 4------|-5
        {0.f, 1.f, 1.f},                  //  |/       |/
        {1.f, 1.f, 1.f}                   //  0--------1
    };

    const uint16_t indices[6][6] = {
        {5, 4, 6, 6, 7, 5}, // Front
        {7, 5, 1, 1, 3, 7}, // Right
        {3, 1, 0, 0, 2, 3}, // Back
        {2, 0, 4, 4, 6, 2}, // Left
        {0, 1, 5, 5, 4, 0}, // Bottom
        {7, 3, 2, 2, 6, 7}  // Top
    };

    const Vector3f colors[6] = {{0.8f, 0.8f, 0.8f}, {1.f, 0.f, 0.f},
                                {0.8f, 0.8f, 0.8f}, {0.f, 1.f, 0.f},
                                {0.8f, 0.8f, 0.8f}, {0.8f, 0.8f, 0.8f}};

    size_t materialId = 0;
    for (size_t i = 1; i < 6; ++i)
    {
        // Cornell box
        auto material =
            model->createMaterial(materialId,
                                  "wall_" + std::to_string(materialId));
        material->setDiffuseColor(colors[i]);
        material->setSpecularColor(WHITE);
        material->setSpecularExponent(10.f);
        material->setReflectionIndex(i == 4 ? 0.2f : 0.f);
        material->setGlossiness(i == 4 ? 0.9f : 1.f);
        material->setOpacity(1.f);

        auto& trianglesMesh = model->getTrianglesMeshes()[materialId];
        for (size_t j = 0; j < 6; ++j)
        {
            const auto position = positions[indices[i][j]];
            trianglesMesh.vertices.push_back(position);
        }
        trianglesMesh.indices.push_back(Vector3ui(0, 1, 2));
        trianglesMesh.indices.push_back(Vector3ui(3, 4, 5));
        ++materialId;
    }

    {
        // Sphere
        auto material = model->createMaterial(materialId, "sphere");
        material->setOpacity(0.2f);
        material->setRefractionIndex(1.5f);
        material->setReflectionIndex(0.1f);
        material->setDiffuseColor(WHITE);
        material->setSpecularColor(WHITE);
        material->setSpecularExponent(100.f);
        model->addSphere(materialId, {{0.25f, 0.26f, 0.30f}, 0.25f});
        ++materialId;
    }

    {
        // Cylinder
        auto material = model->createMaterial(materialId, "cylinder");
        material->setDiffuseColor({0.1f, 0.1f, 0.8f});
        material->setSpecularColor(WHITE);
        material->setSpecularExponent(10.f);
        model->addCylinder(materialId, {{0.25f, 0.126f, 0.75f},
                                        {0.75f, 0.126f, 0.75f},
                                        0.125f});
        ++materialId;
    }

    {
        // Cone
        auto material = model->createMaterial(materialId, "cone");
        material->setReflectionIndex(0.8f);
        material->setSpecularColor(WHITE);
        material->setSpecularExponent(10.f);
        model->addCone(materialId, {{0.75f, 0.01f, 0.25f},
                                    {0.75f, 0.5f, 0.25f},
                                    0.15f,
                                    0.f});
        ++materialId;
    }

    {
        // Lamp
        auto material = model->createMaterial(materialId, "lamp");
        material->setDiffuseColor(WHITE);
        material->setEmission(5.f);
        const Vector3f lampInfo = {0.15f, 0.99f, 0.15f};
        const Vector3f lampPositions[4] = {
            {0.5f - lampInfo.x(), lampInfo.y(), 0.5f - lampInfo.z()},
            {0.5f + lampInfo.x(), lampInfo.y(), 0.5f - lampInfo.z()},
            {0.5f + lampInfo.x(), lampInfo.y(), 0.5f + lampInfo.z()},
            {0.5f - lampInfo.x(), lampInfo.y(), 0.5f + lampInfo.z()}};
        auto& trianglesMesh = model->getTrianglesMeshes()[materialId];
        for (size_t i = 0; i < 4; ++i)
            trianglesMesh.vertices.push_back(lampPositions[i]);
        trianglesMesh.indices.push_back(Vector3i(2, 1, 0));
        trianglesMesh.indices.push_back(Vector3i(0, 3, 2));
    }

    addModel(
        std::make_shared<ModelDescriptor>(std::move(model), "DefaultScene"));
}

void Scene::setMaterialsColorMap(MaterialsColorMap colorMap)
{
    {
        auto lock = acquireReadAccess();
        for (auto modelDescriptors : _modelDescriptors)
            modelDescriptors->getModel().setMaterialsColorMap(colorMap);
    }
    markModified();
}

void Scene::_computeBounds()
{
    std::unique_lock<std::shared_timed_mutex> lock(_modelMutex);
    _bounds.reset();
    for (auto modelDescriptor : _modelDescriptors)
    {
        modelDescriptor->computeBounds();
        _bounds.merge(modelDescriptor->getBounds());
    }

    if (_bounds.isEmpty())
        // If no model is enabled. return empty bounding box
        _bounds.merge({0, 0, 0});
}

void Scene::buildEnvironmentMap()
{
    const auto& environmentMap =
        _parametersManager.getSceneParameters().getEnvironmentMap();
    if (!environmentMap.empty())
        _backgroundMaterial->setTexture(environmentMap, TT_DIFFUSE);
}
}
