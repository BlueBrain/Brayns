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

#include "OSPRayScene.h"
#include "OSPRayRenderer.h"

#include <brayns/common/light/DirectionalLight.h>
#include <brayns/common/light/PointLight.h>
#include <brayns/common/log.h>
#include <brayns/common/material/Texture2D.h>
#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/common/volume/VolumeHandler.h>
#include <brayns/io/ImageManager.h>
#include <brayns/parameters/GeometryParameters.h>
#include <brayns/parameters/SceneParameters.h>

#include <boost/algorithm/string/predicate.hpp> // ends_with

namespace brayns
{
const size_t CACHE_VERSION = 8;

struct TextureTypeMaterialAttribute
{
    TextureType type;
    std::string attribute;
};

static TextureTypeMaterialAttribute textureTypeMaterialAttribute[7] = {
    {TT_DIFFUSE, "map_kd"},
    {TT_NORMALS, "map_bump"},
    {TT_SPECULAR, "map_ks"},
    {TT_EMISSIVE, "map_a"},
    {TT_OPACITY, "map_d"},
    {TT_REFLECTION, "map_reflection"},
    {TT_REFRACTION, "map_refraction"}};

OSPRayScene::OSPRayScene(Renderers renderers,
                         ParametersManager& parametersManager)
    : Scene(renderers, parametersManager)
    , _model(nullptr)
    , _simulationModel(nullptr)
    , _ospLightData(nullptr)
    , _ospMaterialData(nullptr)
    , _ospVolumeData(nullptr)
    , _ospSimulationData(nullptr)
    , _ospTransferFunctionDiffuseData(nullptr)
    , _ospTransferFunctionEmissionData(nullptr)
{
}

OSPRayScene::~OSPRayScene()
{
    if (_ospTransferFunctionDiffuseData)
        ospRelease(_ospTransferFunctionDiffuseData);

    if (_ospTransferFunctionEmissionData)
        ospRelease(_ospTransferFunctionEmissionData);

    for (auto& light : _ospLights)
        ospRelease(light);
    _ospLights.clear();
}

void OSPRayScene::unload()
{
    if (_model)
    {
        for (size_t materialId = 0; materialId < _materials.size();
             ++materialId)
        {
            if (_ospMeshes[materialId])
                ospRemoveGeometry(_model, _ospMeshes[materialId]);
            if (_ospExtendedSpheres[materialId])
                ospRemoveGeometry(_model, _ospExtendedSpheres[materialId]);
            if (_ospExtendedCylinders[materialId])
                ospRemoveGeometry(_model, _ospExtendedCylinders[materialId]);
            if (_ospExtendedCones[materialId])
                ospRemoveGeometry(_model, _ospExtendedCones[materialId]);
        }
        ospCommit(_model);
        ospRelease(_model);
        _model = nullptr;
    }

    if (_simulationModel)
    {
        for (size_t materialId = 0; materialId < _materials.size();
             ++materialId)
        {
            ospRemoveGeometry(_simulationModel,
                              _ospExtendedSpheres[materialId]);
            ospRemoveGeometry(_simulationModel,
                              _ospExtendedCylinders[materialId]);
            ospRemoveGeometry(_simulationModel, _ospExtendedCones[materialId]);
        }
        ospCommit(_simulationModel);
        ospRelease(_simulationModel);
        _simulationModel = nullptr;
    }

    Scene::unload();

    for (auto& material : _ospMaterials)
        ospRelease(material);
    _ospMaterials.clear();

    for (auto& texture : _ospTextures)
        ospRelease(texture.second);
    _ospTextures.clear();

    if (_ospSimulationData)
        ospRelease(_ospSimulationData);

    if (_ospVolumeData)
        ospRelease(_ospVolumeData);

    for (auto& geom : _ospExtendedSpheres)
        ospRelease(geom.second);
    _ospExtendedSpheres.clear();
    for (auto& geom : _ospExtendedSpheresData)
        ospRelease(geom.second);
    _ospExtendedSpheresData.clear();
    for (auto& geom : _ospExtendedCylinders)
        ospRelease(geom.second);
    _ospExtendedCylinders.clear();
    for (auto& geom : _ospExtendedCylindersData)
        ospRelease(geom.second);
    _ospExtendedCylindersData.clear();
    for (auto& geom : _ospExtendedCones)
        ospRelease(geom.second);
    _ospExtendedCones.clear();
    for (auto& geom : _ospExtendedConesData)
        ospRelease(geom.second);
    _ospExtendedConesData.clear();
    for (auto& geom : _ospMeshes)
        ospRelease(geom.second);
    _ospMeshes.clear();
}

void OSPRayScene::commit()
{
    if (_model)
        ospCommit(_model);

    if (_simulationModel)
    {
        BRAYNS_INFO << "Committing simulation model" << std::endl;
        ospCommit(_simulationModel);
    }
}

void OSPRayScene::saveToCacheFile()
{
    const std::string& filename =
        _parametersManager.getGeometryParameters().getSaveCacheFile();
    BRAYNS_INFO << "Saving scene to binary file: " << filename << std::endl;
    std::ofstream file(filename, std::ios::out | std::ios::binary);

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
        // TODO: Textures
    }

