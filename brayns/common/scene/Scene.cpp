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

bool Scene::getModified() const
{
    return _modified;
}

void Scene::resetModified()
{
    _modified = false;
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
    _modified = true;
}

void Scene::buildMaterials()
{
    BRAYNS_INFO << "Building materials" << std::endl;
    for (const auto& spheres : _spheres)
        if (!spheres.second.empty() &&
            _materials.find(spheres.first) == _materials.end())
            _materials[spheres.first] = Material();
    for (const auto& cylinders : _cylinders)
        if (!cylinders.second.empty() &&
            _materials.find(cylinders.first) == _materials.end())
            _materials[cylinders.first] = Material();
    for (const auto& cones : _cones)
        if (!cones.second.empty() &&
            _materials.find(cones.first) == _materials.end())
            _materials[cones.first] = Material();
    for (auto& meshes : _trianglesMeshes)
        if (!meshes.second.indices.empty() &&
            _materials.find(meshes.first) == _materials.end())
            _materials[meshes.first] = Material();

    for (size_t i = 0; i < NB_SYSTEM_MATERIALS; ++i)
    {
        auto& material = _materials[i];
        switch (i)
        {
        case MATERIAL_BOUNDING_BOX:
            material.setColor(Vector3f(1.f, 1.f, 1.f));
            material.setEmission(10.f);
            break;
        case MATERIAL_INVISIBLE:
            material.setOpacity(0.f);
            material.setRefractionIndex(1.f);
            material.setColor(Vector3f(1.f, 1.f, 1.f));
            material.setSpecularColor(Vector3f(0.f, 0.f, 0.f));
            break;
        }
    }
}

void Scene::setMaterials(const MaterialType materialType)
{
    const auto nbMaterials = _materials.size();
    for (size_t i = NB_SYSTEM_MATERIALS; i < nbMaterials; ++i)
    {
        auto& material = _materials[i];
        material.setSpecularColor(Vector3f(0.f, 0.f, 0.f));
        material.setOpacity(1.f);
        material.setReflectionIndex(0.f);

        switch (materialType)
        {
        case MaterialType::none:
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
        case MaterialType::gradient:
        {
            const float a = float(i) / float(nbMaterials);
            material.setColor(Vector3f(a, 0.f, 1.f - a));
            break;
        }
        case MaterialType::pastel:
            material.setColor(
                Vector3f(0.5f + float(std::rand() % 127) / 255.f,
                         0.5f + float(std::rand() % 127) / 255.f,
                         0.5f + float(std::rand() % 127) / 255.f));
            break;
        case MaterialType::random:
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
        case MaterialType::shades_of_grey:
            float value = float(std::rand() % 255) / 255.f;
            material.setColor(Vector3f(value, value, value));
            break;
        }
        _materials[i] = material;
    }
    commitMaterials();
}

