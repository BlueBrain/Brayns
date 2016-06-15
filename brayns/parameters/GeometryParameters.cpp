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

#include "GeometryParameters.h"
#include <brayns/common/types.h>
#include <brayns/common/log.h>
#include <brayns/common/exceptions.h>

#include <boost/lexical_cast.hpp>

namespace
{

const std::string PARAM_MORPHOLOGY_FOLDER = "morphology-folder";
const std::string PARAM_PDB_FILE = "pdb-file";
const std::string PARAM_MESH_FOLDER = "mesh-folder";
const std::string PARAM_CIRCUIT_CONFIG = "circuit-config";
const std::string PARAM_LOAD_CACHE_FILE = "load-cache-file";
const std::string PARAM_SAVE_CACHE_FILE = "save-cache-file";
const std::string PARAM_RADIUS = "radius";
const std::string PARAM_COLOR_SCHEME = "color-scheme";
const std::string PARAM_SCENE_ENVIRONMENT = "scene-environment";
const std::string PARAM_GEOMETRY_QUALITY = "geometry-quality";
const std::string PARAM_TARGET = "target";
const std::string PARAM_REPORT = "report";
const std::string PARAM_NON_SIMULATED_CELLS = "non-simulated-cells";
const std::string PARAM_FIRST_SIMULATION_FRAME = "first-simulation-frame";
const std::string PARAM_LAST_SIMULATION_FRAME = "last-simulation-frame";
const std::string PARAM_MORPHOLOGY_SECTION_TYPES = "morphology-section-types";
const std::string PARAM_MORPHOLOGY_LAYOUT = "morphology-layout";
const std::string PARAM_GENERATE_MULTIPLE_MODELS = "generate-multiple-models";

}

