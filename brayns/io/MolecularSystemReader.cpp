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
#include <brayns/common/scene/Scene.h>
#include <brayns/common/simulation/CADiffusionSimulationHandler.h>
#include <brayns/io/ProteinLoader.h>
#include <fstream>

namespace brayns
{
MolecularSystemReader::MolecularSystemReader(
    const GeometryParameters& geometryParameters)
    : _geometryParameters(geometryParameters)
{
}

bool MolecularSystemReader::import(Scene& scene, MeshLoader& meshLoader)
{
    _nbProteins = 0;
    if (!_loadConfiguration())
        return false;
    if (!_loadProteins())
        return false;
    if (!_loadPositions())
        return false;

    if (!_createScene(scene, meshLoader))
        return false;

    if (!_calciumSimulationFolder.empty())
    {
        CADiffusionSimulationHandlerPtr handler(
            new CADiffusionSimulationHandler(_calciumSimulationFolder));
        handler->setFrame(scene, 0);
        scene.setCADiffusionSimulationHandler(handler);
    }
    BRAYNS_INFO << "Total number of different proteins: " << _proteins.size()
                << std::endl;
    BRAYNS_INFO << "Total number of proteins          : " << _nbProteins
                << std::endl;

    return true;
}

bool MolecularSystemReader::_createScene(Scene& scene, MeshLoader& meshLoader)
{
    const auto quality = _geometryParameters.getGeometryQuality();

    uint64_t proteinCount = 0;
    for (const auto& proteinPosition : _proteinPositions)
    {
        BRAYNS_PROGRESS(proteinCount, _nbProteins);

        const auto& protein = _proteins.find(proteinPosition.first);
        if (!_proteinFolder.empty())
            // Load PDB files
            for (const auto& position : proteinPosition.second)
            {
                const auto pdbFilename =
                    _proteinFolder + '/' + protein->second + ".pdb";
                ProteinLoader loader(_geometryParameters);
                loader.importPDBFile(pdbFilename, position, proteinCount,
                                     scene);
                ++proteinCount;
            }

        if (!_meshFolder.empty())
            // Load meshes
            for (const auto& position : proteinPosition.second)
            {
                const Vector3f scale = {1.f, 1.f, 1.f};
                const Matrix4f transformation(position, scale);
                const auto objFilename =
                    _meshFolder + '/' + protein->second + ".obj";
                const size_t material =
                    _geometryParameters.getColorScheme() ==
                            ColorScheme::protein_by_id
                        ? proteinCount %
                              (NB_MAX_MATERIALS - NB_SYSTEM_MATERIALS)
                        : NO_MATERIAL;

                // Scale mesh to match PDB units. PDB are in angstrom, and
                // positions are
                // in micrometers
                meshLoader.importMeshFromFile(objFilename, scene, quality,
                                              transformation, material);

                if (_proteinFolder.empty())
                    ++proteinCount;
            }
    }

    // Update materials
    if (_geometryParameters.getColorScheme() != ColorScheme::protein_by_id)
    {
        size_t index = 0;
        for (const auto& material : scene.getMaterials())
        {
            ProteinLoader loader(_geometryParameters);
            material.second->setColor(loader.getMaterialKd(index));
            ++index;
        }
    }
    return true;
}

bool MolecularSystemReader::_loadConfiguration()
{
    // Load molecular system configuration
    const auto& configuration = _geometryParameters.getMolecularSystemConfig();
    std::ifstream configurationFile(configuration, std::ios::in);
    if (!configurationFile.good())
    {
        BRAYNS_ERROR << "Could not open file " << configuration << std::endl;
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

    _proteinFolder = parameters["ProteinFolder"];
    _meshFolder = parameters["MeshFolder"];
    _descriptorFilename = parameters["SystemDescriptor"];
    _positionsFilename = parameters["ProteinPositions"];
    _calciumSimulationFolder = parameters["CalciumPositions"];

    BRAYNS_INFO << "Loading molecular system" << std::endl;
    BRAYNS_INFO << "Protein folder    : " << _proteinFolder << std::endl;
    BRAYNS_INFO << "Mesh folder       : " << _meshFolder << std::endl;
    BRAYNS_INFO << "System descriptor : " << _descriptorFilename << std::endl;
    BRAYNS_INFO << "Protein positions : " << _positionsFilename << std::endl;
    BRAYNS_INFO << "Calcium positions : " << _calciumSimulationFolder
                << std::endl;
    return true;
}

bool MolecularSystemReader::_loadProteins()
{
    std::ifstream descriptorFile(_descriptorFilename, std::ios::in);
    if (!descriptorFile.good())
    {
        BRAYNS_ERROR << "Could not open file " << _descriptorFilename
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

        _proteins[id] = protein;

        if (_proteinFolder.empty())
            continue;

        const auto pdbFilename(_proteinFolder + '/' + protein + ".pdb");
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
            command += _proteinFolder;
            int status = system(command.c_str());
            BRAYNS_INFO << command << ": " << status << std::endl;
        }
    }
    descriptorFile.close();
    return true;
}

bool MolecularSystemReader::_loadPositions()
{
    // Load proteins according to specified positions
    std::ifstream filePositions(_positionsFilename, std::ios::in);
    if (!filePositions.good())
    {
        BRAYNS_ERROR << "Could not open file " << _positionsFilename
                     << std::endl;
        return false;
    }

    // Load protein positions
    _nbProteins = 0;
    std::string line;
    while (filePositions.good() && std::getline(filePositions, line))
    {
        std::stringstream lineStream(line);
        size_t id;
        Vector3f position;
        lineStream >> id >> position.x() >> position.y() >> position.z();

        if (_proteins.find(id) != _proteins.end())
        {
            auto& proteinPosition = _proteinPositions[id];
            proteinPosition.push_back(position);
            ++_nbProteins;
        }
    }
    filePositions.close();
    return true;
}

void MolecularSystemReader::_writePositionstoFile(const std::string& filename)
{
    std::ofstream outfile(filename, std::ios::binary);
    for (const auto& position : _proteinPositions)
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
}
