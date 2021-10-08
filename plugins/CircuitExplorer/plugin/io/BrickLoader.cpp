/* Copyright (c) 2018-2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/favreau/Brayns-UC-CircuitExplorer>
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

#include "BrickLoader.h"
#include "MorphologyLoader.h"
#include "SpikeSimulationHandler.h"
#include "VoltageSimulationHandler.h"
#include <common/commonTypes.h>
#include <common/log.h>
#include <common/types.h>

#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>

#include <brain/brain.h>
#include <brion/brion.h>

#include <fstream>

namespace
{
const size_t CACHE_VERSION_1 = 1;
const size_t CACHE_VERSION_2 = 2;
const size_t CACHE_VERSION_3 = 3;
const size_t CACHE_VERSION_4 = 4;

const std::string LOADER_NAME = "Pre-computed brick loader";
const std::string SUPPORTED_EXTENTION_BRAYNS = "brayns";
const std::string SUPPORTED_EXTENTION_BIN = "bin";

const brayns::Property PROP_LOAD_SPHERES = {"spheres", true, {"Load spheres"}};
const brayns::Property PROP_LOAD_CYLINDERS = {"cylinders",
                                              true,
                                              {"Load cylinders"}};
const brayns::Property PROP_LOAD_CONES = {"cones", true, {"Load cones"}};
const brayns::Property PROP_LOAD_MESHES = {"meshes", true, {"Load meshes"}};
const brayns::Property PROP_LOAD_STREAMLINES = {"streamlines",
                                                true,
                                                {"Load streamlines"}};
const brayns::Property PROP_LOAD_SDF = {
    "sdf", true, {"Load signed distance field geometry"}};
const brayns::Property PROP_LOAD_SIMULATION = {
    "simulation", true, {"Attach simulation data (if applicable"}};
} // namespace

BrickLoader::BrickLoader(brayns::Scene& scene,
                         brayns::PropertyMap&& loaderParams)
    : Loader(scene)
    , _defaults(loaderParams)
{
    PLUGIN_INFO << "Registering " << LOADER_NAME << std::endl;
}

std::string BrickLoader::getName() const
{
    return LOADER_NAME;
}

std::vector<std::string> BrickLoader::getSupportedExtensions() const
{
    return {SUPPORTED_EXTENTION_BRAYNS, SUPPORTED_EXTENTION_BIN};
}

bool BrickLoader::isSupported(const std::string& /*filename*/,
                              const std::string& extension) const
{
    const std::set<std::string> types = {SUPPORTED_EXTENTION_BRAYNS,
                                         SUPPORTED_EXTENTION_BIN};
    return types.find(extension) != types.end();
}

std::vector<brayns::ModelDescriptorPtr> BrickLoader::importFromBlob(
    brayns::Blob&& /*blob*/, const brayns::LoaderProgress& /*callback*/,
    const brayns::PropertyMap& /*properties*/) const
{
    throw std::runtime_error("Loading circuit from blob is not supported");
}

std::string BrickLoader::_readString(std::ifstream& f) const
{
    size_t size;
    f.read((char*)&size, sizeof(size_t));
    char* str = new char[size + 1];
    f.read(str, size);
    str[size] = 0;
    std::string s{str};
    delete[] str;
    return s;
}