    // Save geometry
    for (auto& material : _materials)
    {
        const auto materialId = material.first;
        size_t bufferSize;

        // Spheres
        bufferSize = _spheres[materialId].size();
        file.write((char*)&bufferSize, sizeof(size_t));
        bufferSize = _spheres[materialId].size() * sizeof(Sphere);
        file.write((char*)&bufferSize, sizeof(size_t));
        file.write((char*)_spheres[materialId].data(), bufferSize);
        if (bufferSize != 0)
            BRAYNS_DEBUG << "[" << materialId << "] "
                         << _spheres[materialId].size() << " Spheres"
                         << std::endl;

        // Cylinders
        bufferSize = _cylinders[materialId].size();
        file.write((char*)&bufferSize, sizeof(size_t));
        bufferSize = _cylinders[materialId].size() * sizeof(Cylinder);
        file.write((char*)&bufferSize, sizeof(size_t));
        file.write((char*)_cylinders[materialId].data(), bufferSize);
        if (bufferSize != 0)
            BRAYNS_DEBUG << "[" << materialId << "] "
                         << _cylinders[materialId].size() << " Cylinders"
                         << std::endl;

        // Cones
        bufferSize = _cones[materialId].size();
        file.write((char*)&bufferSize, sizeof(size_t));
        bufferSize = _cones[materialId].size() * sizeof(Cone);
        file.write((char*)&bufferSize, sizeof(size_t));
        file.write((char*)_cones[materialId].data(), bufferSize);
        if (bufferSize != 0)
            BRAYNS_DEBUG << "[" << materialId << "] "
                         << _cones[materialId].size() << " Cones" << std::endl;

        if (_trianglesMeshes.find(materialId) != _trianglesMeshes.end())
        {
            // Vertices
            bufferSize =
                _trianglesMeshes[materialId].vertices.size() * sizeof(Vector3f);
            file.write((char*)&bufferSize, sizeof(size_t));
            file.write((char*)_trianglesMeshes[materialId].vertices.data(),
                       bufferSize);
            if (bufferSize != 0)
                BRAYNS_DEBUG << "[" << materialId << "] "
                             << _trianglesMeshes[materialId].vertices.size()
                             << " Vertices" << std::endl;

            // Indices
            bufferSize =
                _trianglesMeshes[materialId].indices.size() * sizeof(Vector3ui);
            file.write((char*)&bufferSize, sizeof(size_t));
            file.write((char*)_trianglesMeshes[materialId].indices.data(),
                       bufferSize);
            if (bufferSize != 0)
                BRAYNS_DEBUG << "[" << materialId << "] "
                             << _trianglesMeshes[materialId].indices.size()
                             << " Indices" << std::endl;

            // Normals
            bufferSize =
                _trianglesMeshes[materialId].normals.size() * sizeof(Vector3f);
            file.write((char*)&bufferSize, sizeof(size_t));
            file.write((char*)_trianglesMeshes[materialId].normals.data(),
                       bufferSize);
            if (bufferSize != 0)
                BRAYNS_DEBUG << "[" << materialId << "] "
                             << _trianglesMeshes[materialId].normals.size()
                             << " Normals" << std::endl;

            // Texture coordinates
            bufferSize =
                _trianglesMeshes[materialId].textureCoordinates.size() *
                sizeof(Vector2f);
            file.write((char*)&bufferSize, sizeof(size_t));
            file.write(
                (char*)_trianglesMeshes[materialId].textureCoordinates.data(),
                bufferSize);
            if (bufferSize != 0)
                BRAYNS_DEBUG
                    << "[" << materialId << "] "
                    << _trianglesMeshes[materialId].textureCoordinates.size()
                    << " Texture coordinates" << std::endl;
        }
        else
        {
            bufferSize = 0;
            file.write((char*)&bufferSize, sizeof(size_t)); // No vertices
            file.write((char*)&bufferSize, sizeof(size_t)); // No indices
            file.write((char*)&bufferSize, sizeof(size_t)); // No normals
            file.write((char*)&bufferSize,
                       sizeof(size_t)); // No Texture coordinates
        }
    }