Material& Scene::getMaterial(size_t index)
{
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

    // Cornell box
    for (size_t material = 1; material < 6; ++material)
    {
        auto& m = _materials[NB_SYSTEM_MATERIALS + material];
        m.setColor(colors[material]);
        m.setSpecularColor(WHITE);
        m.setSpecularExponent(10.f);
        m.setReflectionIndex(material == 4 ? 0.8f : 0.f);
        m.setOpacity(1.f);
        auto& trianglesMesh = _trianglesMeshes[NB_SYSTEM_MATERIALS + material];
        for (size_t i = 0; i < 6; ++i)
        {
            const auto position = positions[indices[material][i]];
            trianglesMesh.vertices.push_back(position);
            _bounds.merge(position);
        }
        trianglesMesh.indices.push_back(Vector3ui(0, 1, 2));
        trianglesMesh.indices.push_back(Vector3ui(3, 4, 5));
    }

    size_t material = NB_SYSTEM_MATERIALS + 7;

    // Sphere
    {
        addSphere(material, {{0.25f, 0.26f, 0.30f}, 0.25f});
        auto& m = _materials[material];
        m.setOpacity(0.3f);
        m.setRefractionIndex(1.1f);
        m.setColor(WHITE);
        m.setSpecularColor(WHITE);
        m.setSpecularExponent(100.f);
    }

    // Cylinder
    {
        ++material;
        addCylinder(material,
                    {{0.25f, 0.126f, 0.75f}, {0.75f, 0.126f, 0.75f}, 0.125f});
        auto& m = _materials[material];
        m.setColor({0.1f, 0.1f, 0.8f});
        m.setSpecularColor(WHITE);
        m.setSpecularExponent(10.f);
    }

    // Cone
    {
        ++material;
        addCone(material,
                {{0.75f, 0.01f, 0.25f}, {0.75f, 0.5f, 0.25f}, 0.15f, 0.f});
        auto& m = _materials[material];
        m.setReflectionIndex(0.8f);
        m.setSpecularColor(WHITE);
        m.setSpecularExponent(10.f);
    }

    // Lamp
    {
        ++material;
        const Vector3f lampInfo = {0.15f, 0.99f, 0.15f};
        const Vector3f lampPositions[4] = {
            {0.5f - lampInfo.x(), lampInfo.y(), 0.5f - lampInfo.z()},
            {0.5f + lampInfo.x(), lampInfo.y(), 0.5f - lampInfo.z()},
            {0.5f + lampInfo.x(), lampInfo.y(), 0.5f + lampInfo.z()},
            {0.5f - lampInfo.x(), lampInfo.y(), 0.5f + lampInfo.z()}};
        for (size_t i = 0; i < 4; ++i)
            _trianglesMeshes[material].vertices.push_back(lampPositions[i]);
        _trianglesMeshes[material].indices.push_back(Vector3i(2, 1, 0));
        _trianglesMeshes[material].indices.push_back(Vector3i(0, 3, 2));

        auto& m = _materials[material];
        m.setColor(WHITE);
        m.setEmission(5.f);
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
        const size_t material = MATERIAL_BOUNDING_BOX;
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
    _simulationHandler = handler;
}

AbstractSimulationHandlerPtr Scene::getSimulationHandler() const
{
    return _simulationHandler;
}

void Scene::setCADiffusionSimulationHandler(
    CADiffusionSimulationHandlerPtr handler)
{
    _caDiffusionSimulationHandler = handler;
}

CADiffusionSimulationHandlerPtr Scene::getCADiffusionSimulationHandler() const
{
    return _caDiffusionSimulationHandler;
}

VolumeHandlerPtr Scene::getVolumeHandler()
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

    return _volumeHandler;
}

bool Scene::empty() const
{
    return _spheres.empty() && _cylinders.empty() && _cones.empty() &&
           _trianglesMeshes.empty();
}

uint64_t Scene::addSphere(const size_t materialId, const Sphere& sphere)
{
    _spheres[materialId].push_back(sphere);
    _bounds.merge(sphere.center);
    return _spheres[materialId].size() - 1;
}

uint64_t Scene::addCylinder(const size_t materialId, const Cylinder& cylinder)
{
    _cylinders[materialId].push_back(cylinder);
    _bounds.merge(cylinder.center);
    _bounds.merge(cylinder.up);
    return _cylinders[materialId].size() - 1;
}