std::vector<brayns::ModelDescriptorPtr> BrickLoader::importFromFile(
    const std::string& filename, const brayns::LoaderProgress& callback,
    const brayns::PropertyMap& properties) const
{
    brayns::PropertyMap props = _defaults;
    props.merge(properties);

    callback.updateProgress("Loading cache...", 0);
    PLUGIN_INFO << "Loading model from cache file: " << filename << std::endl;
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.good())
    {
        const std::string msg = "Could not open cache file " + filename;
        PLUGIN_THROW(msg);
    }

    // File version
    size_t version;
    file.read((char*)&version, sizeof(size_t));

    PLUGIN_INFO << "Version: " << version << std::endl;

    auto model = _scene.createModel();

    // Geometry
    size_t nbSpheres = 0;
    size_t nbCylinders = 0;
    size_t nbCones = 0;
    size_t nbMeshes = 0;
    size_t nbVertices = 0;
    size_t nbIndices = 0;
    size_t nbNormals = 0;
    size_t nbTexCoords = 0;

    // Metadata
    size_t nbElements;
    brayns::ModelMetadata metadata;
    file.read((char*)&nbElements, sizeof(size_t));
    for (size_t i = 0; i < nbElements; ++i)
        metadata[_readString(file)] = _readString(file);

    size_t nbMaterials;
    file.read((char*)&nbMaterials, sizeof(size_t));

    // Materials
    size_t materialId;
    for (size_t i = 0; i < nbMaterials; ++i)
    {
        callback.updateProgress("Materials (" + std::to_string(i + 1) + "/" +
                                    std::to_string(nbMaterials) + ")",
                                0.1f * float(i) / float(nbMaterials));
        file.read((char*)&materialId, sizeof(size_t));

        brayns::PropertyMap materialProps;
        auto name = _readString(file);
        materialProps.add({MATERIAL_PROPERTY_CAST_USER_DATA, false});
        materialProps.add({MATERIAL_PROPERTY_SHADING_MODE,
                           static_cast<int32_t>(MaterialShadingMode::diffuse)});

        auto material = model->createMaterial(materialId, name, materialProps);

        brayns::Vector3f value3f;
        file.read((char*)&value3f, sizeof(brayns::Vector3f));
        material->setDiffuseColor(value3f);
        file.read((char*)&value3f, sizeof(brayns::Vector3f));
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

        if (version == CACHE_VERSION_1)
        {
            bool userData;
            file.read((char*)&userData, sizeof(bool));
            material->updateProperty(MATERIAL_PROPERTY_CAST_USER_DATA,
                                     static_cast<int32_t>(userData));

            size_t shadingMode;
            file.read((char*)&shadingMode, sizeof(size_t));
            material->updateProperty(MATERIAL_PROPERTY_SHADING_MODE,
                                     static_cast<int32_t>(shadingMode));
        }

        if (version >= CACHE_VERSION_2)
        {
            int32_t userData;
            file.read((char*)&userData, sizeof(int32_t));
            material->updateProperty(MATERIAL_PROPERTY_CAST_USER_DATA,
                                     static_cast<bool>(userData));

            int32_t shadingMode;
            file.read((char*)&shadingMode, sizeof(int32_t));
            material->updateProperty(MATERIAL_PROPERTY_SHADING_MODE,
                                     shadingMode);
        }

        if (version == CACHE_VERSION_3)
        {
            bool clipped;
            file.read((char*)&clipped, sizeof(bool));
            material->updateProperty(MATERIAL_PROPERTY_CLIPPING_MODE, clipped);
        }

        if (version >= CACHE_VERSION_4)
        {
            int32_t clippingMode;
            file.read((char*)&clippingMode, sizeof(int32_t));
            material->updateProperty(MATERIAL_PROPERTY_CLIPPING_MODE,
                                     clippingMode);
        }
    }

    uint64_t bufferSize{0};

    // Spheres
    callback.updateProgress("Spheres", 0.2f);
    file.read((char*)&nbSpheres, sizeof(size_t));
    for (size_t i = 0; i < nbSpheres; ++i)
    {
        file.read((char*)&materialId, sizeof(size_t));
        file.read((char*)&nbElements, sizeof(size_t));

        struct SphereV1
        {
            brayns::Vector3f center;
            float radius;
            float timestamp;
            float value;
        };

        if (props[PROP_LOAD_SPHERES.getName()].as<bool>())
        {
            callback.updateProgress("Spheres (" + std::to_string(i + 1) + "/" +
                                        std::to_string(nbSpheres) + ")",
                                    0.2f + 0.1f * float(i) / float(nbSpheres));
            auto& spheres = model->getSpheres()[materialId];
            spheres.resize(nbElements);

            if (version >= CACHE_VERSION_2)
            {
                bufferSize = nbElements * sizeof(brayns::Sphere);
                file.read((char*)spheres.data(), bufferSize);
            }
            else
            {
                std::vector<SphereV1> spheresV1;
                spheresV1.resize(nbElements);
                bufferSize = nbElements * sizeof(SphereV1);
                file.read((char*)spheresV1.data(), bufferSize);
                for (uint64_t s = 0; s < spheresV1.size(); ++s)
                    spheres[s] = {spheresV1[i].center, spheresV1[i].radius};
            }
        }
        else
        {
            if (version >= CACHE_VERSION_2)
                bufferSize = nbElements * sizeof(brayns::Sphere);
            else
                bufferSize = nbElements * sizeof(SphereV1);
            file.ignore(bufferSize);
        }
    }

    // Cylinders
    file.read((char*)&nbCylinders, sizeof(size_t));
    for (size_t i = 0; i < nbCylinders; ++i)
    {
        file.read((char*)&materialId, sizeof(size_t));
        file.read((char*)&nbElements, sizeof(size_t));

        struct CylinderV1
        {
            brayns::Vector3f center;
            brayns::Vector3f up;
            float radius;
            float timestamp;
            float value;
        };

        if (props[PROP_LOAD_CYLINDERS.getName()].as<bool>())
        {
            callback.updateProgress("Cylinders (" + std::to_string(i + 1) +
                                        "/" + std::to_string(nbCylinders) + ")",
                                    0.3f +
                                        0.1f * float(i) / float(nbCylinders));
            auto& cylinders = model->getCylinders()[materialId];
            cylinders.resize(nbElements);
            if (version >= CACHE_VERSION_2)
            {
                bufferSize = nbElements * sizeof(brayns::Cylinder);
                file.read((char*)cylinders.data(), bufferSize);
            }
            else
            {
                std::vector<CylinderV1> cylindersV1(nbElements);
                bufferSize = nbElements * sizeof(CylinderV1);
                file.read((char*)cylindersV1.data(), bufferSize);
                for (uint64_t s = 0; s < cylindersV1.size(); ++s)
                    cylinders[s] = {cylindersV1[i].center, cylindersV1[i].up,
                                    cylindersV1[i].radius};
            }
        }
        else
        {
            if (version >= CACHE_VERSION_2)
                bufferSize = nbElements * sizeof(brayns::Cylinder);
            else
                bufferSize = nbElements * sizeof(CylinderV1);
            file.ignore(bufferSize);
        }
    }

    // Cones
    file.read((char*)&nbCones, sizeof(size_t));
    for (size_t i = 0; i < nbCones; ++i)
    {
        file.read((char*)&materialId, sizeof(size_t));
        file.read((char*)&nbElements, sizeof(size_t));

        struct ConeV1
        {
            brayns::Vector3f center;
            brayns::Vector3f up;
            float centerRadius;
            float upRadius;
            float timestamp;
            float value;
        };

        if (props[PROP_LOAD_CONES.getName()].as<bool>())
        {
            callback.updateProgress("Cones (" + std::to_string(i + 1) + "/" +
                                        std::to_string(nbCones) + ")",
                                    0.4f + 0.1f * float(i) / float(nbCones));
            auto& cones = model->getCones()[materialId];
            cones.resize(nbElements);
            if (version >= CACHE_VERSION_2)
            {
                bufferSize = nbElements * sizeof(brayns::Cone);
                file.read((char*)cones.data(), bufferSize);
            }
            else
            {
                std::vector<ConeV1> conesV1(nbElements);
                bufferSize = nbElements * sizeof(ConeV1);
                file.read((char*)conesV1.data(), bufferSize);
                for (uint64_t s = 0; s < conesV1.size(); ++s)
                    cones[s] = {conesV1[i].center, conesV1[i].up,
                                conesV1[i].centerRadius, conesV1[i].upRadius};
            }
        }
        else
        {
            if (version >= CACHE_VERSION_2)
                bufferSize = nbElements * sizeof(brayns::Cone);
            else
                bufferSize = nbElements * sizeof(ConeV1);

            file.ignore(bufferSize);
        }
    }

    // Meshes
    bool load = props[PROP_LOAD_MESHES.getName()].as<bool>();
    file.read((char*)&nbMeshes, sizeof(size_t));
    for (size_t i = 0; i < nbMeshes; ++i)
    {
        file.read((char*)&materialId, sizeof(size_t));
        auto& meshes = model->getTriangleMeshes()[materialId];
        // Vertices
        file.read((char*)&nbVertices, sizeof(size_t));
        if (nbVertices != 0)
        {
            bufferSize = nbVertices * sizeof(brayns::Vector3f);
            if (load)
            {
                callback.updateProgress("Meshes (" + std::to_string(i + 1) +
                                            "/" + std::to_string(nbMeshes) +
                                            ")",
                                        0.5f +
                                            0.1f * float(i) / float(nbMeshes));
                meshes.vertices.resize(nbVertices);
                file.read((char*)meshes.vertices.data(), bufferSize);
            }
            else
                file.ignore(bufferSize);
        }

        // Indices
        file.read((char*)&nbIndices, sizeof(size_t));
        if (nbIndices != 0)
        {
            bufferSize = nbIndices * sizeof(brayns::Vector3ui);
            if (load)
            {
                meshes.indices.resize(nbIndices);
                file.read((char*)meshes.indices.data(), bufferSize);
            }
            else
                file.ignore(bufferSize);
        }

        // Normals
        file.read((char*)&nbNormals, sizeof(size_t));
        if (nbNormals != 0)
        {
            bufferSize = nbNormals * sizeof(brayns::Vector3f);
            if (load)
            {
                meshes.normals.resize(nbNormals);
                file.read((char*)meshes.normals.data(), bufferSize);
            }
            else
                file.ignore(bufferSize);
        }

        // Texture coordinates
        file.read((char*)&nbTexCoords, sizeof(size_t));
        if (nbTexCoords != 0)
        {
            bufferSize = nbTexCoords * sizeof(brayns::Vector2f);
            if (load)
            {
                meshes.textureCoordinates.resize(nbTexCoords);
                file.read((char*)meshes.textureCoordinates.data(), bufferSize);
            }
            else
                file.ignore(bufferSize);
        }
    }

    // Streamlines
    load = props[PROP_LOAD_STREAMLINES.getName()].as<bool>();
    size_t nbStreamlines;
    auto& streamlines = model->getStreamlines();
    file.read((char*)&nbStreamlines, sizeof(size_t));
    for (size_t i = 0; i < nbStreamlines; ++i)
    {
        brayns::StreamlinesData streamlineData;
        // Id
        size_t id;
        file.read((char*)&id, sizeof(size_t));

        // Vertex
        file.read((char*)&nbElements, sizeof(size_t));
        bufferSize = nbElements * sizeof(brayns::Vector4f);
        if (load)
        {
            callback.updateProgress("Streamlines (" + std::to_string(i + 1) +
                                        "/" + std::to_string(nbStreamlines) +
                                        ")",
                                    0.6f +
                                        0.1f * float(i) / float(nbStreamlines));
            streamlineData.vertex.resize(nbElements);
            file.read((char*)streamlineData.vertex.data(), bufferSize);
        }
        else
            file.ignore(bufferSize);

        // Vertex Color
        file.read((char*)&nbElements, sizeof(size_t));
        bufferSize = nbElements * sizeof(brayns::Vector4f);
        if (load)
        {
            streamlineData.vertexColor.resize(nbElements);
            file.read((char*)streamlineData.vertexColor.data(), bufferSize);
        }
        else
            file.ignore(bufferSize);

        // Indices
        file.read((char*)&nbElements, sizeof(size_t));
        bufferSize = nbElements * sizeof(int32_t);
        if (load)
        {
            streamlineData.indices.resize(nbElements);
            file.read((char*)streamlineData.indices.data(), bufferSize);
        }
        else
            file.ignore(bufferSize);

        streamlines[id] = streamlineData;
    }

    // SDF geometry
    load = props[PROP_LOAD_SDF.getName()].as<bool>();
    auto& sdfData = model->getSDFGeometryData();
    file.read((char*)&nbElements, sizeof(size_t));

    if (nbElements > 0)
    {
        // Geometries
        sdfData.geometries.resize(nbElements);
        bufferSize = nbElements * sizeof(brayns::SDFGeometry);
        file.read((char*)sdfData.geometries.data(), bufferSize);

        // SDF Indices
        file.read((char*)&nbElements, sizeof(size_t));
        for (size_t i = 0; i < nbElements; ++i)
        {
            file.read((char*)&materialId, sizeof(size_t));
            size_t size;
            file.read((char*)&size, sizeof(size_t));
            bufferSize = size * sizeof(uint64_t);
            if (load)
            {
                callback.updateProgress("SDF geometries indices (" +
                                            std::to_string(i + 1) + "/" +
                                            std::to_string(nbElements) + ")",
                                        0.8f + 0.1f * float(i) /
                                                   float(nbElements));
                sdfData.geometryIndices[materialId].resize(size);
                file.read((char*)sdfData.geometryIndices[materialId].data(),
                          bufferSize);
            }
            else
                file.ignore(bufferSize);
        }

        // Neighbours
        file.read((char*)&nbElements, sizeof(size_t));
        sdfData.neighbours.resize(nbElements);

        if (load)
            callback.updateProgress("SDF geometries neighbours", 0.9f);

        for (size_t i = 0; i < nbElements; ++i)
        {
            size_t size;
            file.read((char*)&size, sizeof(size_t));
            bufferSize = size * sizeof(uint64_t);
            if (load)
            {
                sdfData.neighbours[i].resize(size);
                file.read((char*)sdfData.neighbours[i].data(), bufferSize);
            }
            else
                file.ignore(bufferSize);
        }

        // Neighbours flat
        file.read((char*)&nbElements, sizeof(size_t));
        bufferSize = nbElements * sizeof(uint64_t);
        if (load)
        {
            sdfData.neighboursFlat.resize(nbElements);
            file.read((char*)sdfData.neighboursFlat.data(), bufferSize);
        }
        else
            file.ignore(bufferSize);
    }

    load = props[PROP_LOAD_SIMULATION.getName()].as<bool>();
    if (version >= CACHE_VERSION_3 && load)
    {
        // Simulation Handler
        size_t reportType{0};
        file.read((char*)&reportType, sizeof(size_t));

        switch (static_cast<ReportType>(reportType))
        {
        case ReportType::voltages_from_file:
        {
            // Report path
            const auto reportPath = _readString(file);

            // GIDs
            file.read((char*)&nbElements, sizeof(size_t));
            brion::GIDSet gids;
            for (uint32_t i = 0; i < nbElements; ++i)
            {
                uint32_t gid;
                file.read((char*)&gid, sizeof(uint32_t));
                gids.insert(gid);
            }

            // Synchronization
            bool synchronized{false};
            file.read((char*)&synchronized, sizeof(bool));

            // Handler
            auto handler =
                std::make_shared<VoltageSimulationHandler>(reportPath, gids,
                                                           synchronized);
            model->setSimulationHandler(handler);
            break;
        }
        case ReportType::spikes:
        {
            // Report path
            const auto reportPath = _readString(file);

            // GIDs
            file.read((char*)&nbElements, sizeof(size_t));
            brion::GIDSet gids;
            for (uint32_t i = 0; i < nbElements; ++i)
            {
                uint32_t gid;
                file.read((char*)&gid, sizeof(uint32_t));
                gids.insert(gid);
            }

            // Handler
            auto handler =
                std::make_shared<SpikeSimulationHandler>(reportPath, gids);
            model->setSimulationHandler(handler);
            break;
        }
        default:
        {
            // No report in that brick!
        }
        }

        // Transfer function
        file.read((char*)&nbElements, sizeof(size_t));
        if (nbElements == 1)
        {
            auto& tf = _scene.getTransferFunction();
            // Values range
            brayns::Vector2d valuesRange;
            file.read((char*)&valuesRange, sizeof(brayns::Vector2d));
            tf.setValuesRange(valuesRange);

            // Control points
            file.read((char*)&nbElements, sizeof(size_t));
            brayns::Vector2ds controlPoints(nbElements);
            file.read((char*)&controlPoints[0],
                      nbElements * sizeof(brayns::Vector2d));
            tf.setControlPoints(controlPoints);

            // Color map
            brayns::ColorMap colorMap;
            colorMap.name = _readString(file);
            file.read((char*)&nbElements, sizeof(size_t));
            auto& colors = colorMap.colors;
            colors.resize(nbElements);
            file.read((char*)&colors[0], nbElements * sizeof(brayns::Vector3f));
            tf.setColorMap(colorMap);
        }
    }
    callback.updateProgress("Done", 1.f);

    file.close();

    // Restore original circuit config file from cache metadata, if present
    std::string path = filename;
    auto cpIt = metadata.find("CircuitPath");
    if (cpIt != metadata.end())
        path = cpIt->second;

    auto modelDescriptor =
        std::make_shared<brayns::ModelDescriptor>(std::move(model), "Brick",
                                                  path, metadata);
    return {modelDescriptor};
}