    file.write((char*)&_bounds, sizeof(Boxf));
    BRAYNS_INFO << _bounds << std::endl;
    file.close();
    BRAYNS_INFO << "Scene successfully saved" << std::endl;
}

void OSPRayScene::loadFromCacheFile()
{
    const auto& geomParams = _parametersManager.getGeometryParameters();

    const std::string& filename = geomParams.getLoadCacheFile();
    BRAYNS_INFO << "Loading scene from binary file: " << filename << std::endl;
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.good())
    {
        BRAYNS_ERROR << "Could not open cache file " << filename << std::endl;
        return;
    }

    size_t version;
    file.read((char*)&version, sizeof(size_t));
    BRAYNS_INFO << "Version: " << version << std::endl;

    if (version != CACHE_VERSION)
    {
        BRAYNS_ERROR << "Only version " << CACHE_VERSION << " is supported"
                     << std::endl;
        return;
    }

    if (geomParams.getCircuitUseSimulationModel() && !_simulationModel)
        _simulationModel = ospNewModel();

    size_t nbMaterials;
    file.read((char*)&nbMaterials, sizeof(size_t));
    BRAYNS_INFO << nbMaterials << " materials" << std::endl;

    // Read materials
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
        // TODO: Textures
    }
    commitMaterials();

    // Read geometry
    for (size_t materialId = 0; materialId < nbMaterials; ++materialId)
    {
        // Spheres
        size_t bufferSize = 0;
        file.read((char*)&bufferSize, sizeof(size_t));
        if (bufferSize != 0)
        {
            const auto nbElements = bufferSize / sizeof(Sphere);
            BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                         << " Spheres" << std::endl;
            _spheres[materialId].reserve(nbElements);
            file.read((char*)_spheres[materialId].data(), bufferSize);
        }

        // Cylinders
        bufferSize = 0;
        file.read((char*)&bufferSize, sizeof(size_t));
        if (bufferSize != 0)
        {
            const auto nbElements = bufferSize / sizeof(Sphere);
            BRAYNS_DEBUG << "[" << materialId << "] " << nbElements
                         << " Cylinders" << std::endl;
            _cylinders[materialId].reserve(nbElements);
            file.read((char*)_cylinders[materialId].data(), bufferSize);
        }

        // Cones
        bufferSize = 0;
        file.read((char*)&bufferSize, sizeof(size_t));
        if (bufferSize != 0)
        {
            const auto nbElements = bufferSize / sizeof(Sphere);
            BRAYNS_DEBUG << "[" << materialId << "] " << nbElements << " Cones"
                         << std::endl;
            _cones[materialId].reserve(nbElements);
            file.read((char*)_cones[materialId].data(), bufferSize);
        }

        // Vertices
        file.read((char*)&bufferSize, sizeof(size_t));
        _trianglesMeshes[materialId].vertices.reserve(bufferSize /
                                                      sizeof(Vector3f));
        file.read((char*)_trianglesMeshes[materialId].vertices.data(),
                  bufferSize);

        // Indices
        file.read((char*)&bufferSize, sizeof(size_t));
        _trianglesMeshes[materialId].indices.reserve(bufferSize /
                                                     sizeof(Vector3ui));
        file.read((char*)_trianglesMeshes[materialId].indices.data(),
                  bufferSize);

        // Normals
        file.read((char*)&bufferSize, sizeof(size_t));
        _trianglesMeshes[materialId].normals.reserve(bufferSize /
                                                     sizeof(Vector3f));
        file.read((char*)_trianglesMeshes[materialId].normals.data(),
                  bufferSize);

        // Texture coordinates
        file.read((char*)&bufferSize, sizeof(size_t));
        _trianglesMeshes[materialId].textureCoordinates.reserve(
            bufferSize / sizeof(Vector2f));
        file.read((char*)_trianglesMeshes[materialId].textureCoordinates.data(),
                  bufferSize);

        _buildMeshOSPGeometry(materialId);
    }

    // Scene bounds
    file.read((char*)&_bounds, sizeof(Boxf));

    BRAYNS_INFO << _bounds << std::endl;
    BRAYNS_INFO << "Scene successfully loaded" << std::endl;
}

