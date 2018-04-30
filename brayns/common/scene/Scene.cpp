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

#include <brayns/common/log.h>
#include <brayns/common/material/Material.h>
#include <brayns/common/volume/VolumeHandler.h>
#include <brayns/io/NESTLoader.h>
#include <brayns/io/TransferFunctionLoader.h>
#include <brayns/io/simulation/CADiffusionSimulationHandler.h>
#include <brayns/parameters/ParametersManager.h>

#include <boost/filesystem.hpp>
#include <fstream>

namespace
{
const size_t CACHE_VERSION = 8;
}

namespace brayns
{
Scene::Scene(Renderers renderers, ParametersManager& parametersManager)
    : _parametersManager(parametersManager)
    , _renderers(renderers)
    , _spheresDirty(true)
    , _cylindersDirty(true)
    , _conesDirty(true)
    , _trianglesMeshesDirty(true)
    , _volumeHandler(nullptr)
    , _simulationHandler(nullptr)
    , _caDiffusionSimulationHandler(nullptr)
{
}

Scene::~Scene()
{
}

void Scene::reset()
{
    unload();
    _renderers.clear();
}

void Scene::unload()
{
    _markGeometryDirty();
    _spheres.clear();
    _cylinders.clear();
    _cones.clear();
    _trianglesMeshes.clear();
    _bounds.reset();
    _caDiffusionSimulationHandler.reset();
    _simulationHandler.reset();
    _materials.clear();
    _textures.clear();
    _volumeHandler.reset();
}

void Scene::_markGeometryDirty()
{
    _spheresDirty = true;
    _cylindersDirty = true;
    _conesDirty = true;
    _trianglesMeshesDirty = true;
    markModified();
}

void Scene::resetMaterials()
{
    BRAYNS_INFO << "Building system materials" << std::endl;
    _materials.clear();
    for (size_t i = 0; i < NB_SYSTEM_MATERIALS; ++i)
    {
        Material material;
        switch (MaterialType(i))
        {
        case MaterialType::bounding_box:
            material.setColor(Vector3f(1.f, 1.f, 1.f));
            material.setEmission(10.f);
            break;
        case MaterialType::invisible:
            material.setOpacity(0.f);
            material.setRefractionIndex(1.f);
            material.setColor(Vector3f(1.f, 1.f, 1.f));
            material.setSpecularColor(Vector3f(0.f, 0.f, 0.f));
            break;
        default:
            break;
        }
        _materials.push_back(material);
    }
}

void Scene::setMaterialsColorMap(const MaterialsColorMap colorMap)
{
    const auto nbMaterials = _materials.size();
    for (size_t i = NB_SYSTEM_MATERIALS; i < nbMaterials; ++i)
    {
        auto& material = _materials[i];
        material.setSpecularColor(Vector3f(0.f, 0.f, 0.f));
        material.setOpacity(1.f);
        material.setReflectionIndex(0.f);

        switch (colorMap)
        {
        case MaterialsColorMap::none:
            switch (i)
            {
            case 0: // Default
            case 1: // Soma
                material.setColor(Vector3f(0.9f, 0.9f, 0.9f));
                break;
            case 2: // Axon
                material.setColor(Vector3f(0.2f, 0.2f, 0.8f));
                break;
            case 3: // Dendrite
                material.setColor(Vector3f(0.8f, 0.2f, 0.2f));
                break;
            case 4: // Apical dendrite
                material.setColor(Vector3f(0.8f, 0.2f, 0.8f));
                break;
            default:
                material.setColor(Vector3f(float(std::rand() % 255) / 255.f,
                                           float(std::rand() % 255) / 255.f,
                                           float(std::rand() % 255) / 255.f));
            }
            break;
        case MaterialsColorMap::gradient:
        {
            const float a = float(i) / float(nbMaterials);
            material.setColor(Vector3f(a, 0.f, 1.f - a));
            break;
        }
        case MaterialsColorMap::pastel:
            material.setColor(
                Vector3f(0.5f + float(std::rand() % 127) / 255.f,
                         0.5f + float(std::rand() % 127) / 255.f,
                         0.5f + float(std::rand() % 127) / 255.f));
            break;
        case MaterialsColorMap::random:
            material.setColor(Vector3f(float(rand() % 255) / 255.f,
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
                break;
            case 1:
                // Light emmitter
                material.setEmission(1.f);
                break;
            case 2:
                // Reflector
                material.setReflectionIndex(float(std::rand() % 100) / 100.f);
                material.setSpecularColor(Vector3f(0.01f, 0.01f, 0.01f));
                material.setSpecularExponent(10.f);
                break;
            }
            break;
        case MaterialsColorMap::shades_of_grey:
            float value = float(std::rand() % 255) / 255.f;
            material.setColor(Vector3f(value, value, value));
            break;
        }
        _materials[i] = material;
    }
    commitMaterials(Action::update);
}

void Scene::load(Blob&& blob, const Matrix4f& transformation,
                 const size_t materialID, Loader::UpdateCallback cb)
{
    _loaderRegistry.load(std::move(blob), *this, transformation, materialID,
                         cb);
}

void Scene::load(const std::string& filename, const Matrix4f& transformation,
                 const size_t materialID, Loader::UpdateCallback cb)
{
    _loaderRegistry.load(filename, *this, transformation, materialID, cb);
}

Material& Scene::getMaterial(size_t index)
{
    _buildMissingMaterials(index);
    return _materials[index];
}

void Scene::buildDefault()
{
    BRAYNS_INFO << "Building default Cornell Box scene" << std::endl;

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

    for (size_t i = 1; i < 6; ++i)
    {
        // Cornell box
        Material material;
        material.setColor(colors[i]);
        material.setSpecularColor(WHITE);
        material.setSpecularExponent(10.f);
        material.setReflectionIndex(i == 4 ? 0.2f : 0.f);
        material.setGlossiness(i == 4 ? 0.9f : 1.f);
        material.setOpacity(1.f);
        const auto materialId = addMaterial(material);

        auto& trianglesMesh = _trianglesMeshes[materialId];
        for (size_t j = 0; j < 6; ++j)
        {
            const auto position = positions[indices[i][j]];
            trianglesMesh.vertices.push_back(position);
            _bounds.merge(position);
        }
        trianglesMesh.indices.push_back(Vector3ui(0, 1, 2));
        trianglesMesh.indices.push_back(Vector3ui(3, 4, 5));
    }

    {
        // Sphere
        Material material;
        material.setOpacity(0.2f);
        material.setRefractionIndex(1.5f);
        material.setReflectionIndex(0.1f);
        material.setColor(WHITE);
        material.setSpecularColor(WHITE);
        material.setSpecularExponent(100.f);
        const auto materialId = addMaterial(material);
        addSphere(materialId, {{0.25f, 0.26f, 0.30f}, 0.25f});
    }

    {
        // Cylinder
        Material material;
        material.setColor({0.1f, 0.1f, 0.8f});
        material.setSpecularColor(WHITE);
        material.setSpecularExponent(10.f);
        const auto materialId = addMaterial(material);
        addCylinder(materialId,
                    {{0.25f, 0.126f, 0.75f}, {0.75f, 0.126f, 0.75f}, 0.125f});
    }

    {
        // Cone
        Material material;
        material.setReflectionIndex(0.8f);
        material.setSpecularColor(WHITE);
        material.setSpecularExponent(10.f);
        const auto materialId = addMaterial(material);
        addCone(materialId,
                {{0.75f, 0.01f, 0.25f}, {0.75f, 0.5f, 0.25f}, 0.15f, 0.f});
    }

    {
        // Lamp
        Material material;
        material.setColor(WHITE);
        material.setEmission(5.f);
        const auto materialId = addMaterial(material);
        const Vector3f lampInfo = {0.15f, 0.99f, 0.15f};
        const Vector3f lampPositions[4] = {
            {0.5f - lampInfo.x(), lampInfo.y(), 0.5f - lampInfo.z()},
            {0.5f + lampInfo.x(), lampInfo.y(), 0.5f - lampInfo.z()},
            {0.5f + lampInfo.x(), lampInfo.y(), 0.5f + lampInfo.z()},
            {0.5f - lampInfo.x(), lampInfo.y(), 0.5f + lampInfo.z()}};
        for (size_t i = 0; i < 4; ++i)
            _trianglesMeshes[materialId].vertices.push_back(lampPositions[i]);
        _trianglesMeshes[materialId].indices.push_back(Vector3i(2, 1, 0));
        _trianglesMeshes[materialId].indices.push_back(Vector3i(0, 3, 2));
    }
    BRAYNS_INFO << "Bounding Box: " << _bounds << std::endl;
}

void Scene::buildEnvironment()
{
    switch (_parametersManager.getGeometryParameters().getSceneEnvironment())
    {
    case SceneEnvironment::none:
        break;
    case SceneEnvironment::ground:
    {
        // Ground
        const Vector3f scale(4.f, 0.5f, 4.f);
        const float tiles = 8.f;
        const float S =
            0.5f * std::min(_bounds.getSize().x(), _bounds.getSize().z());
        const Vector3f s(S, _bounds.getSize().y(), S);
        const Vector3f c = _bounds.getCenter();

        Vector3i i;
        const size_t material = 0;
        size_t meshIndex = _trianglesMeshes[material].indices.size();

        Vector4f v;
        const Vector4f n(0.f, 1.f, 0.f, 0.f);
        v = Vector4f(c.x() - s.x() * scale.x(),
                     c.y() - s.y() * scale.y() * 1.001f,
                     c.z() - s.z() * scale.z(), 0.f);
        _trianglesMeshes[material].vertices.push_back(v);
        _trianglesMeshes[material].normals.push_back(n);
        _trianglesMeshes[material].textureCoordinates.push_back(
            Vector2f(0.f, 0.f));
        v = Vector4f(c.x() + s.x() * scale.x(),
                     c.y() - s.y() * scale.y() * 1.001f,
                     c.z() - s.z() * scale.z(), 0.f);
        _trianglesMeshes[material].vertices.push_back(v);
        _trianglesMeshes[material].normals.push_back(n);
        _trianglesMeshes[material].textureCoordinates.push_back(
            Vector2f(tiles, 0.f));
        v = Vector4f(c.x() + s.x() * scale.x(),
                     c.y() - s.y() * scale.y() * 1.001f,
                     c.z() + s.z() * scale.z(), 0.f);
        _trianglesMeshes[material].vertices.push_back(v);
        _trianglesMeshes[material].normals.push_back(n);
        _trianglesMeshes[material].textureCoordinates.push_back(
            Vector2f(tiles, tiles));
        i = Vector3i(meshIndex, meshIndex + 1, meshIndex + 2);
        _trianglesMeshes[material].indices.push_back(i);
        meshIndex += 3;

        v = Vector4f(c.x() + s.x() * scale.x(),
                     c.y() - s.y() * scale.y() * 1.001f,
                     c.z() + s.z() * scale.z(), 0.f);
        _trianglesMeshes[material].vertices.push_back(v);
        _trianglesMeshes[material].normals.push_back(n);
        _trianglesMeshes[material].textureCoordinates.push_back(
            Vector2f(tiles, tiles));
        v = Vector4f(c.x() - s.x() * scale.x(),
                     c.y() - s.y() * scale.y() * 1.001f,
                     c.z() + s.z() * scale.z(), 0.f);
        _trianglesMeshes[material].vertices.push_back(v);
        _trianglesMeshes[material].normals.push_back(n);
        _trianglesMeshes[material].textureCoordinates.push_back(
            Vector2f(0.f, tiles));
        v = Vector4f(c.x() - s.x() * scale.x(),
                     c.y() - s.y() * scale.y() * 1.001f,
                     c.z() - s.z() * scale.z(), 0.f);
        _trianglesMeshes[material].vertices.push_back(v);
        _trianglesMeshes[material].normals.push_back(n);
        _trianglesMeshes[material].textureCoordinates.push_back(
            Vector2f(0.f, 0.f));
        i = Vector3i(meshIndex, meshIndex + 1, meshIndex + 2);
        _trianglesMeshes[material].indices.push_back(i);
        break;
    }
    case SceneEnvironment::wall:
    {
        // Wall
        const Vector3f scale(1.f, 1.f, 1.f);
        const float tiles = 4.f;
        const float S =
            0.5f * std::min(_bounds.getSize().x(), _bounds.getSize().z());
        const Vector3f s(S, _bounds.getSize().y(), S);
        const Vector3f c = _bounds.getCenter();
        Vector3i i;
        const size_t material = 0;
        size_t meshIndex = _trianglesMeshes[material].indices.size();
        Vector4f v;
        const Vector4f n(0.f, 0.f, -1.f, 0.f);
        v = Vector4f(c.x() - s.x() * scale.x(), c.y() - s.y() * scale.y(),
                     c.z() + s.z() * scale.z() * 1.001f, 0.f);
        _trianglesMeshes[material].vertices.push_back(v);
        _trianglesMeshes[material].normals.push_back(n);
        _trianglesMeshes[material].textureCoordinates.push_back(
            Vector2f(0.f, 0.f));
        v = Vector4f(c.x() + s.x() * scale.x(), c.y() - s.y() * scale.y(),
                     c.z() + s.z() * scale.z() * 1.001f, 0.f);
        _trianglesMeshes[material].vertices.push_back(v);
        _trianglesMeshes[material].normals.push_back(n);
        _trianglesMeshes[material].textureCoordinates.push_back(
            Vector2f(tiles, 0.f));
        v = Vector4f(c.x() + s.x() * scale.x(), c.y() + s.y() * scale.y(),
                     c.z() + s.z() * scale.z() * 1.001f, 0.f);
        _trianglesMeshes[material].vertices.push_back(v);
        _trianglesMeshes[material].normals.push_back(n);
        _trianglesMeshes[material].textureCoordinates.push_back(
            Vector2f(tiles, tiles));
        i = Vector3i(meshIndex, meshIndex + 1, meshIndex + 2);
        _trianglesMeshes[material].indices.push_back(i);
        meshIndex += 3;
        v = Vector4f(c.x() + s.x() * scale.x(), c.y() + s.y() * scale.y(),
                     c.z() + s.z() * scale.z() * 1.001f, 0.f);
        _trianglesMeshes[material].vertices.push_back(v);
        _trianglesMeshes[material].normals.push_back(n);
        _trianglesMeshes[material].textureCoordinates.push_back(
            Vector2f(tiles, tiles));
        v = Vector4f(c.x() - s.x() * scale.x(), c.y() + s.y() * scale.y(),
                     c.z() + s.z() * scale.z() * 1.001f, 0.f);
        _trianglesMeshes[material].vertices.push_back(v);
        _trianglesMeshes[material].normals.push_back(n);
        _trianglesMeshes[material].textureCoordinates.push_back(
            Vector2f(0.f, tiles));
        v = Vector4f(c.x() - s.x() * scale.x(), c.y() - s.y() * scale.y(),
                     c.z() + s.z() * scale.z() * 1.001f, 0.f);
        _trianglesMeshes[material].vertices.push_back(v);
        _trianglesMeshes[material].normals.push_back(n);
        _trianglesMeshes[material].textureCoordinates.push_back(
            Vector2f(0.f, 0.f));
        i = Vector3i(meshIndex, meshIndex + 1, meshIndex + 2);
        _trianglesMeshes[material].indices.push_back(i);
        break;
    }
    case SceneEnvironment::bounding_box:
    {
        const size_t material = static_cast<size_t>(MaterialType::bounding_box);
        const Vector3f s = _bounds.getSize() / 2.f;
        const Vector3f c = _bounds.getCenter();
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
            addSphere(material, Sphere(positions[i], radius));

        addCylinder(material, {positions[0], positions[1], radius});
        addCylinder(material, {positions[2], positions[3], radius});
        addCylinder(material, {positions[4], positions[5], radius});
        addCylinder(material, {positions[6], positions[7], radius});

        addCylinder(material, {positions[0], positions[2], radius});
        addCylinder(material, {positions[1], positions[3], radius});
        addCylinder(material, {positions[4], positions[6], radius});
        addCylinder(material, {positions[5], positions[7], radius});

        addCylinder(material, {positions[0], positions[4], radius});
        addCylinder(material, {positions[1], positions[5], radius});
        addCylinder(material, {positions[2], positions[6], radius});
        addCylinder(material, {positions[3], positions[7], radius});

        break;
    }
    }
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

VolumeHandlerPtr Scene::getVolumeHandler() const
{
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
    return _spheres.empty() && _cylinders.empty() && _cones.empty() &&
           _trianglesMeshes.empty() && !getVolumeHandler();
}

void Scene::_buildMissingMaterials(const size_t materialId)
{
    if (materialId >= _materials.size())
        _materials.resize(materialId + 1);
}

void Scene::_processVolumeAABBGeometry()
{
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

    const size_t materialId = static_cast<size_t>(MaterialType::invisible);
    uint64_t offset = _trianglesMeshes[materialId].vertices.size();
    for (size_t face = 0; face < 6; ++face)
    {
        for (size_t index = 0; index < 6; ++index)
        {
            const Vector3f position = positions[indices[face][index]] *
                                          volumeElementSpacing *
                                          volumeDimensions +
                                      volumeOffset;

            _trianglesMeshes[materialId].vertices.push_back(position);
            _bounds.merge(position);
        }
        const size_t index = offset + face * 6;
        _trianglesMeshes[materialId].indices.push_back(
            Vector3ui(index + 0, index + 1, index + 2));
        _trianglesMeshes[materialId].indices.push_back(
            Vector3ui(index + 3, index + 4, index + 5));
    }
}

uint64_t Scene::addSphere(const size_t materialId, const Sphere& sphere)
{
    _buildMissingMaterials(materialId);
    _spheres[materialId].push_back(sphere);
    _bounds.merge(sphere.center);
    return _spheres[materialId].size() - 1;
}

uint64_t Scene::addCylinder(const size_t materialId, const Cylinder& cylinder)
{
    _buildMissingMaterials(materialId);
    _cylinders[materialId].push_back(cylinder);
    _bounds.merge(cylinder.center);
    _bounds.merge(cylinder.up);
    return _cylinders[materialId].size() - 1;
}

uint64_t Scene::addCone(const size_t materialId, const Cone& cone)
{
    _buildMissingMaterials(materialId);
    _cones[materialId].push_back(cone);
    _bounds.merge(cone.center);
    _bounds.merge(cone.up);
    return _cones[materialId].size() - 1;
}

void Scene::setSphere(const size_t materialId, const uint64_t index,
                      const Sphere& sphere)
{
    auto& spheres = _spheres[materialId];
    if (index < spheres.size())
    {
        _buildMissingMaterials(materialId);
        spheres[index] = sphere;
        _bounds.merge(sphere.center);
    }
    else
        BRAYNS_ERROR << "Invalid index " << index << std::endl;
}

void Scene::setCone(const size_t materialId, const uint64_t index,
                    const Cone& cone)
{
    auto& cones = _cones[materialId];
    if (index < cones.size())
    {
        _buildMissingMaterials(materialId);
        cones[index] = cone;
        _bounds.merge(cone.center);
        _bounds.merge(cone.up);
    }
    else
        BRAYNS_ERROR << "Invalid index " << index << std::endl;
}

void Scene::setCylinder(const size_t materialId, const uint64_t index,
                        const Cylinder& cylinder)
{
    auto& cylinders = _cylinders[materialId];
    if (index < cylinders.size())
    {
        _buildMissingMaterials(materialId);
        cylinders[index] = cylinder;
        _bounds.merge(cylinder.center);
        _bounds.merge(cylinder.up);
    }
    else
        BRAYNS_ERROR << "Invalid index " << index << std::endl;
}

void Scene::saveToCacheFile()
{
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

    const size_t nbMaterials = _materials.size();
    file.write((char*)&nbMaterials, sizeof(size_t));
    BRAYNS_INFO << nbMaterials << " materials" << std::endl;

    // Save materials
    for (auto& material : _materials)
    {
        Vector3f value3f;
        value3f = material.getColor();
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

    // Save geometry
    for (size_t materialId = 0; materialId < nbMaterials; ++materialId)
    {
        size_t nbElements{0};
        uint64_t bufferSize{0};

        // Spheres
        if (_spheres.find(materialId) != _spheres.end())
        {
            const auto& spheres = _spheres[materialId];
            nbElements = spheres.size();
            file.write((char*)&nbElements, sizeof(size_t));
            bufferSize = nbElements * sizeof(Sphere);
            file.write((char*)spheres.data(), bufferSize);
            BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                         << " spheres" << std::endl;
        }
        else
        {
            nbElements = 0;
            file.write((char*)&nbElements, sizeof(size_t));
        }

        // Cylinders
        if (_cylinders.find(materialId) != _cylinders.end())
        {
            const auto& cylinders = _cylinders[materialId];
            nbElements = cylinders.size();
            file.write((char*)&nbElements, sizeof(size_t));
            bufferSize = nbElements * sizeof(Cylinder);
            file.write((char*)cylinders.data(), bufferSize);
            BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                         << " cylinders" << std::endl;
        }
        else
        {
            nbElements = 0;
            file.write((char*)&nbElements, sizeof(size_t));
        }

        // Cones
        if (_cones.find(materialId) != _cones.end())
        {
            const auto& cones = _cones[materialId];
            nbElements = cones.size();
            file.write((char*)&nbElements, sizeof(size_t));
            bufferSize = nbElements * sizeof(Cone);
            file.write((char*)cones.data(), bufferSize);
            BRAYNS_DEBUG << "[" << materialId << "] " << nbElements << " cones"
                         << std::endl;
        }
        else
        {
            nbElements = 0;
            file.write((char*)&nbElements, sizeof(size_t));
        }

        if (_trianglesMeshes.find(materialId) != _trianglesMeshes.end())
        {
            const auto& triangesMesh = _trianglesMeshes[materialId];
            // Vertices
            nbElements = triangesMesh.vertices.size();
            file.write((char*)&nbElements, sizeof(size_t));
            bufferSize = nbElements * sizeof(Vector3f);
            file.write((char*)triangesMesh.vertices.data(), bufferSize);
            if (nbElements != 0)
                BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                             << " vertices" << std::endl;

            // Indices
            nbElements = triangesMesh.indices.size();
            file.write((char*)&nbElements, sizeof(size_t));
            bufferSize = nbElements * sizeof(Vector3ui);
            file.write((char*)triangesMesh.indices.data(), bufferSize);
            if (nbElements != 0)
                BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                             << " indices" << std::endl;

            // Normals
            nbElements = triangesMesh.normals.size();
            file.write((char*)&nbElements, sizeof(size_t));
            bufferSize = nbElements * sizeof(Vector3f);
            file.write((char*)triangesMesh.normals.data(), bufferSize);
            if (nbElements != 0)
                BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                             << " normals" << std::endl;

            // Texture coordinates
            nbElements = triangesMesh.textureCoordinates.size();
            file.write((char*)&nbElements, sizeof(size_t));
            bufferSize = nbElements * sizeof(Vector2f);
            file.write((char*)triangesMesh.textureCoordinates.data(),
                       bufferSize);
            if (nbElements != 0)
                BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                             << " texture coordinates" << std::endl;
        }
        else
        {
            nbElements = 0;
            file.write((char*)&nbElements, sizeof(size_t)); // No vertices
            file.write((char*)&nbElements, sizeof(size_t)); // No indices
            file.write((char*)&nbElements, sizeof(size_t)); // No normals
            file.write((char*)&nbElements,
                       sizeof(size_t)); // No Texture coordinates
        }
    }

    // Scene bounds
    file.write((char*)&_bounds, sizeof(Boxf));
    BRAYNS_DEBUG << "AABB: " << _bounds << std::endl;
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

    size_t nbMaterials;
    file.read((char*)&nbMaterials, sizeof(size_t));
    BRAYNS_INFO << nbMaterials << " materials" << std::endl;

    // Materials
    _materials.clear();
    _materials.resize(nbMaterials);
    for (size_t i = 0; i < nbMaterials; ++i)
    {
        auto& material = _materials[i];
        Vector3f value3f;
        file.read((char*)&value3f, sizeof(Vector3f));
        material.setColor(value3f);
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

    // Geometry
    size_t nbSpheres = 0;
    size_t nbCylinders = 0;
    size_t nbCones = 0;
    size_t nbVertices = 0;
    size_t nbIndices = 0;
    size_t nbNormals = 0;
    size_t nbTexCoords = 0;
    for (size_t materialId = 0; materialId < nbMaterials; ++materialId)
    {
        uint64_t bufferSize{0};

        // Spheres
        file.read((char*)&nbSpheres, sizeof(size_t));
        if (nbSpheres != 0)
        {
            bufferSize = nbSpheres * sizeof(Sphere);
            BRAYNS_DEBUG << "[" << materialId << "] " << nbSpheres << " spheres"
                         << std::endl;
            _spheres[materialId].resize(nbSpheres);
            file.read((char*)_spheres[materialId].data(), bufferSize);
        }

        // Cylinders
        file.read((char*)&nbCylinders, sizeof(size_t));
        if (nbCylinders != 0)
        {
            bufferSize = nbCylinders * sizeof(Cylinder);
            BRAYNS_DEBUG << "[" << materialId << "] " << nbCylinders
                         << " cylinders" << std::endl;
            _cylinders[materialId].resize(nbCylinders);
            file.read((char*)_cylinders[materialId].data(), bufferSize);
        }

        // Cones
        file.read((char*)&nbCones, sizeof(size_t));
        if (nbCones != 0)
        {
            bufferSize = nbCones * sizeof(Cone);
            BRAYNS_DEBUG << "[" << materialId << "] " << nbCones << " cones"
                         << std::endl;
            _cones[materialId].resize(nbCones);
            file.read((char*)_cones[materialId].data(), bufferSize);
        }

        // Vertices
        file.read((char*)&nbVertices, sizeof(size_t));
        if (nbVertices != 0)
        {
            BRAYNS_DEBUG << "[" << materialId << "] " << nbVertices
                         << " vertices" << std::endl;
            _trianglesMeshes[materialId].vertices.resize(nbVertices);
            bufferSize = nbVertices * sizeof(Vector3f);
            file.read((char*)_trianglesMeshes[materialId].vertices.data(),
                      bufferSize);
        }

        // Indices
        file.read((char*)&nbIndices, sizeof(size_t));
        if (nbIndices != 0)
        {
            BRAYNS_DEBUG << "[" << materialId << "] " << nbIndices << " indices"
                         << std::endl;
            _trianglesMeshes[materialId].indices.resize(nbIndices);
            bufferSize = nbIndices * sizeof(Vector3ui);
            file.read((char*)_trianglesMeshes[materialId].indices.data(),
                      bufferSize);
        }

        // Normals
        file.read((char*)&nbNormals, sizeof(size_t));
        if (nbNormals != 0)
        {
            BRAYNS_DEBUG << "[" << materialId << "] " << nbNormals << " normals"
                         << std::endl;
            _trianglesMeshes[materialId].normals.resize(nbNormals);
            bufferSize = nbNormals * sizeof(Vector3f);
            file.read((char*)_trianglesMeshes[materialId].normals.data(),
                      bufferSize);
        }

        // Texture coordinates
        file.read((char*)&nbTexCoords, sizeof(size_t));
        if (nbTexCoords != 0)
        {
            BRAYNS_DEBUG << "Material " << materialId << ": " << nbTexCoords
                         << " texture coordinates" << std::endl;
            _trianglesMeshes[materialId].textureCoordinates.resize(nbTexCoords);
            bufferSize = nbTexCoords * sizeof(Vector2f);
            file.read(
                (char*)_trianglesMeshes[materialId].textureCoordinates.data(),
                bufferSize);
        }

        BRAYNS_INFO << "[" << materialId << "] " << nbSpheres << " spheres, "
                    << nbCylinders << " cylinders, " << nbCones << " cones, "
                    << nbVertices << " vertices, " << nbIndices << " indices, "
                    << nbNormals << " normals, " << nbTexCoords
                    << " texture coordinates" << std::endl;
    }

    // Scene bounds
    file.read((char*)&_bounds, sizeof(Boxf));
    BRAYNS_DEBUG << "AABB: " << _bounds << std::endl;
    file.close();

    BRAYNS_INFO << "Scene successfully loaded" << std::endl;
}

size_t Scene::addMaterial(const Material& material)
{
    _materials.push_back(material);
    return _materials.size() - 1;
}

void Scene::setMaterial(const size_t index, const Material& material)
{
    _buildMissingMaterials(index);
    _materials[index] = material;
}
}
