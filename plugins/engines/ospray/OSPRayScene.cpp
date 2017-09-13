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
#include <brayns/io/TextureLoader.h>
#include <brayns/parameters/GeometryParameters.h>
#include <brayns/parameters/SceneParameters.h>

#include <boost/algorithm/string/predicate.hpp> // ends_with

namespace brayns
{
const size_t CACHE_VERSION = 7;

struct TextureTypeMaterialAttribute
{
    TextureType type;
    std::string attribute;
};

static TextureTypeMaterialAttribute textureTypeMaterialAttribute[6] = {
    {TT_DIFFUSE, "map_kd"},  {TT_NORMALS, "map_Normal"},
    {TT_SPECULAR, "map_ks"}, {TT_EMISSIVE, "map_a"},
    {TT_OPACITY, "map_d"},   {TT_REFLECTION, "map_Reflection"}};

OSPRayScene::OSPRayScene(Renderers renderers,
                         ParametersManager& parametersManager)
    : Scene(renderers, parametersManager)
    , _simulationModel(0)
    , _ospLightData(0)
    , _ospMaterialData(0)
    , _ospVolumeData(0)
    , _ospSimulationData(0)
    , _ospTransferFunctionDiffuseData(0)
    , _ospTransferFunctionEmissionData(0)
{
}

OSPRayScene::~OSPRayScene()
{
    if (_ospTransferFunctionDiffuseData)
        ospRelease(_ospTransferFunctionDiffuseData);

    if (_ospTransferFunctionEmissionData)
        ospRelease(_ospTransferFunctionEmissionData);

    if (_ospSimulationData)
        ospRelease(_ospSimulationData);

    if (_ospVolumeData)
        ospRelease(_ospVolumeData);

    for (auto& geom : _ospExtendedSpheres)
        ospRelease(geom.second);
    for (auto& geom : _ospExtendedSpheresData)
        ospRelease(geom.second);
    for (auto& geom : _ospExtendedCylinders)
        ospRelease(geom.second);
    for (auto& geom : _ospExtendedCylindersData)
        ospRelease(geom.second);
    for (auto& geom : _ospExtendedCones)
        ospRelease(geom.second);
    for (auto& geom : _ospExtendedConesData)
        ospRelease(geom.second);
    for (auto& geom : _ospMeshes)
        ospRelease(geom.second);
}

void OSPRayScene::reset()
{
    Scene::reset();

    for (const auto& model : _models)
    {
        for (size_t materialId = 0; materialId < _materials.size();
             ++materialId)
        {
            if (_ospMeshes[materialId])
                ospRemoveGeometry(model.second, _ospMeshes[materialId]);
            if (_ospExtendedSpheres[materialId])
                ospRemoveGeometry(model.second,
                                  _ospExtendedSpheres[materialId]);
            if (_ospExtendedCylinders[materialId])
                ospRemoveGeometry(model.second,
                                  _ospExtendedCylinders[materialId]);
            if (_ospExtendedCones[materialId])
                ospRemoveGeometry(model.second, _ospExtendedCones[materialId]);
        }
        ospCommit(model.second);
        ospRelease(model.second);
    }
    _models.clear();

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
    }

    for (auto& material : _ospMaterials)
        ospRelease(material);
    _ospMaterials.clear();

    for (auto& texture : _ospTextures)
        ospRelease(texture.second);
    _ospTextures.clear();

    for (auto& light : _ospLights)
        ospRelease(light);
    _ospLights.clear();

    _serializedSpheresData.clear();
    _serializedCylindersData.clear();
    _serializedConesData.clear();
    _serializedSpheresDataSize.clear();
    _serializedCylindersDataSize.clear();
    _serializedConesDataSize.clear();

    _timestampSpheresIndices.clear();
    _timestampCylindersIndices.clear();
    _timestampConesIndices.clear();
}

void OSPRayScene::commit()
{
    for (auto model : _models)
        ospCommit(model.second);
    if (_simulationModel)
    {
        BRAYNS_INFO << "Committing simulation model" << std::endl;
        ospCommit(_simulationModel);
    }
}