uint64_t OSPRayScene::_serializeSpheres(const size_t materialId)
{
    if (_spheres.find(materialId) == _spheres.end())
        return 0;

    const auto& spheres = _spheres[materialId];
    const auto bufferSize = spheres.size() * sizeof(Sphere);
    if (_ospExtendedSpheres.find(materialId) != _ospExtendedSpheres.end())
        ospRemoveGeometry(_model, _ospExtendedSpheres[materialId]);

    _ospExtendedSpheres[materialId] = ospNewGeometry("extendedspheres");
    _ospExtendedSpheresData[materialId] =
        ospNewData(bufferSize / sizeof(float), OSP_FLOAT, spheres.data(),
                   _getOSPDataFlags());

    ospSetObject(_ospExtendedSpheres[materialId], "extendedspheres",
                 _ospExtendedSpheresData[materialId]);

    if (_ospMaterials[materialId])
        ospSetMaterial(_ospExtendedSpheres[materialId],
                       _ospMaterials[materialId]);

    ospCommit(_ospExtendedSpheres[materialId]);

    const auto& geometryParameters = _parametersManager.getGeometryParameters();
    if (geometryParameters.getCircuitUseSimulationModel())
        ospAddGeometry(_simulationModel, _ospExtendedSpheres[materialId]);
    else
        ospAddGeometry(_model, _ospExtendedSpheres[materialId]);

    return bufferSize;
}

uint64_t OSPRayScene::_serializeCylinders(const size_t materialId)
{
    if (_cylinders.find(materialId) == _cylinders.end())
        return 0;

    const auto& cylinders = _cylinders[materialId];
    const auto bufferSize = cylinders.size() * sizeof(Cylinder);
    if (_ospExtendedCylinders.find(materialId) != _ospExtendedCylinders.end())
        ospRemoveGeometry(_model, _ospExtendedCylinders[materialId]);

    _ospExtendedCylinders[materialId] = ospNewGeometry("extendedcylinders");
    _ospExtendedCylindersData[materialId] =
        ospNewData(bufferSize / sizeof(float), OSP_FLOAT, cylinders.data(),
                   _getOSPDataFlags());
    ospSetObject(_ospExtendedCylinders[materialId], "extendedcylinders",
                 _ospExtendedCylindersData[materialId]);

    if (_ospMaterials[materialId])
        ospSetMaterial(_ospExtendedCylinders[materialId],
                       _ospMaterials[materialId]);

    ospCommit(_ospExtendedCylinders[materialId]);

    const auto& geometryParameters = _parametersManager.getGeometryParameters();
    if (geometryParameters.getCircuitUseSimulationModel())
        ospAddGeometry(_simulationModel, _ospExtendedCylinders[materialId]);
    else
        ospAddGeometry(_model, _ospExtendedCylinders[materialId]);
    return bufferSize;
}

uint64_t OSPRayScene::_serializeCones(const size_t materialId)
{
    if (_cones.find(materialId) == _cones.end())
        return 0;

    const auto& cones = _cones[materialId];
    const auto bufferSize = cones.size() * sizeof(Cone);
    if (_ospExtendedCones.find(materialId) != _ospExtendedCones.end())
        ospRemoveGeometry(_model, _ospExtendedCones[materialId]);

    _ospExtendedCones[materialId] = ospNewGeometry("extendedcones");
    _ospExtendedConesData[materialId] =
        ospNewData(bufferSize / sizeof(float), OSP_FLOAT, cones.data(),
                   _getOSPDataFlags());
    ospSetObject(_ospExtendedCones[materialId], "extendedcones",
                 _ospExtendedConesData[materialId]);

    if (_ospMaterials[materialId])
        ospSetMaterial(_ospExtendedCones[materialId],
                       _ospMaterials[materialId]);

    ospCommit(_ospExtendedCones[materialId]);

    const auto& geometryParameters = _parametersManager.getGeometryParameters();
    if (geometryParameters.getCircuitUseSimulationModel())
        ospAddGeometry(_simulationModel, _ospExtendedCones[materialId]);
    else
        ospAddGeometry(_model, _ospExtendedCones[materialId]);
    return bufferSize;
}

