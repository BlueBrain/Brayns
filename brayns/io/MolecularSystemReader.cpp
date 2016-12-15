/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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
#include <brayns/io/ProteinLoader.h>
#include <brayns/io/MeshLoader.h>
#include <fstream>

namespace brayns
{

MolecularSystemReader::MolecularSystemReader(
    const GeometryParameters& geometryParameters )
    : _geometryParameters( geometryParameters )
{
}

bool MolecularSystemReader::import( Scene& scene )
{
    const auto& configuration = _geometryParameters.getMolecularSystemConfig();
    // Load molecular system configuration
    std::ifstream configurationFile( configuration, std::ios::in );
    if( !configurationFile.good( ))
    {
        BRAYNS_ERROR << "Could not open file " << configuration << std::endl;
        return false;
    }

    std::map< std::string, std::string > parameters;
    std::string line;
    while( std::getline( configurationFile, line ))
    {
        std::stringstream lineStream( line );
        std::string key, value;
        lineStream >> key >> value;
        parameters[ key ] = value;
    }
    configurationFile.close();

    const auto& proteinFolder = parameters["ProteinFolder"];
    const auto& meshFolder = parameters["MeshFolder"];
    const auto& descriptor = parameters["SystemDescriptor"];
    const auto& positions = parameters["ProteinPositions"];

    BRAYNS_INFO << "Loading biological assembly" << std::endl;
    BRAYNS_INFO << "Protein folder    : " << proteinFolder << std::endl;
    BRAYNS_INFO << "Mesh folder       : " << meshFolder << std::endl;
    BRAYNS_INFO << "System descriptor : " << descriptor << std::endl;
    BRAYNS_INFO << "Protein positions : " << positions << std::endl;

    std::ifstream descriptorFile( descriptor, std::ios::in );
    if( !descriptorFile.good( ))
    {
        BRAYNS_ERROR << "Could not open file " << descriptor << std::endl;
        return false;
    }

    // Load list of proteins
    std::map< size_t, std::string > proteins;
    while( std::getline( descriptorFile, line ))
    {
        std::stringstream lineStream( line );

        std::string protein;
        size_t id;
        size_t instances;
        lineStream >> protein >> id >> instances;
        if( protein == "" )
            continue;

        proteins[id] = protein;

        if( proteinFolder == "" )
            continue;

        const std::string pdbFilename( proteinFolder + '/' + protein + ".pdb" );
        std::ifstream pdbFile( pdbFilename, std::ios::in );
        if( pdbFile.good( ))
            pdbFile.close();
        else
        {
            // PDB file not present in folder, download it from RCSB.org
            std::string command;
            command = "wget http://www.rcsb.org/pdb/files/";
            command += protein;
            command += ".pdb";
            command += " -P ";
            command += proteinFolder;
            int status = system( command.c_str( ));
            BRAYNS_INFO << command << ": " << status << std::endl;
        }
    }
    descriptorFile.close();

    // Load proteins according to specified positions
    std::ifstream filePositions( positions, std::ios::in );
    if( !filePositions.good( ))
    {
        BRAYNS_ERROR << "Could not open file " << positions << std::endl;
        return false;
    }

    MeshQuality quality;
    switch( _geometryParameters.getGeometryQuality( ))
    {
    case GeometryQuality::medium:
        quality = MQ_QUALITY;
        break;
    case GeometryQuality::high:
        quality = MQ_MAX_QUALITY;
        break;
    default:
        quality = MQ_FAST ;
        break;
    }

    // Load positions
    size_t nbProteins = 0;
    ProteinPositions proteinPositions;
    while( std::getline( filePositions, line ))
    {
        std::stringstream lineStream( line );

        size_t id;
        Vector3f position;
        lineStream >> id >> position.x() >> position.y() >> position.z();

        if( proteins.find(id) != proteins.end( ))
        {
            // Scale to correct units
            position *= 10.f;
            auto& proteinPosition = proteinPositions[ id ];
            proteinPosition.push_back( position );
            ++nbProteins;
        }
    }
    filePositions.close();

    BRAYNS_INFO << "Total number of different proteins: " << proteins.size() << std::endl;
    BRAYNS_INFO << "Total number of proteins          : " << nbProteins << std::endl;

    MeshLoader meshLoader;
    size_t proteinCount = 0;
    for( const auto& proteinPosition: proteinPositions )
    {
        BRAYNS_PROGRESS( proteinCount, nbProteins );

        const auto& protein = proteins.find( proteinPosition.first );
        if( !proteinFolder.empty( ))
            for( const auto& position: proteinPosition.second )
            {
                const auto pdbFilename = proteinFolder + '/' + protein->second + ".pdb";
                ProteinLoader loader( _geometryParameters );
                loader.importPDBFile( pdbFilename, position, proteinCount, scene );
                ++proteinCount;
            }

        if( !meshFolder.empty( ))
        {
            for( const auto& position: proteinPosition.second )
            {
                const auto objFilename = meshFolder + '/' + protein->second + ".obj";
                MeshContainer MeshContainer =
                {
                    scene.getTriangleMeshes(),
                    scene.getMaterials(),
                    scene.getWorldBounds()
                };

                const size_t material =
                    _geometryParameters.getColorScheme() == ColorScheme::protein_by_id ?
                    proteinCount % (NB_MAX_MATERIALS - NB_SYSTEM_MATERIALS) :
                    NO_MATERIAL;

                const float scale = 0.01f; // Scale mesh to match pdb units
                meshLoader.importMeshFromFile(
                    objFilename, MeshContainer, quality,
                    position, Vector3f( scale, scale, scale ), material );
                if( proteinFolder.empty( ))
                    ++proteinCount;
            }
        }
    }

    // Update materials
    if( _geometryParameters.getColorScheme() != ColorScheme::protein_by_id )
    {
        size_t index = 0;
        for( const auto& material: scene.getMaterials( ))
        {
            ProteinLoader loader( _geometryParameters );
            material->setColor( loader.getMaterialKd( index ));
            ++index;
        }
    }

    return true;
}

}