OSPModel* OSPRayScene::modelImpl(const size_t timestamp)
{
    if (_models.find(timestamp) != _models.end())
        return &_models[timestamp];

    int index = -1;
    for (const auto& model : _models)
        if (model.first <= timestamp)
            index = model.first;
    BRAYNS_DEBUG << "Request model for timestamp " << timestamp << ", returned "
                 << index << std::endl;
    return index == -1 ? nullptr : &_models[index];
}

void OSPRayScene::_saveCacheFile()
{
    const std::string& filename =
        _parametersManager.getGeometryParameters().getSaveCacheFile();
    BRAYNS_INFO << "Saving scene to binary file: " << filename << std::endl;
    std::ofstream file(filename, std::ios::out | std::ios::binary);

    const size_t version = CACHE_VERSION;
    file.write((char*)&version, sizeof(size_t));
    BRAYNS_INFO << "Version: " << version << std::endl;

    const size_t nbModels = _models.size();
    file.write((char*)&nbModels, sizeof(size_t));
    BRAYNS_INFO << nbModels << " models" << std::endl;
    for (const auto& model : _models)
        file.write((char*)&model.first, sizeof(size_t));

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
        bufferSize = _timestampSpheresIndices[materialId].size();
        file.write((char*)&bufferSize, sizeof(size_t));
        for (const auto& index : _timestampSpheresIndices[materialId])
        {
            file.write((char*)&index.first, sizeof(size_t));
            file.write((char*)&index.second, sizeof(size_t));
        }

        bufferSize = _serializedSpheresDataSize[materialId] *
                     Sphere::getSerializationSize() * sizeof(float);
        file.write((char*)&bufferSize, sizeof(size_t));
        file.write((char*)_serializedSpheresData[materialId].data(),
                   bufferSize);
        if (bufferSize != 0)
            BRAYNS_DEBUG << "[" << materialId << "] "
                         << _serializedSpheresDataSize[materialId] << " Spheres"
                         << std::endl;

        // Cylinders
        bufferSize = _timestampCylindersIndices[materialId].size();
        file.write((char*)&bufferSize, sizeof(size_t));
        for (const auto& index : _timestampCylindersIndices[materialId])
        {
            file.write((char*)&index.first, sizeof(size_t));
            file.write((char*)&index.second, sizeof(size_t));
        }

        bufferSize = _serializedCylindersDataSize[materialId] *
                     Cylinder::getSerializationSize() * sizeof(float);
        file.write((char*)&bufferSize, sizeof(size_t));
        file.write((char*)_serializedCylindersData[materialId].data(),
                   bufferSize);
        if (bufferSize != 0)
            BRAYNS_DEBUG << "[" << materialId << "] "
                         << _serializedCylindersDataSize[materialId]
                         << " Cylinders" << std::endl;

        // Cones
        bufferSize = _timestampConesIndices[materialId].size();
        file.write((char*)&bufferSize, sizeof(size_t));
        for (const auto& index : _timestampConesIndices[materialId])
        {
            file.write((char*)&index.first, sizeof(size_t));
            file.write((char*)&index.second, sizeof(size_t));
        }

        bufferSize = _serializedConesDataSize[materialId] *
                     Cone::getSerializationSize() * sizeof(float);
        file.write((char*)&bufferSize, sizeof(size_t));
        file.write((char*)_serializedConesData[materialId].data(), bufferSize);
        if (bufferSize != 0)
            BRAYNS_DEBUG << "[" << materialId << "] "
                         << _serializedConesDataSize[materialId] << " Cones"
                         << std::endl;

        if (_trianglesMeshes.find(materialId) != _trianglesMeshes.end())
        {
            // Vertices
            bufferSize = _trianglesMeshes[materialId].getVertices().size() *
                         sizeof(Vector3f);
            file.write((char*)&bufferSize, sizeof(size_t));
            file.write((char*)_trianglesMeshes[materialId].getVertices().data(),
                       bufferSize);
            if (bufferSize != 0)
                BRAYNS_DEBUG
                    << "[" << materialId << "] "
                    << _trianglesMeshes[materialId].getVertices().size()
                    << " Vertices" << std::endl;

            // Indices
            bufferSize = _trianglesMeshes[materialId].getIndices().size() *
                         sizeof(Vector3ui);
            file.write((char*)&bufferSize, sizeof(size_t));
            file.write((char*)_trianglesMeshes[materialId].getIndices().data(),
                       bufferSize);
            if (bufferSize != 0)
                BRAYNS_DEBUG << "[" << materialId << "] "
                             << _trianglesMeshes[materialId].getIndices().size()
                             << " Indices" << std::endl;

            // Normals
            bufferSize = _trianglesMeshes[materialId].getNormals().size() *
                         sizeof(Vector3f);
            file.write((char*)&bufferSize, sizeof(size_t));
            file.write((char*)_trianglesMeshes[materialId].getNormals().data(),
                       bufferSize);
            if (bufferSize != 0)
                BRAYNS_DEBUG << "[" << materialId << "] "
                             << _trianglesMeshes[materialId].getNormals().size()
                             << " Normals" << std::endl;

            // Texture coordinates
            bufferSize =
                _trianglesMeshes[materialId].getTextureCoordinates().size() *
                sizeof(Vector2f);
            file.write((char*)&bufferSize, sizeof(size_t));
            file.write((char*)_trianglesMeshes[materialId]
                           .getTextureCoordinates()
                           .data(),
                       bufferSize);
            if (bufferSize != 0)
                BRAYNS_DEBUG << "[" << materialId << "] "
                             << _trianglesMeshes[materialId]
                                    .getTextureCoordinates()
                                    .size()
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

void OSPRayScene::_loadCacheFile()
{
    const std::string& filename =
        _parametersManager.getGeometryParameters().getLoadCacheFile();
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

    _models.clear();
    size_t nbModels;
    file.read((char*)&nbModels, sizeof(size_t));
    BRAYNS_INFO << nbModels << " models" << std::endl;
    for (size_t model = 0; model < nbModels; ++model)
    {
        size_t ts;
        file.read((char*)&ts, sizeof(size_t));
        BRAYNS_INFO << "Model for ts " << ts << " created" << std::endl;
        _models[ts] = ospNewModel();
    }

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
        for (size_t i = 0; i < bufferSize; ++i)
        {
            size_t ts;
            file.read((char*)&ts, sizeof(size_t));
            size_t index;
            file.read((char*)&index, sizeof(size_t));
            _timestampSpheresIndices[materialId][ts] = index;
        }

        file.read((char*)&bufferSize, sizeof(size_t));
        _serializedSpheresDataSize[materialId] =
            bufferSize / (Sphere::getSerializationSize() * sizeof(float));
        if (bufferSize != 0)
        {
            BRAYNS_DEBUG << "[" << materialId << "] "
                         << _serializedSpheresDataSize[materialId] << " Spheres"
                         << std::endl;
            _serializedSpheresData[materialId].resize(bufferSize);
            file.read((char*)_serializedSpheresData[materialId].data(),
                      bufferSize);
        }
        _serializeSpheres(materialId);

        // Cylinders
        bufferSize = 0;
        file.read((char*)&bufferSize, sizeof(size_t));
        for (size_t i = 0; i < bufferSize; ++i)
        {
            size_t ts;
            file.read((char*)&ts, sizeof(size_t));
            size_t index;
            file.read((char*)&index, sizeof(size_t));
            _timestampCylindersIndices[materialId][ts] = index;
        }

        file.read((char*)&bufferSize, sizeof(size_t));
        _serializedCylindersDataSize[materialId] =
            bufferSize / (Cylinder::getSerializationSize() * sizeof(float));
        if (bufferSize != 0)
        {
            BRAYNS_DEBUG << "[" << materialId << "] "
                         << _serializedCylindersDataSize[materialId]
                         << " Cylinders" << std::endl;
            _serializedCylindersData[materialId].reserve(bufferSize);
            file.read((char*)_serializedCylindersData[materialId].data(),
                      bufferSize);
        }
        _serializeCylinders(materialId);

        // Cones
        bufferSize = 0;
        file.read((char*)&bufferSize, sizeof(size_t));
        for (size_t i = 0; i < bufferSize; ++i)
        {
            size_t ts;
            file.read((char*)&ts, sizeof(size_t));
            size_t index;
            file.read((char*)&index, sizeof(size_t));
            _timestampConesIndices[materialId][ts] = index;
        }

        file.read((char*)&bufferSize, sizeof(size_t));
        _serializedConesDataSize[materialId] =
            bufferSize / (Cone::getSerializationSize() * sizeof(float));
        if (bufferSize != 0)
        {
            BRAYNS_DEBUG << "[" << materialId << "] "
                         << _serializedConesDataSize[materialId] << " Cones"
                         << std::endl;
            _serializedConesData[materialId].reserve(bufferSize);
            file.read((char*)_serializedConesData[materialId].data(),
                      bufferSize);
        }
        _serializeCones(materialId);

        // Vertices
        _trianglesMeshes[materialId].getVertices().clear();
        bufferSize = 0;
        file.read((char*)&bufferSize, sizeof(size_t));
        for (size_t i = 0; i < bufferSize / sizeof(Vector3f); ++i)
        {
            Vector3f vertex;
            file.read((char*)&vertex, sizeof(Vector3f));
            _trianglesMeshes[materialId].getVertices().push_back(vertex);
        }

        // Indices
        _trianglesMeshes[materialId].getIndices().clear();
        bufferSize = 0;
        file.read((char*)&bufferSize, sizeof(size_t));
        for (size_t i = 0; i < bufferSize / sizeof(Vector3ui); ++i)
        {
            Vector3ui index;
            file.read((char*)&index, sizeof(Vector3ui));
            _trianglesMeshes[materialId].getIndices().push_back(index);
        }

        // Normals
        _trianglesMeshes[materialId].getNormals().clear();
        bufferSize = 0;
        file.read((char*)&bufferSize, sizeof(size_t));
        for (size_t i = 0; i < bufferSize / sizeof(Vector3f); ++i)
        {
            Vector3f normal;
            file.read((char*)&normal, sizeof(Vector3f));
            _trianglesMeshes[materialId].getNormals().push_back(normal);
        }

        // Texture coordinates
        _trianglesMeshes[materialId].getTextureCoordinates().clear();
        bufferSize = 0;
        file.read((char*)&bufferSize, sizeof(size_t));
        for (size_t i = 0; i < bufferSize / sizeof(Vector2f); ++i)
        {
            Vector2f texCoord;
            file.read((char*)&texCoord, sizeof(Vector2f));
            _trianglesMeshes[materialId].getTextureCoordinates().push_back(
                texCoord);
        }

        _buildMeshOSPGeometry(materialId);
    }

    // Scene bounds
    file.read((char*)&_bounds, sizeof(Boxf));

    BRAYNS_INFO << _bounds << std::endl;
    BRAYNS_INFO << "Scene successfully loaded" << std::endl;
    file.close();
}

void OSPRayScene::_createModel(const size_t timestamp)
{
    if (_models.find(timestamp) == _models.end())
    {
        _models[timestamp] = ospNewModel();
        BRAYNS_INFO << "Model created for timestamp " << timestamp << ": "
                    << _models[timestamp] << std::endl;
    }
}

uint64_t OSPRayScene::_serializeSpheres(const size_t materialId)
{
    const auto& geometryParameters = _parametersManager.getGeometryParameters();
    uint64_t size = 0;

    size_t count = 0;
    if (_spheres.find(materialId) != _spheres.end())
        for (const auto& sphere : _spheres[materialId])
        {
            const float ts =
                (_models.size() == 1) ? 0.f : sphere->getTimestamp();
            size += sphere->serializeData(_serializedSpheresData[materialId]);
            ++_serializedSpheresDataSize[materialId];
            _timestampSpheresIndices[materialId][ts] = ++count;
        }

    // Extended spheres
    for (const auto& timestampSpheresIndex :
         _timestampSpheresIndices[materialId])
    {
        const size_t spheresBufferSize =
            timestampSpheresIndex.second * Sphere::getSerializationSize();

        for (const auto& model : _models)
        {
            if (timestampSpheresIndex.first <= model.first)
            {
                if (_ospExtendedSpheres.find(materialId) !=
                    _ospExtendedSpheres.end())
                    ospRemoveGeometry(model.second,
                                      _ospExtendedSpheres[materialId]);

                _ospExtendedSpheres[materialId] =
                    ospNewGeometry("extendedspheres");

                _ospExtendedSpheresData[materialId] =
                    ospNewData(spheresBufferSize, OSP_FLOAT,
                               &_serializedSpheresData[materialId][0],
                               _getOSPDataFlags());

                ospSetObject(_ospExtendedSpheres[materialId], "extendedspheres",
                             _ospExtendedSpheresData[materialId]);
                ospSet1i(_ospExtendedSpheres[materialId],
                         "bytes_per_extended_sphere",
                         Sphere::getSerializationSize() * sizeof(float));
                ospSet1i(_ospExtendedSpheres[materialId], "materialID",
                         materialId);
                ospSet1i(_ospExtendedSpheres[materialId], "offset_radius",
                         3 * sizeof(float));
                ospSet1i(_ospExtendedSpheres[materialId], "offset_timestamp",
                         4 * sizeof(float));
                ospSet1i(_ospExtendedSpheres[materialId], "offset_value_x",
                         5 * sizeof(float));
                ospSet1i(_ospExtendedSpheres[materialId], "offset_value_y",
                         6 * sizeof(float));

                if (_ospMaterials[materialId])
                    ospSetMaterial(_ospExtendedSpheres[materialId],
                                   _ospMaterials[materialId]);

                ospCommit(_ospExtendedSpheres[materialId]);

                if (geometryParameters.getCircuitUseSimulationModel())
                    ospAddGeometry(_simulationModel,
                                   _ospExtendedSpheres[materialId]);
                else
                    ospAddGeometry(model.second,
                                   _ospExtendedSpheres[materialId]);
            }
        }
    }
    return size;
}

uint64_t OSPRayScene::_serializeCylinders(const size_t materialId)
{
    const auto& geometryParameters = _parametersManager.getGeometryParameters();
    uint64_t size = 0;

    size_t count = 0;
    if (_cylinders.find(materialId) != _cylinders.end())
        for (const auto& cylinder : _cylinders[materialId])
        {
            const float ts =
                (_models.size() == 1) ? 0.f : cylinder->getTimestamp();
            size +=
                cylinder->serializeData(_serializedCylindersData[materialId]);
            ++_serializedCylindersDataSize[materialId];
            _timestampCylindersIndices[materialId][ts] = ++count;
        }

    // Extended cylinders
    for (const auto& timestampCylindersIndex :
         _timestampCylindersIndices[materialId])
    {
        const size_t cylindersBufferSize =
            timestampCylindersIndex.second * Cylinder::getSerializationSize();

        for (const auto& model : _models)
        {
            if (timestampCylindersIndex.first <= model.first)
            {
                if (_ospExtendedCylinders.find(materialId) !=
                    _ospExtendedCylinders.end())
                    ospRemoveGeometry(model.second,
                                      _ospExtendedCylinders[materialId]);

                _ospExtendedCylinders[materialId] =
                    ospNewGeometry("extendedcylinders");
                assert(_ospExtendedCylinders[materialId]);

                _ospExtendedCylindersData[materialId] =
                    ospNewData(cylindersBufferSize, OSP_FLOAT,
                               &_serializedCylindersData[materialId][0],
                               _getOSPDataFlags());

                ospSet1i(_ospExtendedCylinders[materialId], "materialID",
                         materialId);
                ospSetObject(_ospExtendedCylinders[materialId],
                             "extendedcylinders",
                             _ospExtendedCylindersData[materialId]);
                ospSet1i(_ospExtendedCylinders[materialId],
                         "bytes_per_extended_cylinder",
                         Cylinder::getSerializationSize() * sizeof(float));
                ospSet1i(_ospExtendedCylinders[materialId], "offset_timestamp",
                         7 * sizeof(float));
                ospSet1i(_ospExtendedCylinders[materialId], "offset_value_x",
                         8 * sizeof(float));
                ospSet1i(_ospExtendedCylinders[materialId], "offset_value_y",
                         9 * sizeof(float));

                if (_ospMaterials[materialId])
                    ospSetMaterial(_ospExtendedCylinders[materialId],
                                   _ospMaterials[materialId]);

                ospCommit(_ospExtendedCylinders[materialId]);
                if (geometryParameters.getCircuitUseSimulationModel())
                    ospAddGeometry(_simulationModel,
                                   _ospExtendedCylinders[materialId]);
                else
                    ospAddGeometry(model.second,
                                   _ospExtendedCylinders[materialId]);
            }
        }
    }
    return size;
}

uint64_t OSPRayScene::_serializeCones(const size_t materialId)
{
    const auto& geometryParameters = _parametersManager.getGeometryParameters();
    uint64_t size = 0;

    size_t count = 0;
    if (_cones.find(materialId) != _cones.end())
        for (const auto& cone : _cones[materialId])
        {
            const float ts = (_models.size() == 1) ? 0.f : cone->getTimestamp();
            size += cone->serializeData(_serializedConesData[materialId]);
            ++_serializedConesDataSize[materialId];
            _timestampConesIndices[materialId][ts] = ++count;
        }

    // Extended cones
    for (const auto& timestampConesIndex : _timestampConesIndices[materialId])
    {
        const size_t conesBufferSize =
            timestampConesIndex.second * Cone::getSerializationSize();

        for (const auto& model : _models)
        {
            if (timestampConesIndex.first <= model.first)
            {
                if (_ospExtendedCones.find(materialId) !=
                    _ospExtendedCones.end())
                    ospRemoveGeometry(model.second,
                                      _ospExtendedCones[materialId]);

                _ospExtendedCones[materialId] = ospNewGeometry("extendedcones");
                assert(_ospExtendedCones[materialId]);

                _ospExtendedConesData[materialId] =
                    ospNewData(conesBufferSize, OSP_FLOAT,
                               &_serializedConesData[materialId][0],
                               _getOSPDataFlags());

                ospSet1i(_ospExtendedCones[materialId], "materialID",
                         materialId);
                ospSetObject(_ospExtendedCones[materialId], "extendedcones",
                             _ospExtendedConesData[materialId]);
                ospSet1i(_ospExtendedCones[materialId],
                         "bytes_per_extended_cone",
                         Cone::getSerializationSize() * sizeof(float));
                ospSet1i(_ospExtendedCones[materialId], "offset_timestamp",
                         8 * sizeof(float));
                ospSet1i(_ospExtendedCones[materialId], "offset_value_x",
                         9 * sizeof(float));
                ospSet1i(_ospExtendedCones[materialId], "offset_value_y",
                         10 * sizeof(float));

                if (_ospMaterials[materialId])
                    ospSetMaterial(_ospExtendedCones[materialId],
                                   _ospMaterials[materialId]);

                ospCommit(_ospExtendedCones[materialId]);
                if (geometryParameters.getCircuitUseSimulationModel())
                    ospAddGeometry(_simulationModel,
                                   _ospExtendedCones[materialId]);
                else
                    ospAddGeometry(model.second, _ospExtendedCones[materialId]);
            }
        }
    }
    return size;
}

uint64_t OSPRayScene::serializeGeometry()
{
    uint64_t size = 0;

    if (_spheresDirty)
    {
        _serializedSpheresDataSize.clear();
        _timestampSpheresIndices.clear();
        _serializedSpheresData.clear();
        for (auto& material : _materials)
        {
            const auto materialId = material.first;
            _serializedSpheresDataSize[materialId] = 0;
            size += _serializeSpheres(materialId);
        }
        _spheresDirty = false;
    }

    if (_cylindersDirty)
    {
        _serializedCylindersData.clear();
        _serializedCylindersDataSize.clear();
        _timestampCylindersIndices.clear();
        for (auto& material : _materials)
        {
            const auto materialId = material.first;
            _serializedCylindersDataSize[materialId] = 0;
            size += _serializeCylinders(materialId);
        }
        _cylindersDirty = false;
    }

    if (_conesDirty)
    {
        _serializedConesData.clear();
        _serializedConesDataSize.clear();
        _timestampConesIndices.clear();
        for (auto& material : _materials)
        {
            const auto materialId = material.first;
            _serializedConesDataSize[materialId] = 0;
            size += _serializeCones(materialId);
        }
        _conesDirty = false;
    }

    // Triangle meshes
    if (_trianglesMeshesDirty)
    {
        for (auto& material : _materials)
            size += _buildMeshOSPGeometry(material.first);
        _trianglesMeshesDirty = false;
    }
    return size;
}

void OSPRayScene::buildGeometry()
{
    BRAYNS_INFO << "Building OSPRay geometry" << std::endl;

    commitMaterials();

    if (_parametersManager.getGeometryParameters().getGenerateMultipleModels())
        // Initialize models according to timestamps
        for (auto& material : _materials)
        {
            for (const auto& sphere : _spheres[material.first])
                _createModel(sphere->getTimestamp());
            for (const auto& cylinder : _cylinders[material.first])
                _createModel(cylinder->getTimestamp());
            for (const auto& cone : _cones[material.first])
                _createModel(cone->getTimestamp());
        }

    if (_models.size() == 0)
        // If no timestamp is available, create a default model at timestamp 0
        _models[0] = ospNewModel();

    if (_parametersManager.getGeometryParameters()
            .getCircuitUseSimulationModel())
        _simulationModel = ospNewModel();

    BRAYNS_INFO << "Models to process: " << _models.size() << std::endl;

    uint64_t size = serializeGeometry();
    commitLights();

    if (!_parametersManager.getGeometryParameters().getLoadCacheFile().empty())
        _loadCacheFile();

    size_t totalNbSpheres = 0;
    size_t totalNbCylinders = 0;
    size_t totalNbCones = 0;
    size_t totalNbVertices = 0;
    size_t totalNbIndices = 0;
    for (auto& material : _materials)
    {
        const auto materialId = material.first;
        totalNbSpheres += _serializedSpheresDataSize[materialId];
        totalNbCylinders += _serializedCylindersDataSize[materialId];
        totalNbCones += _serializedConesDataSize[materialId];
        if (_trianglesMeshes.find(materialId) != _trianglesMeshes.end())
        {
            totalNbVertices +=
                _trianglesMeshes[materialId].getVertices().size();
            totalNbIndices += _trianglesMeshes[materialId].getIndices().size();
        }
    }

    BRAYNS_INFO << "---------------------------------------------------"
                << std::endl;
    BRAYNS_INFO << "Geometry information" << std::endl;
    BRAYNS_INFO << "Spheres  : " << totalNbSpheres << std::endl;
    BRAYNS_INFO << "Cylinders: " << totalNbCylinders << std::endl;
    BRAYNS_INFO << "Cones    : " << totalNbCones << std::endl;
    BRAYNS_INFO << "Vertices : " << totalNbVertices << std::endl;
    BRAYNS_INFO << "Indices  : " << totalNbIndices << std::endl;
    BRAYNS_INFO << "Materials: " << _materials.size() << std::endl;
    BRAYNS_INFO << "Total    : " << size << " bytes" << std::endl;
    BRAYNS_INFO << "---------------------------------------------------"
                << std::endl;

    if (!_parametersManager.getGeometryParameters().getSaveCacheFile().empty())
        _saveCacheFile();
}

uint64_t OSPRayScene::_buildMeshOSPGeometry(const size_t materialId)
{
    uint64_t size = 0;
    // Triangle meshes
    if (_trianglesMeshes.find(materialId) != _trianglesMeshes.end())
    {
        _ospMeshes[materialId] = ospNewGeometry("trianglemesh");
        assert(_ospMeshes[materialId]);

        size += _trianglesMeshes[materialId].getVertices().size() * 3 *
                sizeof(float);
        OSPData vertices =
            ospNewData(_trianglesMeshes[materialId].getVertices().size(),
                       OSP_FLOAT3,
                       &_trianglesMeshes[materialId].getVertices()[0],
                       _getOSPDataFlags());

        if (!_trianglesMeshes[materialId].getNormals().empty())
        {
            size += _trianglesMeshes[materialId].getNormals().size() * 3 *
                    sizeof(float);
            OSPData normals =
                ospNewData(_trianglesMeshes[materialId].getNormals().size(),
                           OSP_FLOAT3,
                           &_trianglesMeshes[materialId].getNormals()[0],
                           _getOSPDataFlags());
            ospSetObject(_ospMeshes[materialId], "vertex.normal", normals);
        }

        size +=
            _trianglesMeshes[materialId].getIndices().size() * 3 * sizeof(int);
        OSPData indices =
            ospNewData(_trianglesMeshes[materialId].getIndices().size(),
                       OSP_INT3, &_trianglesMeshes[materialId].getIndices()[0],
                       _getOSPDataFlags());

        if (!_trianglesMeshes[materialId].getColors().empty())
        {
            size += _trianglesMeshes[materialId].getColors().size() * 4 *
                    sizeof(float);
            OSPData colors =
                ospNewData(_trianglesMeshes[materialId].getColors().size(),
                           OSP_FLOAT3A,
                           &_trianglesMeshes[materialId].getColors()[0],
                           _getOSPDataFlags());
            ospSetObject(_ospMeshes[materialId], "vertex.color", colors);
            ospRelease(colors);
        }

        if (!_trianglesMeshes[materialId].getTextureCoordinates().empty())
        {
            size +=
                _trianglesMeshes[materialId].getTextureCoordinates().size() *
                2 * sizeof(float);
            OSPData texCoords = ospNewData(
                _trianglesMeshes[materialId].getTextureCoordinates().size(),
                OSP_FLOAT2,
                &_trianglesMeshes[materialId].getTextureCoordinates()[0],
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
        for (const auto& model : _models)
            ospAddGeometry(model.second, _ospMeshes[materialId]);
    }
    return size;
}

void OSPRayScene::commitLights()
{
    for (auto renderer : _renderers)
    {
        OSPRayRenderer* osprayRenderer =
            dynamic_cast<OSPRayRenderer*>(renderer.get());

        size_t lightCount = 0;
        for (auto light : _lights)
        {
            DirectionalLight* directionalLight =
                dynamic_cast<DirectionalLight*>(light.get());
            if (directionalLight != 0)
            {
                if (_ospLights.size() <= lightCount)
                    _ospLights.push_back(ospNewLight(osprayRenderer->impl(),
                                                     "DirectionalLight"));

                const Vector3f color = directionalLight->getColor();
                ospSet3f(_ospLights[lightCount], "color", color.x(), color.y(),
                         color.z());
                const Vector3f direction = directionalLight->getDirection();
                ospSet3f(_ospLights[lightCount], "direction", direction.x(),
                         direction.y(), direction.z());
                ospSet1f(_ospLights[lightCount], "intensity",
                         directionalLight->getIntensity());
                ospCommit(_ospLights[lightCount]);
            }
            else
            {
                PointLight* pointLight = dynamic_cast<PointLight*>(light.get());
                if (pointLight != 0)
                {
                    if (_ospLights.size() <= lightCount)
                        _ospLights.push_back(
                            ospNewLight(osprayRenderer->impl(), "PointLight"));

                    const Vector3f position = pointLight->getPosition();
                    ospSet3f(_ospLights[lightCount], "position", position.x(),
                             position.y(), position.z());
                    const Vector3f color = pointLight->getColor();
                    ospSet3f(_ospLights[lightCount], "color", color.x(),
                             color.y(), color.z());
                    ospSet1f(_ospLights[lightCount], "intensity",
                             pointLight->getIntensity());
                    ospSet1f(_ospLights[lightCount], "radius",
                             pointLight->getCutoffDistance());
                    ospCommit(_ospLights[lightCount]);
                }
            }
            ++lightCount;
        }

        if (_ospLightData == 0)
        {
            _ospLightData = ospNewData(_lights.size(), OSP_OBJECT,
                                       &_ospLights[0], _getOSPDataFlags());
            ospCommit(_ospLightData);
        }
        ospSetData(osprayRenderer->impl(), "lights", _ospLightData);
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
            for (const auto& renderer : _renderers)
            {
                OSPRayRenderer* osprayRenderer =
                    dynamic_cast<OSPRayRenderer*>(renderer.get());
                _ospMaterials.push_back(ospNewMaterial(osprayRenderer->impl(),
                                                       "ExtendedOBJMaterial"));
            }

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
        ospSet1f(ospMaterial, "g", material.second.getGlossiness());

        if (!updateOnly)
        {
            // Textures
            for (auto texture : material.second.getTextures())
            {
                TextureLoader textureLoader;
                if (texture.second != TEXTURE_NAME_SIMULATION)
                    textureLoader.loadTexture(_textures, texture.first,
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
    }
}

void OSPRayScene::commitVolumeData()
{
    VolumeHandlerPtr volumeHandler = getVolumeHandler();
    if (!volumeHandler)
        return;

    const float timestamp =
        _parametersManager.getSceneParameters().getTimestamp();
    volumeHandler->setTimestamp(timestamp);
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
}

void OSPRayScene::commitSimulationData()
{
    if (!_simulationHandler)
        return;

    const float timestamp =
        _parametersManager.getSceneParameters().getTimestamp();

    if (_simulationHandler->getTimestamp() == timestamp)
        return;

    _simulationHandler->setTimestamp(timestamp);
    auto frameData = _simulationHandler->getFrameData();

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

void OSPRayScene::saveSceneToCacheFile()
{
    _saveCacheFile();
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
