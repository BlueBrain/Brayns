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

#include "Scene.h"

#include <brayns/common/geometry/Model.h>
#include <brayns/common/log.h>
#include <brayns/common/material/Material.h>
#include <brayns/common/utils/Utils.h>
#include <brayns/common/volume/VolumeHandler.h>
#include <brayns/io/NESTLoader.h>
#include <brayns/io/TransferFunctionLoader.h>
#include <brayns/io/simulation/CADiffusionSimulationHandler.h>
#include <brayns/parameters/ParametersManager.h>

#include <boost/filesystem.hpp>
#include <fstream>

namespace
{
const size_t CACHE_VERSION = 9;
}

namespace brayns
{
Scene::Scene(Renderers renderers, ParametersManager& parametersManager,
             MaterialManager& materialManager)
    : _renderers(renderers)
    , _parametersManager(parametersManager)
    , _materialManager(materialManager)
    , _volumeHandler(nullptr)
    , _simulationHandler(nullptr)
    , _caDiffusionSimulationHandler(nullptr)
{
}

Scene::~Scene()
{
    BRAYNS_FCT_ENTRY

    reset();
}

void Scene::reset()
{
    BRAYNS_FCT_ENTRY

    unload();
    _renderers.clear();
    _bounds.reset();
}

void Scene::unload()
{
    BRAYNS_FCT_ENTRY

    _materialManager.clear();
    _markGeometryDirty();
    for (auto model : _models)
        model->unload();
    _models.clear();
    _modelDescriptors.clear();
    _caDiffusionSimulationHandler.reset();
    _simulationHandler.reset();
    _volumeHandler.reset();

    markModified();
}

void Scene::_markGeometryDirty()
{
    BRAYNS_FCT_ENTRY

    _geometryGroupsDirty = true;
    markModified();
}

void Scene::setMaterialsColorMap(const MaterialsColorMap colorMap)
{
    BRAYNS_FCT_ENTRY

    size_t materialId = 0;
    for (auto& material : _materialManager.getMaterials())
    {
        material.setSpecularColor(Vector3f(0.f, 0.f, 0.f));
        material.setOpacity(1.f);
        material.setReflectionIndex(0.f);

        switch (colorMap)
        {
        case MaterialsColorMap::none:
            switch (materialId)
            {
            case 0: // Default
            case 1: // Soma
                material.setDiffuseColor(Vector3f(0.9f, 0.9f, 0.9f));
                break;
            case 2: // Axon
                material.setDiffuseColor(Vector3f(0.2f, 0.2f, 0.8f));
                break;
            case 3: // Dendrite
                material.setDiffuseColor(Vector3f(0.8f, 0.2f, 0.2f));
                break;
            case 4: // Apical dendrite
                material.setDiffuseColor(Vector3f(0.8f, 0.2f, 0.8f));
                break;
            default:
                material.setDiffuseColor(
                    Vector3f(float(std::rand() % 255) / 255.f,
                             float(std::rand() % 255) / 255.f,
                             float(std::rand() % 255) / 255.f));
            }
            break;
        case MaterialsColorMap::gradient:
        {
            const auto nbMaterials = _materialManager.getMaterials().size();
            const float a = float(materialId) / float(nbMaterials);
            material.setDiffuseColor(Vector3f(a, 0.f, 1.f - a));
            break;
        }
        case MaterialsColorMap::pastel:
            material.setDiffuseColor(
                Vector3f(0.5f + float(std::rand() % 127) / 255.f,
                         0.5f + float(std::rand() % 127) / 255.f,
                         0.5f + float(std::rand() % 127) / 255.f));
            break;
        case MaterialsColorMap::random:
            material.setDiffuseColor(Vector3f(float(rand() % 255) / 255.f,
                                              float(rand() % 255) / 255.f,
                                              float(rand() % 255) / 255.f));
            switch (rand() % 4)
            {
            case 0:
                // Transparent
                material.setOpacity(float(std::rand() % 100) / 100.f);
                material.setRefractionIndex(0.98f);
                material.setSpecularColor(Vector3f(0.01f, 0.01f, 0.01f));
                material.setSpecularExponent(10.f);
            case 1:
                // Light emmitter
                material.setEmission(1.f);
            case 2:
                // Reflector
                material.setReflectionIndex(float(std::rand() % 100) / 100.f);
                material.setSpecularColor(Vector3f(0.01f, 0.01f, 0.01f));
                material.setSpecularExponent(10.f);
            }
            break;
        case MaterialsColorMap::shades_of_grey:
            float value = float(std::rand() % 255) / 255.f;
            material.setDiffuseColor(Vector3f(value, value, value));
            break;
        }
        ++materialId;
    }

    _materialManager.markModified();
}

void Scene::buildDefault()
{
    BRAYNS_FCT_ENTRY

    _markGeometryDirty();

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

    auto model = addModel("CornellBox");
    for (size_t i = 1; i < 6; ++i)
    {
        // Cornell box
        Material material;
        material.setDiffuseColor(colors[i]);
        material.setSpecularColor(WHITE);
        material.setSpecularExponent(10.f);
        material.setReflectionIndex(i == 4 ? 0.2f : 0.f);
        material.setGlossiness(i == 4 ? 0.9f : 1.f);
        material.setOpacity(1.f);
        material.setName("cornellbox_wall" + std::to_string(i));
        const auto materialId = _materialManager.add(material);

        auto& meshes = model->getTrianglesMeshes()[materialId];
        for (size_t j = 0; j < 6; ++j)
        {
            const auto position = positions[indices[i][j]];
            meshes.vertices.push_back(position);
            model->getBounds().merge(position);
        }
        meshes.indices.push_back(Vector3ui(0, 1, 2));
        meshes.indices.push_back(Vector3ui(3, 4, 5));
    }

    {
        // Sphere
        Material material;
        material.setOpacity(0.2f);
        material.setRefractionIndex(1.5f);
        material.setReflectionIndex(0.1f);
        material.setDiffuseColor(WHITE);
        material.setSpecularColor(WHITE);
        material.setSpecularExponent(100.f);
        material.setName("cornellbox_sphere");
        const auto materialId = _materialManager.add(material);
        model->addSphere(materialId, {{0.25f, 0.26f, 0.30f}, 0.25f});
    }

    {
        // Cylinder
        Material material;
        material.setDiffuseColor({0.1f, 0.1f, 0.8f});
        material.setSpecularColor(WHITE);
        material.setSpecularExponent(10.f);
        material.setName("cornellbox_cylinder");
        const auto materialId = _materialManager.add(material);
        model->addCylinder(materialId, {{0.25f, 0.126f, 0.75f},
                                        {0.75f, 0.126f, 0.75f},
                                        0.125f});
    }

    {
        // Cone
        Material material;
        material.setReflectionIndex(0.8f);
        material.setSpecularColor(WHITE);
        material.setSpecularExponent(10.f);
        material.setName("cornellbox_cone");
        const auto materialId = _materialManager.add(material);
        model->addCone(materialId, {{0.75f, 0.01f, 0.25f},
                                    {0.75f, 0.5f, 0.25f},
                                    0.15f,
                                    0.f});
    }

    {
        // Lamp
        Material material;
        material.setDiffuseColor(WHITE);
        material.setEmission(5.f);
        material.setName("cornellbox_lamp");
        const auto materialId = _materialManager.add(material);
        const Vector3f lampInfo = {0.15f, 0.99f, 0.15f};
        const Vector3f lampPositions[4] = {
            {0.5f - lampInfo.x(), lampInfo.y(), 0.5f - lampInfo.z()},
            {0.5f + lampInfo.x(), lampInfo.y(), 0.5f - lampInfo.z()},
            {0.5f + lampInfo.x(), lampInfo.y(), 0.5f + lampInfo.z()},
            {0.5f - lampInfo.x(), lampInfo.y(), 0.5f + lampInfo.z()}};
        auto& meshes = model->getTrianglesMeshes()[materialId];
        for (size_t i = 0; i < 4; ++i)
            meshes.vertices.push_back(lampPositions[i]);
        meshes.indices.push_back(Vector3i(2, 1, 0));
        meshes.indices.push_back(Vector3i(0, 3, 2));
    }
}

void Scene::buildEnvironment()
{
    BRAYNS_FCT_ENTRY

    const auto sceneEnvironment =
        _parametersManager.getGeometryParameters().getSceneEnvironment();
    if (sceneEnvironment == SceneEnvironment::none)
        return;

    auto model = addModel("scene_environment");
    const auto sceneBounds = getBounds();
    auto& meshes = model->getTrianglesMeshes();
    switch (sceneEnvironment)
    {
    case SceneEnvironment::ground:
    {
        // Ground
        const Vector3f scale(4.f, 0.5f, 4.f);
        const float tiles = 8.f;
        const float S = 0.5f * std::min(sceneBounds.getSize().x(),
                                        sceneBounds.getSize().z());
        const Vector3f s(S, sceneBounds.getSize().y(), S);
        const Vector3f c = sceneBounds.getCenter();

        Vector3i i;
        Material material;
        material.setDiffuseColor(Vector3f(1, 1, 1));
        material.setName("scene_ground");
        const auto materialId = _materialManager.add(material);
        size_t meshIndex = meshes[materialId].indices.size();

        Vector3f v;
        const Vector3f n(0.f, 1.f, 0.f);
        v = Vector3f(c.x() - s.x() * scale.x(),
                     c.y() - s.y() * scale.y() * 1.001f,
                     c.z() - s.z() * scale.z());
        meshes[materialId].vertices.push_back(v);
        meshes[materialId].normals.push_back(n);
        meshes[materialId].textureCoordinates.push_back(Vector2f(0.f, 0.f));
        v = Vector3f(c.x() + s.x() * scale.x(),
                     c.y() - s.y() * scale.y() * 1.001f,
                     c.z() - s.z() * scale.z());
        meshes[materialId].vertices.push_back(v);
        meshes[materialId].normals.push_back(n);
        meshes[materialId].textureCoordinates.push_back(Vector2f(tiles, 0.f));
        v = Vector3f(c.x() + s.x() * scale.x(),
                     c.y() - s.y() * scale.y() * 1.001f,
                     c.z() + s.z() * scale.z());
        meshes[materialId].vertices.push_back(v);
        meshes[materialId].normals.push_back(n);
        meshes[materialId].textureCoordinates.push_back(Vector2f(tiles, tiles));
        i = Vector3i(meshIndex, meshIndex + 1, meshIndex + 2);
        meshes[materialId].indices.push_back(i);
        meshIndex += 3;

        v = Vector3f(c.x() + s.x() * scale.x(),
                     c.y() - s.y() * scale.y() * 1.001f,
                     c.z() + s.z() * scale.z());
        meshes[materialId].vertices.push_back(v);
        meshes[materialId].normals.push_back(n);
        meshes[materialId].textureCoordinates.push_back(Vector2f(tiles, tiles));
        v = Vector3f(c.x() - s.x() * scale.x(),
                     c.y() - s.y() * scale.y() * 1.001f,
                     c.z() + s.z() * scale.z());
        meshes[materialId].vertices.push_back(v);
        meshes[materialId].normals.push_back(n);
        meshes[materialId].textureCoordinates.push_back(Vector2f(0.f, tiles));
        v = Vector3f(c.x() - s.x() * scale.x(),
                     c.y() - s.y() * scale.y() * 1.001f,
                     c.z() - s.z() * scale.z());
        meshes[materialId].vertices.push_back(v);
        meshes[materialId].normals.push_back(n);
        meshes[materialId].textureCoordinates.push_back(Vector2f(0.f, 0.f));
        i = Vector3i(meshIndex, meshIndex + 1, meshIndex + 2);
        meshes[materialId].indices.push_back(i);
        break;
    }
    case SceneEnvironment::wall:
    {
        // Wall
        const Vector3f scale(3.f, 3.f, 1.f);
        const float tiles = 4.f;
        const float S = 0.5f * std::min(sceneBounds.getSize().x(),
                                        sceneBounds.getSize().y());
        const Vector3f s(S, sceneBounds.getSize().z(), S);
        const Vector3f c = sceneBounds.getCenter();

        Material material;
        material.setDiffuseColor(Vector3f(1, 1, 1));
        material.setName("scene_wall");
        const auto materialId = _materialManager.add(material);

        Vector3f v;
        Vector3i i;
        const Vector3f n(0.f, 0.f, -1.f);
        v = Vector3f(c.x() - s.x() * scale.x(), c.y() - s.y() * scale.y(),
                     c.z() - s.z() * scale.z() * 1.001f);
        meshes[materialId].vertices.push_back(v);
        meshes[materialId].normals.push_back(n);
        meshes[materialId].textureCoordinates.push_back(Vector2f(0.f, 0.f));
        v = Vector3f(c.x() + s.x() * scale.x(), c.y() - s.y() * scale.y(),
                     c.z() - s.z() * scale.z() * 1.001f);
        meshes[materialId].vertices.push_back(v);
        meshes[materialId].normals.push_back(n);
        meshes[materialId].textureCoordinates.push_back(Vector2f(tiles, 0.f));
        v = Vector3f(c.x() + s.x() * scale.x(), c.y() + s.y() * scale.y(),
                     c.z() - s.z() * scale.z() * 1.001f);
        meshes[materialId].vertices.push_back(v);
        meshes[materialId].normals.push_back(n);
        meshes[materialId].textureCoordinates.push_back(Vector2f(tiles, tiles));
        i = Vector3i(0, 1, 2);
        meshes[materialId].indices.push_back(i);
        v = Vector3f(c.x() + s.x() * scale.x(), c.y() + s.y() * scale.y(),
                     c.z() - s.z() * scale.z() * 1.001f);
        meshes[materialId].vertices.push_back(v);
        meshes[materialId].normals.push_back(n);
        meshes[materialId].textureCoordinates.push_back(Vector2f(tiles, tiles));
        v = Vector3f(c.x() - s.x() * scale.x(), c.y() + s.y() * scale.y(),
                     c.z() - s.z() * scale.z() * 1.001f);
        meshes[materialId].vertices.push_back(v);
        meshes[materialId].normals.push_back(n);
        meshes[materialId].textureCoordinates.push_back(Vector2f(0.f, tiles));
        v = Vector3f(c.x() - s.x() * scale.x(), c.y() - s.y() * scale.y(),
                     c.z() - s.z() * scale.z() * 1.001f);
        meshes[materialId].vertices.push_back(v);
        meshes[materialId].normals.push_back(n);
        meshes[materialId].textureCoordinates.push_back(Vector2f(0.f, 0.f));
        i = Vector3i(3, 4, 5);
        meshes[materialId].indices.push_back(i);
        break;
    }
    case SceneEnvironment::bounding_box:
    {
        Material material;
        material.setDiffuseColor(Vector3f(1, 1, 1));
        material.setName("scene_bounding_box");
        const auto materialId = _materialManager.add(material);
        const Vector3f s = sceneBounds.getSize() / 2.f;
        const Vector3f c = sceneBounds.getCenter();
        const float radius = s.length() / 500.f;
        const Vector3f positions[8] = {
            {c.x() - s.x(), c.y() - s.y(), c.z() - s.z()},
            {c.x() + s.x(), c.y() - s.y(), c.z() - s.z()}, //    6--------7
            {c.x() - s.x(), c.y() + s.y(), c.z() - s.z()}, //   /|       /|
            {c.x() + s.x(), c.y() + s.y(), c.z() - s.z()}, //  2--------3 |
            {c.x() - s.x(), c.y() - s.y(), c.z() + s.z()}, //  | |      | |
            {c.x() + s.x(), c.y() - s.y(), c.z() + s.z()}, //  | 4------|-5
            {c.x() - s.x(), c.y() + s.y(), c.z() + s.z()}, //  |/       |/
            {c.x() + s.x(), c.y() + s.y(), c.z() + s.z()}  //  0--------1
        };

        for (size_t i = 0; i < 8; ++i)
            model->addSphere(materialId, Sphere(positions[i], radius));

        model->addCylinder(materialId, {positions[0], positions[1], radius});
        model->addCylinder(materialId, {positions[2], positions[3], radius});
        model->addCylinder(materialId, {positions[4], positions[5], radius});
        model->addCylinder(materialId, {positions[6], positions[7], radius});

        model->addCylinder(materialId, {positions[0], positions[2], radius});
        model->addCylinder(materialId, {positions[1], positions[3], radius});
        model->addCylinder(materialId, {positions[4], positions[6], radius});
        model->addCylinder(materialId, {positions[5], positions[7], radius});

        model->addCylinder(materialId, {positions[0], positions[4], radius});
        model->addCylinder(materialId, {positions[1], positions[5], radius});
        model->addCylinder(materialId, {positions[2], positions[6], radius});
        model->addCylinder(materialId, {positions[3], positions[7], radius});

        break;
    }
    default:
        break;
    }
}

void Scene::addLight(LightPtr light)
{
    BRAYNS_FCT_ENTRY

    removeLight(light);
    _lights.push_back(light);
}

void Scene::removeLight(LightPtr light)
{
    BRAYNS_FCT_ENTRY

    Lights::iterator it = std::find(_lights.begin(), _lights.end(), light);
    if (it != _lights.end())
        _lights.erase(it);
}

LightPtr Scene::getLight(const size_t index)
{
    BRAYNS_FCT_ENTRY

    if (index < _lights.size())
        return _lights[index];
    return 0;
}

void Scene::clearLights()
{
    BRAYNS_FCT_ENTRY

    _lights.clear();
}

void Scene::setSimulationHandler(AbstractSimulationHandlerPtr handler)
{
    BRAYNS_FCT_ENTRY

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
    BRAYNS_FCT_ENTRY

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

VolumeHandlerPtr Scene::getVolumeHandler()
{
    BRAYNS_FCT_ENTRY

    const auto& volumeFile =
        _parametersManager.getVolumeParameters().getFilename();
    const auto& volumeFolder =
        _parametersManager.getVolumeParameters().getFolder();
    if (volumeFile.empty() && volumeFolder.empty())
        return nullptr;

    try
    {
        if (!_volumeHandler)
        {
            _volumeHandler.reset(
                new VolumeHandler(_parametersManager.getVolumeParameters(),
                                  IndexMode::modulo));
            if (!volumeFile.empty())
            {
                if (!isVolumeSupported(volumeFile))
                {
                    _volumeHandler.reset();
                    return nullptr;
                }
                _volumeHandler->attachVolumeToFile(0.f, volumeFile);
            }
            else
            {
                strings filenames;

                boost::filesystem::directory_iterator endIter;
                if (boost::filesystem::is_directory(volumeFolder))
                {
                    for (boost::filesystem::directory_iterator dirIter(
                             volumeFolder);
                         dirIter != endIter; ++dirIter)
                    {
                        if (boost::filesystem::is_regular_file(
                                dirIter->status()))
                        {
                            const std::string& filename =
                                dirIter->path().string();
                            if (isVolumeSupported(filename))
                                filenames.push_back(filename);
                        }
                    }
                }

                if (filenames.empty())
                {
                    _volumeHandler.reset();
                    return nullptr;
                }

                std::sort(filenames.begin(), filenames.end());
                uint32_t index = 0;
                for (const auto& filename : filenames)
                    _volumeHandler->attachVolumeToFile(index++, filename);
            }

            // Add geometry group
            _volumeHandler->setCurrentIndex(0);
            const auto& dimensions = _volumeHandler->getDimensions();
            const auto& offset = _volumeHandler->getOffset();
            const auto name = getNameFromFullPath(volumeFile);
            auto model = addModel(name);
            model->getBounds().merge(offset);
            model->getBounds().merge(offset + dimensions);

            _parametersManager.getVolumeParameters().resetModified();
        }
    }
    catch (const std::runtime_error& e)
    {
        BRAYNS_ERROR << e.what() << std::endl;
    }

    if (_volumeHandler)
        _parametersManager.getAnimationParameters().setEnd(
            _volumeHandler->getNbFrames());
    else
        _parametersManager.getAnimationParameters().reset();

    return _volumeHandler;
}

bool Scene::empty() const
{
    BRAYNS_FCT_ENTRY

    bool empty = true;
    for (const auto& model : _models)
        empty = empty && model->empty();
    return empty;
}

void Scene::_processVolumeAABBGeometry()
{
    BRAYNS_FCT_ENTRY

    VolumeHandlerPtr volumeHandler = getVolumeHandler();
    if (!volumeHandler)
        return;

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
        {0, 1, 3, 3, 2, 0}, // Front
        {1, 5, 7, 7, 3, 1}, // Right
        {5, 4, 6, 6, 7, 5}, // Back
        {4, 0, 2, 2, 6, 4}, // Left
        {0, 1, 5, 5, 4, 0}, // Bottom
        {2, 3, 7, 7, 6, 2}  // Top
    };

    const auto animationFrame =
        _parametersManager.getAnimationParameters().getFrame();
    volumeHandler->setCurrentIndex(animationFrame);
    const Vector3f& volumeElementSpacing = volumeHandler->getElementSpacing();
    const Vector3f& volumeOffset = volumeHandler->getOffset();
    const Vector3ui& volumeDimensions = volumeHandler->getDimensions();

    auto model = addModel("VolumeContainer");
    Material material;
    material.setOpacity(0.f);
    material.setName("volume_container");
    const size_t materialId = _materialManager.add(material);
    auto& meshes = model->getTrianglesMeshes()[materialId];
    uint64_t offset = meshes.vertices.size();
    for (size_t face = 0; face < 6; ++face)
    {
        for (size_t index = 0; index < 6; ++index)
        {
            const Vector3f position = positions[indices[face][index]] *
                                          volumeElementSpacing *
                                          volumeDimensions +
                                      volumeOffset;

            meshes.vertices.push_back(position);
            model->getBounds().merge(position);
        }
        const size_t index = offset + face * 6;
        meshes.indices.push_back(Vector3ui(index + 0, index + 1, index + 2));
        meshes.indices.push_back(Vector3ui(index + 3, index + 4, index + 5));
    }

    model->getBounds().merge(Vector3f(0.f, 0.f, 0.f));
    model->getBounds().merge(volumeOffset +
                             Vector3f(volumeDimensions) * volumeElementSpacing);
}

void Scene::saveToCacheFile()
{
    BRAYNS_FCT_ENTRY

    const auto& filename =
        _parametersManager.getGeometryParameters().getSaveCacheFile();
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
    size_t nbElements = _models.size();
    file.write((char*)&nbElements, sizeof(size_t));
    for (auto model : _models)
    {
        const size_t nbMaterials = _materialManager.getMaterials().size();
        file.write((char*)&nbMaterials, sizeof(size_t));
        BRAYNS_INFO << nbMaterials << " materials" << std::endl;

        // Save materials
        for (auto& material : _materialManager.getMaterials())
        {
            Vector3f value3f;
            value3f = material.getDiffuseColor();
            file.write((char*)&value3f, sizeof(Vector3f));
            value3f = material.getSpecularColor();
            file.write((char*)&value3f, sizeof(Vector3f));
            float value = material.getSpecularExponent();
            file.write((char*)&value, sizeof(float));
            value = material.getReflectionIndex();
            file.write((char*)&value, sizeof(float));
            value = material.getOpacity();
            file.write((char*)&value, sizeof(float));
            value = material.getRefractionIndex();
            file.write((char*)&value, sizeof(float));
            value = material.getEmission();
            file.write((char*)&value, sizeof(float));
            value = material.getGlossiness();
            file.write((char*)&value, sizeof(float));
            const bool boolean = material.getCastSimulationData();
            file.write((char*)&boolean, sizeof(bool));
            // TODO: Textures
        }

        for (size_t materialId = 0; materialId < nbMaterials; ++materialId)
        {
            uint64_t bufferSize{0};

            // Spheres
            auto& spheres = model->getSpheres();
            if (spheres.find(materialId) != spheres.end())
            {
                auto& data = spheres[materialId];
                nbElements = data.size();
                file.write((char*)&nbElements, sizeof(size_t));
                bufferSize = nbElements * sizeof(Sphere);
                file.write((char*)data.data(), bufferSize);
                BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                             << " spheres" << std::endl;
            }
            else
            {
                nbElements = 0;
                file.write((char*)&nbElements, sizeof(size_t));
            }

            // Cylinders
            auto& cylinders = model->getCylinders();
            if (cylinders.find(materialId) != cylinders.end())
            {
                auto& data = cylinders[materialId];
                nbElements = data.size();
                file.write((char*)&nbElements, sizeof(size_t));
                bufferSize = nbElements * sizeof(Cylinder);
                file.write((char*)data.data(), bufferSize);
                BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                             << " cylinders" << std::endl;
            }
            else
            {
                nbElements = 0;
                file.write((char*)&nbElements, sizeof(size_t));
            }

            // Cones
            auto& cones = model->getCones();
            if (cones.find(materialId) != cones.end())
            {
                auto& data = cones[materialId];
                nbElements = data.size();
                file.write((char*)&nbElements, sizeof(size_t));
                bufferSize = nbElements * sizeof(Cone);
                file.write((char*)data.data(), bufferSize);
                BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                             << " cones" << std::endl;
            }
            else
            {
                nbElements = 0;
                file.write((char*)&nbElements, sizeof(size_t));
            }

            auto& meshes = model->getTrianglesMeshes();
            if (meshes.find(materialId) != meshes.end())
            {
                auto& data = meshes[materialId];
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
            else
            {
                BRAYNS_FCT_ENTRY nbElements = 0;
                file.write((char*)&nbElements, sizeof(size_t)); // No vertices
                file.write((char*)&nbElements, sizeof(size_t)); // No indices
                file.write((char*)&nbElements, sizeof(size_t)); // No normals
                file.write((char*)&nbElements,
                           sizeof(size_t)); // No Texture coordinates
            }
        }

        // Bounds
        const auto& bounds = model->getBounds();
        file.write((char*)&bounds, sizeof(Boxf));
        BRAYNS_DEBUG << "AABB: " << bounds << std::endl;
    }

    file.close();

    BRAYNS_INFO << "Scene successfully saved" << std::endl;
}

void Scene::loadFromCacheFile()
{
    BRAYNS_FCT_ENTRY

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

    size_t nbMaterials;
    file.read((char*)&nbMaterials, sizeof(size_t));
    BRAYNS_INFO << nbMaterials << " materials" << std::endl;

    // Geometry
    size_t nbGeometryGroups = 0;
    size_t nbSpheres = 0;
    size_t nbCylinders = 0;
    size_t nbCones = 0;
    size_t nbVertices = 0;
    size_t nbIndices = 0;
    size_t nbNormals = 0;
    size_t nbTexCoords = 0;

    file.read((char*)&nbGeometryGroups, sizeof(size_t));
    for (size_t groupId = 0; groupId < nbGeometryGroups; ++groupId)
    {
        auto model = addModel("ToBeDefined");
        // Materials
        _materialManager.clear();
        for (size_t i = 0; i < nbMaterials; ++i)
        {
            auto& material = _materialManager.get(i);
            Vector3f value3f;
            file.read((char*)&value3f, sizeof(Vector3f));
            material.setDiffuseColor(value3f);
            file.read((char*)&value3f, sizeof(Vector3f));
            material.setSpecularColor(value3f);
            float value;
            file.read((char*)&value, sizeof(float));
            material.setSpecularExponent(value);
            file.read((char*)&value, sizeof(float));
            material.setReflectionIndex(value);
            file.read((char*)&value, sizeof(float));
            material.setOpacity(value);
            file.read((char*)&value, sizeof(float));
            material.setRefractionIndex(value);
            file.read((char*)&value, sizeof(float));
            material.setEmission(value);
            file.read((char*)&value, sizeof(float));
            material.setGlossiness(value);
            bool boolean;
            file.read((char*)&boolean, sizeof(bool));
            material.setCastSimulationData(boolean);
            // TODO: Textures
        }

        for (size_t materialId = 0; materialId < nbMaterials; ++materialId)
        {
            uint64_t bufferSize{0};

            // Spheres
            file.read((char*)&nbSpheres, sizeof(size_t));
            if (nbSpheres != 0)
            {
                bufferSize = nbSpheres * sizeof(Sphere);
                BRAYNS_DEBUG << "[" << materialId << "] " << nbSpheres
                             << " spheres" << std::endl;
                auto& spheres = model->getSpheres()[materialId];
                spheres.resize(nbSpheres);
                file.read((char*)spheres.data(), bufferSize);
            }

            // Cylinders
            file.read((char*)&nbCylinders, sizeof(size_t));
            if (nbCylinders != 0)
            {
                bufferSize = nbCylinders * sizeof(Cylinder);
                BRAYNS_DEBUG << "[" << materialId << "] " << nbCylinders
                             << " cylinders" << std::endl;
                auto& cylinders = model->getCylinders()[materialId];
                cylinders.resize(nbCylinders);
                file.read((char*)cylinders.data(), bufferSize);
            }

            // Cones
            file.read((char*)&nbCones, sizeof(size_t));
            if (nbCones != 0)
            {
                bufferSize = nbCones * sizeof(Cone);
                BRAYNS_DEBUG << "[" << materialId << "] " << nbCones << " cones"
                             << std::endl;
                auto& cones = model->getCones()[materialId];
                cones.resize(nbCones);
                file.read((char*)cones.data(), bufferSize);
            }

            // Meshes
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

            BRAYNS_INFO << "[" << materialId << "] " << nbSpheres
                        << " spheres, " << nbCylinders << " cylinders, "
                        << nbCones << " cones, " << nbVertices << " vertices, "
                        << nbIndices << " indices, " << nbNormals
                        << " normals, " << nbTexCoords << " texture coordinates"
                        << std::endl;
        }

        // Bounds
        Boxf bounds;
        file.read((char*)&bounds, sizeof(Boxf));
        model->getBounds().merge(bounds);
        BRAYNS_DEBUG << "AABB: " << bounds << std::endl;
    }
    file.close();

    BRAYNS_INFO << "Scene successfully loaded" << std::endl;
}

Boxf& Scene::getBounds()
{
    BRAYNS_FCT_ENTRY

    _bounds.reset();
    for (size_t i = 0; i < _models.size(); ++i)
    {
        const auto& modelDescriptor = _modelDescriptors[i];
        if (modelDescriptor.enabled())
            _bounds.merge(_models[i]->getBounds());
    }
    return _bounds;
}
}
