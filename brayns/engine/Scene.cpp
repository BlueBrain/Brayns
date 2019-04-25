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
#include <brayns/common/scene/ClipPlane.h>
#include <brayns/common/utils/utils.h>
#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>

#include <brayns/parameters/GeometryParameters.h>

#include <fstream>

namespace
{
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
} // namespace

namespace brayns
{
Scene::Scene(AnimationParameters& animationParameters,
             GeometryParameters& geometryParameters,
             VolumeParameters& volumeParameters)
    : _animationParameters(animationParameters)
    , _geometryParameters(geometryParameters)
    , _volumeParameters(volumeParameters)
{
}

void Scene::copyFrom(const Scene& rhs)
{
    if (this == &rhs)
        return;

    {
        std::unique_lock<std::shared_timed_mutex> lock(_modelMutex);
        std::shared_lock<std::shared_timed_mutex> rhsLock(rhs._modelMutex);

        _modelDescriptors.clear();
        _modelDescriptors.reserve(rhs._modelDescriptors.size());
        for (const auto& modelDesc : rhs._modelDescriptors)
            _modelDescriptors.push_back(modelDesc->clone(createModel()));
    }
    _computeBounds();

    *_backgroundMaterial = *rhs._backgroundMaterial;
    _backgroundMaterial->markModified();

    _lightManager = rhs._lightManager;
    _clipPlanes = rhs._clipPlanes;

    markModified();
}

void Scene::commit() {}

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

size_t Scene::addModel(ModelDescriptorPtr modelDescriptor)
{
    auto& model = modelDescriptor->getModel();
    if (model.empty())
        throw std::runtime_error("Empty models not supported.");

    const auto defaultBVHFlags = _geometryParameters.getDefaultBVHFlags();

    model.setBVHFlags(defaultBVHFlags);
    model.buildBoundingBox();

    // Since models can be added concurrently we check if that is supported
    if (supportsConcurrentSceneUpdates())
        model.commitGeometry();

    {
        std::unique_lock<std::shared_timed_mutex> lock(_modelMutex);
        modelDescriptor->setModelID(_modelID++);
        _modelDescriptors.push_back(modelDescriptor);

        // add default instance of this model to render something
        if (modelDescriptor->getInstances().empty())
            modelDescriptor->addInstance(
                {true, true, modelDescriptor->getTransformation()});
    }

    markModified();

    return modelDescriptor->getModelID();
}

bool Scene::removeModel(const size_t id)
{
    ModelDescriptorPtr model = nullptr;

    if (supportsConcurrentSceneUpdates())
    {
        {
            std::unique_lock<std::shared_timed_mutex> lock(_modelMutex);
            model =
                _remove(_modelDescriptors, id, &ModelDescriptor::getModelID);
        }
        if (model)
            model->callOnRemoved();
    }
    else
    {
        {
            std::unique_lock<std::shared_timed_mutex> lock(_modelMutex);
            model = _find(_modelDescriptors, id, &ModelDescriptor::getModelID);
        }
        if (model)
            model->markForRemoval();
    }

    if (model)
    {
        markModified();
        return true;
    }

    return false;
}

ModelDescriptorPtr Scene::getModel(const size_t id) const
{
    auto lock = acquireReadAccess();
    return _find(_modelDescriptors, id, &ModelDescriptor::getModelID);
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
    clipPlane->onModified([&](const BaseObject&) { markModified(); });
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

ModelDescriptorPtr Scene::loadModel(Blob&& blob, const ModelParams& params,
                                    LoaderProgress cb)
{
    const auto& loader =
        _loaderRegistry.getSuitableLoader("", blob.type,
                                          params.getLoaderName());

    // HACK: Add loader name in properties for archive loader
    auto propCopy = params.getLoaderProperties();
    propCopy.setProperty({"loaderName", params.getLoaderName()});
    auto modelDescriptor = loader.importFromBlob(std::move(blob), cb, propCopy);
    if (!modelDescriptor)
        throw std::runtime_error("No model returned by loader");
    *modelDescriptor = params;
    addModel(modelDescriptor);
    return modelDescriptor;
}

ModelDescriptorPtr Scene::loadModel(const std::string& path,
                                    const ModelParams& params,
                                    LoaderProgress cb)
{
    const auto& loader =
        _loaderRegistry.getSuitableLoader(path, "", params.getLoaderName());
    // HACK: Add loader name in properties for archive loader
    auto propCopy = params.getLoaderProperties();
    propCopy.setProperty({"loaderName", params.getLoaderName()});
    auto modelDescriptor = loader.importFromFile(path, cb, propCopy);
    if (!modelDescriptor)
        throw std::runtime_error("No model returned by loader");
    *modelDescriptor = params;
    addModel(modelDescriptor);
    return modelDescriptor;
}

void Scene::visitModels(const std::function<void(Model&)>& functor)
{
    std::unique_lock<std::shared_timed_mutex> lock(_modelMutex);
    for (const auto& modelDescriptor : _modelDescriptors)
        functor(modelDescriptor->getModel());
}

void Scene::buildGrid()
{
    BRAYNS_INFO << "Building Grid scene" << std::endl;

    auto model = createModel();
    const float radius = 0.0005;
    const float gridStep = 0.2;
    const float planeOpacity = 0.5;

    const Vector3f red = {1, 0, 0};
    const Vector3f green = {0, 1, 0};
    const Vector3f blue = {0, 0, 1};
    const Vector3f grey = {0.5, 0.5, 0.5};

    auto material = model->createMaterial(0, "x");
    material->setDiffuseColor(grey);
    material = model->createMaterial(1, "y");
    material->setDiffuseColor(grey);
    material = model->createMaterial(2, "z");
    material->setDiffuseColor(grey);

    for (double x = -1.0; x <= 1.0; x += gridStep)
        for (double y = -1.0; y <= 1.0; y += gridStep)
            if (abs(x) < 0.01 || abs(y) < 0.01)
            {
                model->addCylinder(2, {{x, y, -1}, {x, y, 1}, radius});
                model->addCylinder(0, {{-1, x, y}, {1, x, y}, radius});
                model->addCylinder(1, {{x, -1, y}, {x, 1, y}, radius});
            }

    material = model->createMaterial(3, "plane_x");
    material->setDiffuseColor(red);
    material->setOpacity(planeOpacity);
    auto& tmx = model->getTrianglesMeshes()[3];
    tmx.vertices.push_back({-1, 0, -1});
    tmx.vertices.push_back({1, 0, -1});
    tmx.vertices.push_back({1, 0, 1});
    tmx.vertices.push_back({-1, 0, 1});
    tmx.indices.push_back(Vector3ui(0, 1, 2));
    tmx.indices.push_back(Vector3ui(2, 3, 0));

    material = model->createMaterial(4, "plane_y");
    material->setDiffuseColor(green);
    material->setOpacity(planeOpacity);
    auto& tmy = model->getTrianglesMeshes()[4];
    tmy.vertices.push_back({-1, -1, 0});
    tmy.vertices.push_back({1, -1, 0});
    tmy.vertices.push_back({1, 1, 0});
    tmy.vertices.push_back({-1, 1, 0});
    tmy.indices.push_back(Vector3ui(0, 1, 2));
    tmy.indices.push_back(Vector3ui(2, 3, 0));

    material = model->createMaterial(5, "plane_z");
    material->setDiffuseColor(blue);
    material->setOpacity(planeOpacity);
    auto& tmz = model->getTrianglesMeshes()[5];
    tmz.vertices.push_back({0, -1, -1});
    tmz.vertices.push_back({0, -1, 1});
    tmz.vertices.push_back({0, 1, 1});
    tmz.vertices.push_back({0, 1, -1});
    tmz.indices.push_back(Vector3ui(0, 1, 2));
    tmz.indices.push_back(Vector3ui(2, 3, 0));

    addModel(std::make_shared<ModelDescriptor>(std::move(model), "Grid"));
}

void Scene::buildAxis()
{
    BRAYNS_INFO << "Building Axis scene" << std::endl;

    auto model = createModel();

    const float smallRadius = 0.01;
    const float largeRadius = 0.02;

    // X
    size_t materialId = 0;
    auto material = model->createMaterial(materialId, "x_axis");
    material->setDiffuseColor({1, 0, 0});
    model->addCylinder(materialId, {{0, 0, 0}, {0.89f, 0, 0}, smallRadius});
    model->addCone(materialId,
                   {{0.89, 0, 0}, {0.9f, 0, 0}, smallRadius, largeRadius});
    model->addCone(materialId, {{0.9, 0, 0}, {1.f, 0, 0}, largeRadius, 0});

    // Y
    materialId = 1;
    material = model->createMaterial(materialId, "y_axis");
    material->setDiffuseColor({0, 1, 0});
    model->addCylinder(materialId, {{0, 0, 0}, {0, 0.89f, 0}, smallRadius});
    model->addCone(materialId,
                   {{0, 0.89, 0}, {0, 0.9f, 0}, smallRadius, largeRadius});
    model->addCone(materialId, {{0, 0.9, 0}, {0, 1.f, 0}, largeRadius, 0});

    // Z
    materialId = 2;
    material = model->createMaterial(materialId, "z_axis");
    material->setDiffuseColor({0, 0, 1});
    model->addCylinder(materialId, {{0, 0, 0}, {0, 0, 0.89f}, smallRadius});
    model->addCone(materialId,
                   {{0, 0, 0.89}, {0, 0, 0.9f}, smallRadius, largeRadius});
    model->addCone(materialId, {{0, 0, 0.9}, {0, 0, 1.f}, largeRadius, 0});

    // Origin
    materialId = 3;
    material = model->createMaterial(materialId, "origin");
    material->setDiffuseColor({1, 1, 1});
    model->addSphere(materialId, {{0, 0, 0}, smallRadius});

    addModel(std::make_shared<ModelDescriptor>(std::move(model), "Axis"));
}

void Scene::buildCornellBox()
{
    BRAYNS_INFO << "Building Cornell Box scene" << std::endl;

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
            {0.5f - lampInfo.x, lampInfo.y, 0.5f - lampInfo.z},
            {0.5f + lampInfo.x, lampInfo.y, 0.5f - lampInfo.z},
            {0.5f + lampInfo.x, lampInfo.y, 0.5f + lampInfo.z},
            {0.5f - lampInfo.x, lampInfo.y, 0.5f + lampInfo.z}};
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

bool Scene::setEnvironmentMap(const std::string& envMap)
{
    bool success = true;
    if (envMap.empty())
        _backgroundMaterial->removeTexture(TT_DIFFUSE);
    else
    {
        try
        {
            _backgroundMaterial->setTexture(envMap, TT_DIFFUSE);
        }
        catch (const std::runtime_error& e)
        {
            BRAYNS_DEBUG << "Cannot load environment map: " << e.what()
                         << std::endl;
            _backgroundMaterial->removeTexture(TT_DIFFUSE);
            success = false;
        }
    }

    _updateValue(_environmentMap, success ? envMap : "");
    if (_backgroundMaterial->isModified())
        markModified();
    return success;
}

bool Scene::hasEnvironmentMap() const
{
    return !_environmentMap.empty();
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
} // namespace brayns