void BrickLoader::exportToFile(const brayns::ModelDescriptorPtr modelDescriptor,
                               const std::string& filename)
{
    exportToFile(modelDescriptor, filename);
}

void BrickLoader::exportToFile(brayns::ModelDescriptor& modelDescriptor,
                               const std::string& filename)
{
    PLUGIN_INFO << "Saving model to cache file: " << filename << std::endl;
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file.good())
    {
        const std::string msg = "Could not open cache file " + filename;
        PLUGIN_THROW(msg);
    }

    const size_t version = CACHE_VERSION_3;
    file.write((char*)&version, sizeof(size_t));

    // Save geometry
    auto& model = modelDescriptor.getModel();
    uint64_t bufferSize{0};

    // Metadata
    auto metadata = modelDescriptor.getMetadata();
    size_t nbElements = metadata.size();
    file.write((char*)&nbElements, sizeof(size_t));
    for (const auto& data : metadata)
    {
        size_t size = data.first.length();
        file.write((char*)&size, sizeof(size_t));
        file.write((char*)data.first.c_str(), size);
        size = data.second.length();
        file.write((char*)&size, sizeof(size_t));
        file.write((char*)data.second.c_str(), size);
    }

    const auto& materials = model.getMaterials();
    const auto nbMaterials = materials.size();
    file.write((char*)&nbMaterials, sizeof(size_t));

    // Save materials
    for (const auto& material : materials)
    {
        file.write((char*)&material.first, sizeof(size_t));

        auto name = material.second->getName();
        size_t size = name.length();
        file.write((char*)&size, sizeof(size_t));
        file.write((char*)name.c_str(), size);

        brayns::Vector3f value3f;
        value3f = material.second->getDiffuseColor();
        file.write((char*)&value3f, sizeof(brayns::Vector3f));
        value3f = material.second->getSpecularColor();
        file.write((char*)&value3f, sizeof(brayns::Vector3f));
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
        int32_t simulation = 0;
        try
        {
            simulation = material.second->getProperty<int32_t>(
                MATERIAL_PROPERTY_CAST_USER_DATA);
        }
        catch (const std::runtime_error&)
        {
        }
        file.write((char*)&simulation, sizeof(int32_t));

        int32_t shadingMode = MaterialShadingMode::none;
        try
        {
            shadingMode = material.second->getProperty<int32_t>(
                MATERIAL_PROPERTY_SHADING_MODE);
        }
        catch (const std::runtime_error&)
        {
        }
        file.write((char*)&shadingMode, sizeof(int32_t));

        // TODO: Change bool to int32_t for Version 4
        bool clipped = false;
        try
        {
            clipped = material.second->getProperty<bool>(
                MATERIAL_PROPERTY_CLIPPING_MODE);
        }
        catch (const std::runtime_error&)
        {
        }
        file.write((char*)&clipped, sizeof(bool));
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
        bufferSize = nbElements * sizeof(brayns::Sphere);
        file.write((char*)data.data(), bufferSize);
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
        bufferSize = nbElements * sizeof(brayns::Cylinder);
        file.write((char*)data.data(), bufferSize);
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
        bufferSize = nbElements * sizeof(brayns::Cone);
        file.write((char*)data.data(), bufferSize);
    }

    // Meshes
    nbElements = model.getTriangleMeshes().size();
    file.write((char*)&nbElements, sizeof(size_t));
    for (const auto& meshes : model.getTriangleMeshes())
    {
        const auto materialId = meshes.first;
        file.write((char*)&materialId, sizeof(size_t));

        const auto& data = meshes.second;

        // Vertices
        nbElements = data.vertices.size();
        file.write((char*)&nbElements, sizeof(size_t));
        bufferSize = nbElements * sizeof(brayns::Vector3f);
        file.write((char*)data.vertices.data(), bufferSize);

        // Indices
        nbElements = data.indices.size();
        file.write((char*)&nbElements, sizeof(size_t));
        bufferSize = nbElements * sizeof(brayns::Vector3ui);
        file.write((char*)data.indices.data(), bufferSize);

        // Normals
        nbElements = data.normals.size();
        file.write((char*)&nbElements, sizeof(size_t));
        bufferSize = nbElements * sizeof(brayns::Vector3f);
        file.write((char*)data.normals.data(), bufferSize);

        // Texture coordinates
        nbElements = data.textureCoordinates.size();
        file.write((char*)&nbElements, sizeof(size_t));
        bufferSize = nbElements * sizeof(brayns::Vector2f);
        file.write((char*)data.textureCoordinates.data(), bufferSize);
    }

    // Streamlines
    const auto& streamlines = model.getStreamlines();
    nbElements = streamlines.size();
    file.write((char*)&nbElements, sizeof(size_t));
    for (const auto& streamline : streamlines)
    {
        const auto& streamlineData = streamline.second;
        // Id
        size_t id = streamline.first;
        file.write((char*)&id, sizeof(size_t));

        // Vertex
        nbElements = streamlineData.vertex.size();
        file.write((char*)&nbElements, sizeof(size_t));
        bufferSize = nbElements * sizeof(brayns::Vector4f);
        file.write((char*)streamlineData.vertex.data(), bufferSize);

        // Vertex Color
        nbElements = streamlineData.vertexColor.size();
        file.write((char*)&nbElements, sizeof(size_t));
        bufferSize = nbElements * sizeof(brayns::Vector4f);
        file.write((char*)streamlineData.vertexColor.data(), bufferSize);

        // Indices
        nbElements = streamlineData.indices.size();
        file.write((char*)&nbElements, sizeof(size_t));
        bufferSize = nbElements * sizeof(int32_t);
        file.write((char*)streamlineData.indices.data(), bufferSize);
    }

    // SDF geometry
    const auto& sdfData = model.getSDFGeometryData();
    nbElements = sdfData.geometries.size();
    file.write((char*)&nbElements, sizeof(size_t));

    if (nbElements > 0)
    {
        // Geometries
        bufferSize = nbElements * sizeof(brayns::SDFGeometry);
        file.write((char*)sdfData.geometries.data(), bufferSize);

        // SDF indices
        nbElements = sdfData.geometryIndices.size();
        file.write((char*)&nbElements, sizeof(size_t));
        for (const auto& geometryIndex : sdfData.geometryIndices)
        {
            size_t materialId = geometryIndex.first;
            file.write((char*)&materialId, sizeof(size_t));
            nbElements = geometryIndex.second.size();
            file.write((char*)&nbElements, sizeof(size_t));
            bufferSize = nbElements * sizeof(uint64_t);
            file.write((char*)geometryIndex.second.data(), bufferSize);
        }

        // Neighbours
        nbElements = sdfData.neighbours.size();
        file.write((char*)&nbElements, sizeof(size_t));
        for (const auto& neighbour : sdfData.neighbours)
        {
            nbElements = neighbour.size();
            file.write((char*)&nbElements, sizeof(size_t));
            bufferSize = nbElements * sizeof(size_t);
            file.write((char*)neighbour.data(), bufferSize);
        }

        // Neighbours flat
        nbElements = sdfData.neighboursFlat.size();
        file.write((char*)&nbElements, sizeof(size_t));
        bufferSize = nbElements * sizeof(uint64_t);
        file.write((char*)sdfData.neighboursFlat.data(), bufferSize);
    }

    // Simulation handler
    const brayns::AbstractSimulationHandlerPtr handler =
        model.getSimulationHandler();
    if (handler)
    {
        VoltageSimulationHandler* vsh =
            dynamic_cast<VoltageSimulationHandler*>(handler.get());
        SpikeSimulationHandler* ssh =
            dynamic_cast<SpikeSimulationHandler*>(handler.get());
        if (vsh)
        {
            const size_t reportType{
                static_cast<size_t>(ReportType::voltages_from_file)};
            file.write((char*)&reportType, sizeof(size_t));

            // Report path
            const auto& value = vsh->getReportPath();
            size_t size = value.length();
            file.write((char*)&size, sizeof(size_t));
            file.write((char*)value.c_str(), size);

            // Gids
            const brion::GIDSet& gids = vsh->getReport()->getGIDs();
            size = gids.size();
            file.write((char*)&size, sizeof(size_t));
            for (const auto gid : gids)
                file.write((char*)&gid, sizeof(uint32_t));

            // Synchronization mode
            const bool sync = vsh->isSynchronized();
            file.write((char*)&sync, sizeof(bool));
        }
        else if (ssh)
        {
            const size_t reportType{static_cast<size_t>(ReportType::spikes)};
            file.write((char*)&reportType, sizeof(size_t));

            // Report path
            const auto& value = ssh->getReportPath();
            size_t size = value.length();
            file.write((char*)&size, sizeof(size_t));
            file.write((char*)value.c_str(), size);

            // Gids
            const brion::GIDSet& gids = ssh->getGIDs();
            size = gids.size();
            file.write((char*)&size, sizeof(size_t));
            for (const auto gid : gids)
                file.write((char*)&gid, sizeof(uint32_t));
        }
        else
        {
            // Handler is ignored. Only voltage simulation handler is
            // currently supported
            const size_t reportType{static_cast<size_t>(ReportType::undefined)};
            file.write((char*)&reportType, sizeof(size_t));
        }
    }
    else
    {
        // No handler
        nbElements = 0;
        file.write((char*)&nbElements, sizeof(size_t));
    }

    // Transfer function
    nbElements = 1;
    file.write((char*)&nbElements, sizeof(size_t));
    const auto& tf = _scene.getTransferFunction();
    {
        // Values range
        const brayns::Vector2d& valuesRange = tf.getValuesRange();
        file.write((char*)&valuesRange, sizeof(brayns::Vector2d));

        // Control points
        const brayns::Vector2ds& controlPoints = tf.getControlPoints();
        nbElements = controlPoints.size();
        file.write((char*)&nbElements, sizeof(size_t));
        file.write((char*)&controlPoints[0],
                   nbElements * sizeof(brayns::Vector2d));

        // Color map
        const brayns::ColorMap& colorMap = tf.getColorMap();
        const std::string name = colorMap.name;
        const size_t size = name.length();
        file.write((char*)&size, sizeof(size_t));
        file.write((char*)name.c_str(), size);
        nbElements = colorMap.colors.size();
        file.write((char*)&nbElements, sizeof(size_t));
        file.write((char*)&colorMap.colors[0],
                   nbElements * sizeof(brayns::Vector3f));
    }

    file.close();
}

brayns::PropertyMap BrickLoader::getProperties() const
{
    return _defaults;
}

brayns::PropertyMap BrickLoader::getCLIProperties()
{
    brayns::PropertyMap pm("CircuitExplorer");
    pm.add(PROP_LOAD_SPHERES);
    pm.add(PROP_LOAD_CYLINDERS);
    pm.add(PROP_LOAD_CONES);
    pm.add(PROP_LOAD_MESHES);
    pm.add(PROP_LOAD_STREAMLINES);
    pm.add(PROP_LOAD_SDF);
    pm.add(PROP_LOAD_SIMULATION);
    return pm;
}