namespace brayns
{

GeometryParameters::GeometryParameters( )
    : AbstractParameters( "Geometry" )
    , _radius(1)
    , _colorScheme( CS_NONE )
    , _sceneEnvironment( SE_NONE )
    , _geometryQuality( GQ_MAX_QUALITY )
    , _morphologySectionTypes( MST_ALL )
    , _nonSimulatedCells(0)
    , _firstSimulationFrame(0)
    , _lastSimulationFrame(0)
    , _generateMultipleModels(false)
{
    _parameters.add_options()
        ( PARAM_MORPHOLOGY_FOLDER.c_str(), po::value< std::string >( ),
            "Folder containing SWC and H5 files" )
        ( PARAM_MESH_FOLDER.c_str(), po::value< std::string >( ),
            "Folder containing mesh files" )
        ( PARAM_PDB_FILE.c_str(), po::value< std::string >( ),
            "PDB file to load" )
        ( PARAM_CIRCUIT_CONFIG.c_str(), po::value< std::string >( ),
            "Circuit configuration file" )
        ( PARAM_LOAD_CACHE_FILE.c_str(), po::value< std::string >( ),
            "Load binary container of a scene" )
        ( PARAM_SAVE_CACHE_FILE.c_str(), po::value< std::string >( ),
            "Save binary container of a scene" )
        ( PARAM_RADIUS.c_str(), po::value< float >( ),
            "Radius multiplier for spheres, cones and cylinders" )
        ( PARAM_COLOR_SCHEME.c_str( ), po::value< size_t >( ),
            "Color scheme to be applied to the geometry" )
        ( PARAM_SCENE_ENVIRONMENT.c_str(), po::value< size_t >( ),
            "Scene environment (0: none, 1: ground, 2: wall, 3: box)" )
        ( PARAM_GEOMETRY_QUALITY.c_str(), po::value< size_t >( ),
            "Geometry rendering quality (0: Fast rendering, "
                "1: Medium rendering, 2: Max quality)" )
        ( PARAM_TARGET.c_str(), po::value< std::string >( ),
            "Circuit target to load" )
        ( PARAM_REPORT.c_str(), po::value< std::string >( ),
            "Circuit report to load" )
        ( PARAM_MORPHOLOGY_SECTION_TYPES.c_str(), po::value< size_t > ( ),
            "Morphology section types (1: soma, 2: axon, 4: dendrite, "
            "8: apical dendrite). Values can be added to select more than "
            "one type of section" )
        ( PARAM_MORPHOLOGY_LAYOUT.c_str(), po::value< size_ts >()->multitoken(),
            "Morphology layout (number of lines, number of columns, "
            "vertical spacing, horizontal spacing)" )
        ( PARAM_NON_SIMULATED_CELLS.c_str(), po::value< size_t >(),
            "Defines the number of non-simulated cells should be loaded when a "
            "report is specified" )
        ( PARAM_FIRST_SIMULATION_FRAME.c_str(), po::value< size_t >(),
            "First simulation frame to be loaded" )
        ( PARAM_LAST_SIMULATION_FRAME.c_str(), po::value< size_t >(),
            "Last simulation frame to be loaded" )
        ( PARAM_GENERATE_MULTIPLE_MODELS.c_str(), po::value< bool >(),
            "Generated multiple models based on geometry timestamps" );
}

bool GeometryParameters::_parse( const po::variables_map& vm )
{
    if( vm.count( PARAM_MORPHOLOGY_FOLDER ))
        _morphologyFolder = vm[PARAM_MORPHOLOGY_FOLDER].as< std::string >( );
    if( vm.count( PARAM_PDB_FILE ))
        _pdbFile = vm[PARAM_PDB_FILE].as< std::string >( );
    if( vm.count( PARAM_MESH_FOLDER ))
        _meshFolder = vm[PARAM_MESH_FOLDER].as< std::string >( );
    if( vm.count( PARAM_CIRCUIT_CONFIG ))
        _circuitConfig = vm[PARAM_CIRCUIT_CONFIG].as< std::string >( );
    if( vm.count( PARAM_LOAD_CACHE_FILE ))
        _loadCacheFile = vm[PARAM_LOAD_CACHE_FILE].as< std::string >( );
    if( vm.count( PARAM_SAVE_CACHE_FILE ))
        _saveCacheFile = vm[PARAM_SAVE_CACHE_FILE].as< std::string >( );
    if( vm.count( PARAM_COLOR_SCHEME ))
        _colorScheme = static_cast< ColorScheme >(
            vm[PARAM_COLOR_SCHEME].as< size_t >( ));
    if( vm.count( PARAM_RADIUS))
        _radius = vm[PARAM_RADIUS].as< float >( );
    if( vm.count( PARAM_SCENE_ENVIRONMENT ))
        _sceneEnvironment = static_cast< SceneEnvironment >(
            vm[PARAM_SCENE_ENVIRONMENT].as< size_t >( ));
    if( vm.count( PARAM_GEOMETRY_QUALITY ))
        _geometryQuality = static_cast< GeometryQuality >(
            vm[PARAM_GEOMETRY_QUALITY].as< size_t >( ));
    if( vm.count( PARAM_TARGET ))
        _target = vm[PARAM_TARGET].as< std::string >( );
    if( vm.count( PARAM_REPORT ))
        _report = vm[PARAM_REPORT].as< std::string >( );
    if( vm.count( PARAM_MORPHOLOGY_SECTION_TYPES ))
        _morphologySectionTypes =
            vm[PARAM_MORPHOLOGY_SECTION_TYPES].as< size_t >( );
    if( vm.count( PARAM_MORPHOLOGY_LAYOUT ))
    {
        size_ts values = vm[PARAM_MORPHOLOGY_LAYOUT].as< size_ts >( );
        if( values.size( ) == 3 )
        {
            _morphologyLayout.type = ML_GRID;
            _morphologyLayout.nbColumns = values[0];
            _morphologyLayout.verticalSpacing = values[1];
            _morphologyLayout.horizontalSpacing = values[2];
        }
    }
    if( vm.count( PARAM_NON_SIMULATED_CELLS))
        _nonSimulatedCells =
            vm[PARAM_NON_SIMULATED_CELLS].as< size_t >( );
    if( vm.count( PARAM_FIRST_SIMULATION_FRAME ))
        _firstSimulationFrame =
            vm[PARAM_FIRST_SIMULATION_FRAME].as< size_t >( );
    if( vm.count( PARAM_LAST_SIMULATION_FRAME ))
        _lastSimulationFrame =
            vm[PARAM_LAST_SIMULATION_FRAME].as< size_t >( );
    if( vm.count( PARAM_GENERATE_MULTIPLE_MODELS ))
        _generateMultipleModels =
            vm[PARAM_GENERATE_MULTIPLE_MODELS].as< bool >( );

    return true;
}

void GeometryParameters::print( )
{
    AbstractParameters::print( );
    BRAYNS_INFO << "Morphology folder          : " <<
        _morphologyFolder << std::endl;
    BRAYNS_INFO << "PDB file                   : " << _pdbFile << std::endl;
    BRAYNS_INFO << "Mesh folder                : " << _meshFolder << std::endl;
    BRAYNS_INFO << "Cache file to load         : " << _loadCacheFile << std::endl;
    BRAYNS_INFO << "Cache file to save         : " << _saveCacheFile << std::endl;
    BRAYNS_INFO << "Circuit configuration      : " << _circuitConfig << std::endl;
    BRAYNS_INFO << "Color scheme               : " <<
        static_cast<size_t>( _colorScheme ) << std::endl;
    BRAYNS_INFO << "Radius                     : " << _radius << std::endl;
    BRAYNS_INFO << "Scene environment          : " <<
        static_cast<size_t>( _sceneEnvironment ) << std::endl;
    BRAYNS_INFO << "Geometry quality           : " <<
        static_cast<size_t>( _geometryQuality ) << std::endl;
    BRAYNS_INFO << "Target                     : " <<
        _target << std::endl;
    BRAYNS_INFO << "Report                     : " <<
        _report << std::endl;
    BRAYNS_INFO << "- Non-simulated cells      : " <<
        _nonSimulatedCells << std::endl;
    BRAYNS_INFO << "- First frame              : " <<
        _firstSimulationFrame << std::endl;
    BRAYNS_INFO << "- Last frame               : " <<
        _lastSimulationFrame << std::endl;
    BRAYNS_INFO << "Morphology section types   : " <<
        _morphologySectionTypes << std::endl;
    BRAYNS_INFO << "Morphology Layout          : " << std::endl;
    BRAYNS_INFO << " - Columns                 : " <<
        _morphologyLayout.nbColumns << std::endl;
    BRAYNS_INFO << " - Vertical spacing        : " <<
        _morphologyLayout.verticalSpacing << std::endl;
    BRAYNS_INFO << " - Horizontal spacing      : " <<
        _morphologyLayout.horizontalSpacing << std::endl;
    BRAYNS_INFO << "Generate multiple models   : " <<
        (_generateMultipleModels ? "on" : "off") << std::endl;
}

}