uint64_t Scene::addCone(const size_t materialId, const Cone& cone)
{
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
        size_t id = material.first;
        file.write((char*)&id, sizeof(size_t));
        Vector3f value3f;
        value3f = material.second.getColor();
        file.write((char*)&value3f, sizeof(Vector3f));
        value3f = material.second.getSpecularColor();
        file.write((char*)&value3f, sizeof(Vector3f));
        float value = material.second.getSpecularExponent();
        file.write((char*)&value, sizeof(float));
        value = material.second.getReflectionIndex();
        file.write((char*)&value, sizeof(float));
        value = material.second.getOpacity();
        file.write((char*)&value, sizeof(float));
        value = material.second.getRefractionIndex();
        file.write((char*)&value, sizeof(float));
        value = material.second.getEmission();
        file.write((char*)&value, sizeof(float));
        value = material.second.getGlossiness();
        file.write((char*)&value, sizeof(float));
        const bool boolean = material.second.getCastSimulationData();
        file.write((char*)&boolean, sizeof(bool));
        // TODO: Textures
    }

    // Save geometry
    for (auto& material : _materials)
    {
        const auto materialId = material.first;
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
    buildMaterials();
    for (size_t i = 0; i < nbMaterials; ++i)
    {
        size_t id;
        file.read((char*)&id, sizeof(size_t));
        auto& material = _materials[id];
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
    commitMaterials();

    // Geometry
    for (size_t materialId = 0; materialId < nbMaterials; ++materialId)
    {
        size_t nbElements{0};
        uint64_t bufferSize{0};

        // Spheres
        file.read((char*)&nbElements, sizeof(size_t));
        if (nbElements != 0)
        {
            bufferSize = nbElements * sizeof(Sphere);
            BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                         << " spheres" << std::endl;
            _spheres[materialId].resize(nbElements);
            file.read((char*)_spheres[materialId].data(), bufferSize);
        }

        // Cylinders
        file.read((char*)&nbElements, sizeof(size_t));
        if (nbElements != 0)
        {
            bufferSize = nbElements * sizeof(Cylinder);
            BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                         << " cylinders" << std::endl;
            _cylinders[materialId].resize(nbElements);
            file.read((char*)_cylinders[materialId].data(), bufferSize);
        }

        // Cones
        file.read((char*)&nbElements, sizeof(size_t));
        if (nbElements != 0)
        {
            bufferSize = nbElements * sizeof(Cone);
            BRAYNS_DEBUG << "[" << materialId << "] " << nbElements << " cones"
                         << std::endl;
            _cones[materialId].resize(nbElements);
            file.read((char*)_cones[materialId].data(), bufferSize);
        }

        // Vertices
        file.read((char*)&nbElements, sizeof(size_t));
        if (nbElements != 0)
        {
            BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                         << " vertices" << std::endl;
            _trianglesMeshes[materialId].vertices.resize(nbElements);
            bufferSize = nbElements * sizeof(Vector3f);
            file.read((char*)_trianglesMeshes[materialId].vertices.data(),
                      bufferSize);
        }

        // Indices
        file.read((char*)&nbElements, sizeof(size_t));
        if (nbElements != 0)
        {
            BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                         << " indices" << std::endl;
            _trianglesMeshes[materialId].indices.resize(nbElements);
            bufferSize = nbElements * sizeof(Vector3ui);
            file.read((char*)_trianglesMeshes[materialId].indices.data(),
                      bufferSize);
        }

        // Normals
        file.read((char*)&nbElements, sizeof(size_t));
        if (nbElements != 0)
        {
            BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                         << " normals" << std::endl;
            _trianglesMeshes[materialId].normals.resize(nbElements);
            bufferSize = nbElements * sizeof(Vector3f);
            file.read((char*)_trianglesMeshes[materialId].normals.data(),
                      bufferSize);
        }

        // Texture coordinates
        file.read((char*)&nbElements, sizeof(size_t));
        if (nbElements != 0)
        {
            BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                         << " texture coordinates" << std::endl;
            _trianglesMeshes[materialId].textureCoordinates.resize(nbElements);
            bufferSize = nbElements * sizeof(Vector2f);
            file.read(
                (char*)_trianglesMeshes[materialId].textureCoordinates.data(),
                bufferSize);
        }
    }

    // Scene bounds
    file.read((char*)&_bounds, sizeof(Boxf));
    file.close();

    BRAYNS_INFO << "Scene successfully loaded" << std::endl;
}
}