uint64_t OSPRayScene::serializeGeometry()
{
    uint64_t size = 0;
    if (_spheresDirty)
        for (const auto& material : _materials)
            size += _serializeSpheres(material.first);

    if (_cylindersDirty)
        for (const auto& material : _materials)
            size += _serializeCylinders(material.first);

    if (_conesDirty)
        for (const auto& material : _materials)
            size += _serializeCones(material.first);

    if (_trianglesMeshesDirty)
        for (const auto& material : _materials)
            size += _buildMeshOSPGeometry(material.first);

    _spheresDirty = false;
    _cylindersDirty = false;
    _conesDirty = false;
    _trianglesMeshesDirty = false;
    return size;
}

void OSPRayScene::buildGeometry()
{
    BRAYNS_INFO << "Building OSPRay geometry" << std::endl;

    commitMaterials();

    const auto& geomParams = _parametersManager.getGeometryParameters();

    _model = ospNewModel();

    if (geomParams.getCircuitUseSimulationModel() && !_simulationModel)
        _simulationModel = ospNewModel();

    const size_t size = serializeGeometry();

    size_t totalNbSpheres = 0;
    size_t totalNbCylinders = 0;
    size_t totalNbCones = 0;
    size_t totalNbVertices = 0;
    size_t totalNbIndices = 0;
    for (auto& material : _materials)
    {
        const auto materialId = material.first;
        totalNbSpheres += _spheres[materialId].size();
        totalNbCylinders += _cylinders[materialId].size();
        totalNbCones += _cones[materialId].size();
        if (_trianglesMeshes.find(materialId) != _trianglesMeshes.end())
        {
            totalNbVertices += _trianglesMeshes[materialId].vertices.size();
            totalNbIndices += _trianglesMeshes[materialId].indices.size();
        }
    }

    BRAYNS_INFO << "---------------------------------------------------"
                << std::endl;
    BRAYNS_INFO << "Geometry inforesizermation" << std::endl;
    BRAYNS_INFO << "Spheres  : " << totalNbSpheres << std::endl;
    BRAYNS_INFO << "Cylinders: " << totalNbCylinders << std::endl;
    BRAYNS_INFO << "Cones    : " << totalNbCones << std::endl;
    BRAYNS_INFO << "Vertices : " << totalNbVertices << std::endl;
    BRAYNS_INFO << "Indices  : " << totalNbIndices << std::endl;
    BRAYNS_INFO << "Materials: " << _materials.size() << std::endl;
    BRAYNS_INFO << "Total    : " << size << " bytes (" << size / 1048576
                << " MB)" << std::endl;
    BRAYNS_INFO << "---------------------------------------------------"
                << std::endl;
}

uint64_t OSPRayScene::_buildMeshOSPGeometry(const size_t materialId)
{
    uint64_t size = 0;
    // Triangle meshes
    if (_trianglesMeshes.find(materialId) != _trianglesMeshes.end())
    {
        _ospMeshes[materialId] = ospNewGeometry("trianglemesh");
        assert(_ospMeshes[materialId]);

        size +=
            _trianglesMeshes[materialId].vertices.size() * 3 * sizeof(float);
        OSPData vertices =
            ospNewData(_trianglesMeshes[materialId].vertices.size(), OSP_FLOAT3,
                       &_trianglesMeshes[materialId].vertices[0],
                       _getOSPDataFlags());

        if (!_trianglesMeshes[materialId].normals.empty())
        {
            size +=
                _trianglesMeshes[materialId].normals.size() * 3 * sizeof(float);
            OSPData normals =
                ospNewData(_trianglesMeshes[materialId].normals.size(),
                           OSP_FLOAT3, &_trianglesMeshes[materialId].normals[0],
                           _getOSPDataFlags());
            ospSetObject(_ospMeshes[materialId], "vertex.normal", normals);
        }

        size += _trianglesMeshes[materialId].indices.size() * 3 * sizeof(int);
        OSPData indices =
            ospNewData(_trianglesMeshes[materialId].indices.size(), OSP_INT3,
                       &_trianglesMeshes[materialId].indices[0],
                       _getOSPDataFlags());

        if (!_trianglesMeshes[materialId].colors.empty())
        {
            size +=
                _trianglesMeshes[materialId].colors.size() * 4 * sizeof(float);
            OSPData colors =
                ospNewData(_trianglesMeshes[materialId].colors.size(),
                           OSP_FLOAT3A, &_trianglesMeshes[materialId].colors[0],
                           _getOSPDataFlags());
            ospSetObject(_ospMeshes[materialId], "vertex.color", colors);
            ospRelease(colors);
        }

        if (!_trianglesMeshes[materialId].textureCoordinates.empty())
        {
            size += _trianglesMeshes[materialId].textureCoordinates.size() * 2 *
                    sizeof(float);
            OSPData texCoords = ospNewData(
                _trianglesMeshes[materialId].textureCoordinates.size(),
                OSP_FLOAT2, &_trianglesMeshes[materialId].textureCoordinates[0],
                _getOSPDataFlags());
            ospSetObject(_ospMeshes[materialId], "vertex.texcoord", texCoords);
            ospRelease(texCoords);
        }

        ospSetObject(_ospMeshes[materialId], "position", vertices);
        ospRelease(vertices);
        ospSetObject(_ospMeshes[materialId], "index", indices);
        ospRelease(indices);
        ospSet1i(_ospMeshes[materialId], "alpha_type", 0);
        ospSet1i(_ospMeshes[materialId], "alpha_component", 4);

        if (_ospMaterials[materialId])
            ospSetMaterial(_ospMeshes[materialId], _ospMaterials[materialId]);

        ospCommit(_ospMeshes[materialId]);

        // Meshes are by default added to all timestamps
        ospAddGeometry(_model, _ospMeshes[materialId]);
    }
    return size;
}

