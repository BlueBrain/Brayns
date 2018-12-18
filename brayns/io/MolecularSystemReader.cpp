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

#include "MolecularSystemReader.h"

#include <brayns/common/log.h>
#include <brayns/common/utils/enumUtils.h>
#include <brayns/common/utils/utils.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>
#if BRAYNS_USE_ASSIMP
#include <brayns/io/MeshLoader.h>
#endif
#include <brayns/io/simulation/CADiffusionSimulationHandler.h>

#include <fstream>

namespace
{
const auto PROP_COLOR_SCHEME = "colorScheme";
const auto LOADER_NAME = "molecular-system";
}

namespace brayns
{
MolecularSystemReader::MolecularSystemReader(Scene& scene,
                                             const GeometryParameters& params)
    : Loader(scene)
    , _proteinLoader(scene, params)
{
    _defaults.setProperty({PROP_COLOR_SCHEME,
                           enumToString(params.getColorScheme()),
                           brayns::enumNames<brayns::ColorScheme>(),
                           {"Color scheme"}});
#if BRAYNS_USE_ASSIMP
    { // Add all mesh loader properties
        const auto mlpm = MeshLoader(_scene, params).getProperties();
        for (const auto& prop : mlpm.getProperties())
            if (prop && !_defaults.hasProperty(prop->name))
                _defaults.setProperty(*prop);
    }
#endif
}

bool MolecularSystemReader::isSupported(const std::string& filename
                                            BRAYNS_UNUSED,
                                        const std::string& extension) const
{
    return extension == "molsys";
}

ModelDescriptorPtr MolecularSystemReader::importFromFile(
    const std::string& fileName, const LoaderProgress& callback,
    const PropertyMap& inProperties, const size_t index BRAYNS_UNUSED,
    const size_t defaultMaterialId BRAYNS_UNUSED) const
{
    PropertyMap properties = _defaults;
    properties.merge(inProperties);

    LoaderData data;
    data._callback = callback;
    data._nbProteins = 0;
    if (!_loadConfiguration(fileName, data))
        throw std::runtime_error("Failed to load " + fileName);
    if (!_loadProteins(data))
        throw std::runtime_error("Failed to load proteins");
    if (!_loadPositions(data))
        throw std::runtime_error("Failed to load positions");

    if (!_createScene(data, properties))
        throw std::runtime_error("Failed to load scene");

    if (!data._calciumSimulationFolder.empty())
    {
        CADiffusionSimulationHandlerPtr handler(
            new CADiffusionSimulationHandler(data._calciumSimulationFolder));
        handler->setFrame(_scene, 0);
        _scene.setCADiffusionSimulationHandler(handler);
    }
    BRAYNS_INFO << "Total number of different proteins: "
                << data._proteins.size() << std::endl;
    BRAYNS_INFO << "Total number of proteins          : " << data._nbProteins
                << std::endl;
    return {};
}

bool MolecularSystemReader::_createScene(
    MolecularSystemReader::LoaderData& data,
    const PropertyMap& properties) const
{
#if BRAYNS_USE_ASSIMP
    const auto colorScheme = stringToEnum<ColorScheme>(
        properties.getProperty<std::string>(PROP_COLOR_SCHEME));
    MeshLoader meshLoader(_scene);
#endif
    uint64_t proteinCount = 0;
    for (const auto& proteinPosition : data._proteinPositions)
    {
        const auto& protein = data._proteins.find(proteinPosition.first);
        if (!data._proteinFolder.empty())
            // Load PDB files
            for (const auto& position : proteinPosition.second)
            {
                const auto pdbFilename =
                    data._proteinFolder + '/' + protein->second + ".pdb";
                Matrix4f transformation;
                transformation.setTranslation(position);
                _proteinLoader.importFromFile(pdbFilename, data._callback,
                                              properties, proteinCount,
                                              NO_MATERIAL);
                ++proteinCount;
            }

        if (!data._meshFolder.empty())
        {
#if BRAYNS_USE_ASSIMP
            // Load meshes
            for (const auto& position : proteinPosition.second)
            {
                const Vector3f scale = {1.f, 1.f, 1.f};
                const Matrix4f transformation(position, scale);
                const size_t materialId = colorScheme == ColorScheme::by_id
                                              ? proteinCount
                                              : NO_MATERIAL;

                // Scale mesh to match PDB units. PDB are in angstrom, and
                // positions are in micrometers
                const std::string fileName =
                    data._meshFolder + '/' + protein->second + ".obj";
                meshLoader.importFromFile(fileName, data._callback, properties,
                                          proteinCount, materialId);

                if (data._proteinFolder.empty())
                    ++proteinCount;
            }
#else
            throw std::runtime_error(
                "assimp dependency is required to load meshes");
#endif
        }

        data._callback.updateProgress("Loading proteins...",
                                      proteinCount /
                                          static_cast<float>(data._nbProteins));
    }
    return true;
}

bool MolecularSystemReader::_loadConfiguration(
    const std::string& fileName, MolecularSystemReader::LoaderData& data) const
{
    // Load molecular system configuration
    std::ifstream configurationFile(fileName, std::ios::in);
    if (!configurationFile.good())
    {
        BRAYNS_ERROR << "Could not open file " << fileName << std::endl;
        return false;
    }

    std::map<std::string, std::string> parameters;
    std::string line;
    while (std::getline(configurationFile, line))
    {
        std::stringstream lineStream(line);
        std::string key, value;
        lineStream >> key >> value;
        parameters[key] = value;
    }
    configurationFile.close();

    data._proteinFolder = parameters["ProteinFolder"];
    data._meshFolder = parameters["MeshFolder"];
    data._descriptorFilename = parameters["SystemDescriptor"];
    data._positionsFilename = parameters["ProteinPositions"];
    data._calciumSimulationFolder = parameters["CalciumPositions"];

    BRAYNS_INFO << "Loading molecular system" << std::endl;
    BRAYNS_INFO << "Protein folder    : " << data._proteinFolder << std::endl;
    BRAYNS_INFO << "Mesh folder       : " << data._meshFolder << std::endl;
    BRAYNS_INFO << "System descriptor : " << data._descriptorFilename
                << std::endl;
    BRAYNS_INFO << "Protein positions : " << data._positionsFilename
                << std::endl;
    BRAYNS_INFO << "Calcium positions : " << data._calciumSimulationFolder
                << std::endl;
    return true;
}

bool MolecularSystemReader::_loadProteins(
    MolecularSystemReader::LoaderData& data) const
{
    std::ifstream descriptorFile(data._descriptorFilename, std::ios::in);
    if (!descriptorFile.good())
    {
        BRAYNS_ERROR << "Could not open file " << data._descriptorFilename
                     << std::endl;
        return false;
    }

    // Load list of proteins
    std::string line;
    while (descriptorFile.good() && std::getline(descriptorFile, line))
    {
        std::stringstream lineStream(line);
        std::string protein;
        size_t id;
        size_t instances;
        lineStream >> protein >> id >> instances;
        if (protein.empty())
            continue;

        data._proteins[id] = protein;

        if (data._proteinFolder.empty())
            continue;

        const auto pdbFilename(data._proteinFolder + '/' + protein + ".pdb");
        std::ifstream pdbFile(pdbFilename, std::ios::in);
        if (pdbFile.good())
            pdbFile.close();
        else
        {
            // PDB file not present in folder, download it from RCSB.org
            std::string command;
            command = "wget http://www.rcsb.org/pdb/files/";
            command += protein;
            command += ".pdb";
            command += " -P ";
            command += data._proteinFolder;
            int status = system(command.c_str());
            BRAYNS_INFO << command << ": " << status << std::endl;
        }
    }
    descriptorFile.close();
    return true;
}

bool MolecularSystemReader::_loadPositions(
    MolecularSystemReader::LoaderData& data) const
{
    // Load proteins according to specified positions
    std::ifstream filePositions(data._positionsFilename, std::ios::in);
    if (!filePositions.good())
    {
        BRAYNS_ERROR << "Could not open file " << data._positionsFilename
                     << std::endl;
        return false;
    }

    // Load protein positions
    data._nbProteins = 0;
    std::string line;
    while (filePositions.good() && std::getline(filePositions, line))
    {
        std::stringstream lineStream(line);
        size_t id;
        Vector3f position;
        lineStream >> id >> position.x() >> position.y() >> position.z();

        if (data._proteins.find(id) != data._proteins.end())
        {
            auto& proteinPosition = data._proteinPositions[id];
            proteinPosition.push_back(position);
            ++data._nbProteins;
        }
    }
    filePositions.close();
    return true;
}

void MolecularSystemReader::_writePositionstoFile(
    const std::string& filename,
    const MolecularSystemReader::LoaderData& data) const
{
    std::ofstream outfile(filename, std::ios::binary);
    for (const auto& position : data._proteinPositions)
    {
        for (const auto& element : position.second)
        {
            const float radius = 1.f;
            const float value = 1.f;
            outfile.write((char*)&element.x(), sizeof(float));
            outfile.write((char*)&element.y(), sizeof(float));
            outfile.write((char*)&element.z(), sizeof(float));
            outfile.write((char*)&radius, sizeof(float));
            outfile.write((char*)&value, sizeof(float));
        }
    }
    outfile.close();
}

std::string MolecularSystemReader::getName() const
{
    return LOADER_NAME;
}

std::vector<std::string> MolecularSystemReader::getSupportedExtensions() const
{
    return {"molsys"};
}

PropertyMap MolecularSystemReader::getProperties() const
{
    return _defaults;
}
}