void OSPRayScene::commitLights()
{
    size_t lightCount = 0;
    for (const auto& light : _lights)
    {
        DirectionalLight* directionalLight =
            dynamic_cast<DirectionalLight*>(light.get());
        if (directionalLight)
        {
            if (_ospLights.size() <= lightCount)
                _ospLights.push_back(ospNewLight(nullptr, "DirectionalLight"));

            const Vector3f color = directionalLight->getColor();
            ospSet3f(_ospLights[lightCount], "color", color.x(), color.y(),
                     color.z());
            const Vector3f direction = directionalLight->getDirection();
            ospSet3f(_ospLights[lightCount], "direction", direction.x(),
                     direction.y(), direction.z());
            ospSet1f(_ospLights[lightCount], "intensity",
                     directionalLight->getIntensity());
            ospCommit(_ospLights[lightCount]);
            ++lightCount;
        }
        else
        {
            PointLight* pointLight = dynamic_cast<PointLight*>(light.get());
            if (pointLight)
            {
                if (_ospLights.size() <= lightCount)
                    _ospLights.push_back(ospNewLight(nullptr, "PointLight"));

                const Vector3f position = pointLight->getPosition();
                ospSet3f(_ospLights[lightCount], "position", position.x(),
                         position.y(), position.z());
                const Vector3f color = pointLight->getColor();
                ospSet3f(_ospLights[lightCount], "color", color.x(), color.y(),
                         color.z());
                ospSet1f(_ospLights[lightCount], "intensity",
                         pointLight->getIntensity());
                ospSet1f(_ospLights[lightCount], "radius",
                         pointLight->getCutoffDistance());
                ospCommit(_ospLights[lightCount]);
                ++lightCount;
            }
        }
    }

    if (!_ospLightData)
    {
        _ospLightData = ospNewData(_ospLights.size(), OSP_OBJECT,
                                   &_ospLights[0], _getOSPDataFlags());
        ospCommit(_ospLightData);
        for (auto renderer : _renderers)
        {
            OSPRayRenderer* osprayRenderer =
                dynamic_cast<OSPRayRenderer*>(renderer.get());
            ospSetData(osprayRenderer->impl(), "lights", _ospLightData);
        }
    }
}

void OSPRayScene::commitMaterials(const bool updateOnly)
{
    // Determine how many materials need to be created
    size_t maxId = 0;
    for (auto& material : _materials)
        maxId = std::max(maxId, material.first);

    for (size_t i = 0; i < maxId; ++i)
        if (_materials.find(i) == _materials.end())
            _materials[i] = Material();

    BRAYNS_INFO << "Committing " << maxId + 1 << " OSPRay materials"
                << std::endl;

    for (auto& material : _materials)
    {
        if (_ospMaterials.size() <= material.first)
            _ospMaterials.push_back(
                ospNewMaterial(nullptr, "ExtendedOBJMaterial"));

        auto& ospMaterial = _ospMaterials[material.first];

        Vector3f value3f = material.second.getColor();
        ospSet3f(ospMaterial, "kd", value3f.x(), value3f.y(), value3f.z());
        value3f = material.second.getSpecularColor();
        ospSet3f(ospMaterial, "ks", value3f.x(), value3f.y(), value3f.z());
        ospSet1f(ospMaterial, "ns", material.second.getSpecularExponent());
        ospSet1f(ospMaterial, "d", material.second.getOpacity());
        ospSet1f(ospMaterial, "refraction",
                 material.second.getRefractionIndex());
        ospSet1f(ospMaterial, "reflection",
                 material.second.getReflectionIndex());
        ospSet1f(ospMaterial, "a", material.second.getEmission());
        ospSet1f(ospMaterial, "glossiness", material.second.getGlossiness());
        ospSet1i(ospMaterial, "cast_simulation_data",
                 material.second.getCastSimulationData());

        for (const auto& textureType : textureTypeMaterialAttribute)
            ospSetObject(ospMaterial, textureType.attribute.c_str(), nullptr);

        if (!updateOnly)
        {
            // Textures
            for (auto texture : material.second.getTextures())
            {
                if (texture.second != TEXTURE_NAME_SIMULATION)
                    ImageManager::importTextureFromFile(_textures,
                                                        texture.first,
                                                        texture.second);
                else
                    BRAYNS_ERROR << "Failed to load texture: " << texture.second
                                 << std::endl;

                OSPTexture2D ospTexture = _createTexture2D(texture.second);
                ospSetObject(ospMaterial,
                             textureTypeMaterialAttribute[texture.first]
                                 .attribute.c_str(),
                             ospTexture);

                BRAYNS_DEBUG
                    << "Texture assigned to "
                    << textureTypeMaterialAttribute[texture.first].attribute
                    << " of material " << material.first << ": "
                    << texture.second << std::endl;
            }
        }
        ospCommit(ospMaterial);
    }

    _ospMaterialData = ospNewData(_materials.size(), OSP_OBJECT,
                                  &_ospMaterials[0], _getOSPDataFlags());
    ospCommit(_ospMaterialData);

    for (const auto& renderer : _renderers)
    {
        OSPRayRenderer* osprayRenderer =
            dynamic_cast<OSPRayRenderer*>(renderer.get());
        ospSetData(osprayRenderer->impl(), "materials", _ospMaterialData);
        ospCommit(osprayRenderer->impl());
    }
    _modified = true;
}

void OSPRayScene::commitTransferFunctionData()
{
    if (_ospTransferFunctionDiffuseData)
        ospRelease(_ospTransferFunctionDiffuseData);

    if (_ospTransferFunctionEmissionData)
        ospRelease(_ospTransferFunctionEmissionData);

    _ospTransferFunctionDiffuseData =
        ospNewData(_transferFunction.getDiffuseColors().size(), OSP_FLOAT4,
                   _transferFunction.getDiffuseColors().data(),
                   _getOSPDataFlags());
    ospCommit(_ospTransferFunctionDiffuseData);

    _ospTransferFunctionEmissionData =
        ospNewData(_transferFunction.getEmissionIntensities().size(),
                   OSP_FLOAT3,
                   _transferFunction.getEmissionIntensities().data(),
                   _getOSPDataFlags());
    ospCommit(_ospTransferFunctionEmissionData);

    for (const auto& renderer : _renderers)
    {
        OSPRayRenderer* osprayRenderer =
            dynamic_cast<OSPRayRenderer*>(renderer.get());

        // Transfer function Diffuse colors
        ospSetData(osprayRenderer->impl(), "transferFunctionDiffuseData",
                   _ospTransferFunctionDiffuseData);

        // Transfer function emission data
        ospSetData(osprayRenderer->impl(), "transferFunctionEmissionData",
                   _ospTransferFunctionEmissionData);

        // Transfer function size
        ospSet1i(osprayRenderer->impl(), "transferFunctionSize",
                 _transferFunction.getDiffuseColors().size());

        // Transfer function range
        ospSet1f(osprayRenderer->impl(), "transferFunctionMinValue",
                 _transferFunction.getValuesRange().x());
        ospSet1f(osprayRenderer->impl(), "transferFunctionRange",
                 _transferFunction.getValuesRange().y() -
                     _transferFunction.getValuesRange().x());

        ospCommit(osprayRenderer->impl());
    }
    _modified = true;
}

void OSPRayScene::commitVolumeData()
{
    VolumeHandlerPtr volumeHandler = getVolumeHandler();
    if (!volumeHandler)
        return;

    const auto animationFrame =
        _parametersManager.getSceneParameters().getAnimationFrame();
    volumeHandler->setCurrentIndex(animationFrame);
    void* data = volumeHandler->getData();
    if (data)
    {
        for (const auto& renderer : _renderers)
        {
            OSPRayRenderer* osprayRenderer =
                dynamic_cast<OSPRayRenderer*>(renderer.get());

            const size_t size = volumeHandler->getSize();

            _ospVolumeData =
                ospNewData(size, OSP_UCHAR, data, _getOSPDataFlags());
            ospCommit(_ospVolumeData);
            ospSetData(osprayRenderer->impl(), "volumeData", _ospVolumeData);

            const Vector3ui& dimensions = volumeHandler->getDimensions();
            ospSet3i(osprayRenderer->impl(), "volumeDimensions", dimensions.x(),
                     dimensions.y(), dimensions.z());

            const Vector3f& elementSpacing =
                _parametersManager.getVolumeParameters().getElementSpacing();
            ospSet3f(osprayRenderer->impl(), "volumeElementSpacing",
                     elementSpacing.x(), elementSpacing.y(),
                     elementSpacing.z());

            const Vector3f& offset =
                _parametersManager.getVolumeParameters().getOffset();
            ospSet3f(osprayRenderer->impl(), "volumeOffset", offset.x(),
                     offset.y(), offset.z());

            const float epsilon = volumeHandler->getEpsilon(
                elementSpacing,
                _parametersManager.getVolumeParameters().getSamplesPerRay());
            ospSet1f(osprayRenderer->impl(), "volumeEpsilon", epsilon);
        }
    }
    _modified = true;
}

void OSPRayScene::commitSimulationData()
{
    if (!_simulationHandler)
    {
        return;
    }

    const auto animationFrame =
        _parametersManager.getSceneParameters().getAnimationFrame();

    if (_simulationHandler->getCurrentFrame() == animationFrame)
        return;

    auto frameData = _simulationHandler->getFrameData(animationFrame);

    if (!frameData)
        return;

    if (_ospSimulationData)
        ospRelease(_ospSimulationData);
    _ospSimulationData = ospNewData(_simulationHandler->getFrameSize(),
                                    OSP_FLOAT, frameData, _getOSPDataFlags());
    ospCommit(_ospSimulationData);

    for (const auto& renderer : _renderers)
    {
        OSPRayRenderer* osprayRenderer =
            dynamic_cast<OSPRayRenderer*>(renderer.get());

        ospSetData(osprayRenderer->impl(), "simulationData",
                   _ospSimulationData);
        ospSet1i(osprayRenderer->impl(), "simulationDataSize",
                 _simulationHandler->getFrameSize());
        ospCommit(osprayRenderer->impl());
    }
    _modified = true;
}

OSPTexture2D OSPRayScene::_createTexture2D(const std::string& textureName)
{
    if (_ospTextures.find(textureName) != _ospTextures.end())
        return _ospTextures[textureName];

    Texture2DPtr texture = _textures[textureName];
    if (!texture)
    {
        BRAYNS_WARN << "Texture " << textureName << " is not in the cache"
                    << std::endl;
        return nullptr;
    }

    OSPTextureFormat type = OSP_TEXTURE_R8; // smallest valid type as default
    if (texture->getDepth() == 1)
    {
        if (texture->getNbChannels() == 3)
            type = OSP_TEXTURE_SRGB;
        if (texture->getNbChannels() == 4)
            type = OSP_TEXTURE_SRGBA;
    }
    else if (texture->getDepth() == 4)
    {
        if (texture->getNbChannels() == 3)
            type = OSP_TEXTURE_RGB32F;
        if (texture->getNbChannels() == 4)
            type = OSP_TEXTURE_RGBA32F;
    }

    BRAYNS_DEBUG << "Creating OSPRay texture from " << textureName << " :"
                 << texture->getWidth() << "x" << texture->getHeight() << "x"
                 << (int)type << std::endl;

    osp::vec2i texSize{int(texture->getWidth()), int(texture->getHeight())};
    OSPTexture2D ospTexture =
        ospNewTexture2D(texSize, type, texture->getRawData(), 0);

    assert(ospTexture);
    ospCommit(ospTexture);

    _ospTextures[textureName] = ospTexture;

    return ospTexture;
}

bool OSPRayScene::isVolumeSupported(const std::string& volumeFile) const
{
    return boost::algorithm::ends_with(volumeFile, ".raw");
}

uint32_t OSPRayScene::_getOSPDataFlags()
{
    return _parametersManager.getGeometryParameters().getMemoryMode() ==
                   MemoryMode::shared
               ? OSP_DATA_SHARED_BUFFER
               : 0;
}
}
