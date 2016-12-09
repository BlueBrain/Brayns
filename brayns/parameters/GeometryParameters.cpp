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
const std::string PARAM_NEST_CIRCUIT = "nest-circuit";
const std::string PARAM_NEST_REPORT = "nest-report";
const std::string PARAM_PDB_FILE = "pdb-file";
const std::string PARAM_PDB_FOLDER = "pdb-folder";
const std::string PARAM_XYZB_FILE = "xyzb-file";
const std::string PARAM_MESH_FOLDER = "mesh-folder";
const std::string PARAM_CIRCUIT_CONFIG = "circuit-config";
const std::string PARAM_LOAD_CACHE_FILE = "load-cache-file";
const std::string PARAM_SAVE_CACHE_FILE = "save-cache-file";
const std::string PARAM_RADIUS_MULTIPLIER = "radius-multiplier";
const std::string PARAM_RADIUS_CORRECTION = "radius-correction";
const std::string PARAM_COLOR_SCHEME = "color-scheme";
const std::string PARAM_SCENE_ENVIRONMENT = "scene-environment";
const std::string PARAM_GEOMETRY_QUALITY = "geometry-quality";
const std::string PARAM_TARGET = "target";
const std::string PARAM_REPORT = "report";
const std::string PARAM_NON_SIMULATED_CELLS = "non-simulated-cells";
const std::string PARAM_START_SIMULATION_TIME = "start-simulation-time";
const std::string PARAM_END_SIMULATION_TIME = "end-simulation-time";
const std::string PARAM_SIMULATION_RANGE = "simulation-values-range";
const std::string PARAM_SIMULATION_CACHE_FILENAME = "simulation-cache-file";
const std::string PARAM_SIMULATION_HISTOGRAM_SIZE = "simulation-histogram-size";
const std::string PARAM_NEST_CACHE_FILENAME = "nest-cache-file";
const std::string PARAM_MORPHOLOGY_SECTION_TYPES = "morphology-section-types";
const std::string PARAM_MORPHOLOGY_LAYOUT = "morphology-layout";
const std::string PARAM_GENERATE_MULTIPLE_MODELS = "generate-multiple-models";
const std::string PARAM_SPLASH_SCENE_FOLDER = "splash-scene-folder";

}

namespace brayns
{

GeometryParameters::GeometryParameters( )
    : AbstractParameters( "Geometry" )
    , _radiusMultiplier( 1.f )
    , _radiusCorrection( 0.f )
    , _colorScheme( ColorScheme::none )
    , _sceneEnvironment( SE_NONE )
    , _geometryQuality( GeometryQuality::high )
    , _morphologySectionTypes( MST_ALL )
    , _nonSimulatedCells( 0 )
    , _startSimulationTime( 0.f )
    , _endSimulationTime( std::numeric_limits<float>::max() )
    , _simulationValuesRange( Vector2f(
        std::numeric_limits<float>::max(), std::numeric_limits<float>::min() ))
    , _simulationHistogramSize( 128 )
    , _generateMultipleModels( false )
{
    _parameters.add_options()
        ( PARAM_MORPHOLOGY_FOLDER.c_str(), po::value< std::string >( ),
            "Folder containing SWC and H5 files" )
        ( PARAM_NEST_CIRCUIT.c_str(), po::value< std::string >( ),
            "H5 file containing the NEST circuit" )
        ( PARAM_NEST_REPORT.c_str(), po::value< std::string >( ),
            "NEST simulation report file" )
        ( PARAM_MESH_FOLDER.c_str(), po::value< std::string >( ),
            "Folder containing mesh files" )
        ( PARAM_PDB_FILE.c_str(), po::value< std::string >( ),
            "PDB file to load" )
        ( PARAM_PDB_FOLDER.c_str(), po::value< std::string >( ),
            "Folder containing PDB files" )
        ( PARAM_XYZB_FILE.c_str(), po::value< std::string >( ),
            "XYZB file to load" )
        ( PARAM_CIRCUIT_CONFIG.c_str(), po::value< std::string >( ),
            "Circuit configuration file" )
        ( PARAM_LOAD_CACHE_FILE.c_str(), po::value< std::string >( ),
            "Load binary container of a scene" )
        ( PARAM_SAVE_CACHE_FILE.c_str(), po::value< std::string >( ),
            "Save binary container of a scene" )
        ( PARAM_RADIUS_MULTIPLIER.c_str(), po::value< float >( ),
            "Radius multiplier for spheres, cones and cylinders" )
        ( PARAM_RADIUS_CORRECTION.c_str(), po::value< float >( ),
            "Forces radius of spheres and cylinders to the specified value" )
        ( PARAM_COLOR_SCHEME.c_str( ), po::value< size_t >( ),
            "Color scheme to be applied to the geometry" )
        ( PARAM_SCENE_ENVIRONMENT.c_str(), po::value< size_t >( ),
            "Scene environment (0: none, 1: ground, 2: wall, 3: bounding box)" )
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
        ( PARAM_START_SIMULATION_TIME.c_str(), po::value< float >(),
            "Start simulation time" )
        ( PARAM_END_SIMULATION_TIME.c_str(), po::value< float >(),
            "End simulation time" )
        ( PARAM_SIMULATION_RANGE.c_str(), po::value< floats >()->multitoken(),
            "Minimum and maximum values for the simulation" )
        ( PARAM_SIMULATION_CACHE_FILENAME.c_str(), po::value< std::string >(),
            "Cache file containing simulation data" )
        (PARAM_SIMULATION_HISTOGRAM_SIZE.c_str(), po::value< size_t >(),
            "Number of values defining the simulation histogram")
        ( PARAM_NEST_CACHE_FILENAME.c_str(), po::value< std::string >(),
            "Cache file containing nest data" )
        ( PARAM_GENERATE_MULTIPLE_MODELS.c_str(), po::value< bool >(),
            "Generated multiple models based on geometry timestamps" )
        ( PARAM_SPLASH_SCENE_FOLDER.c_str(), po::value< std::string >( ),
            "Folder containing splash scene folder" );
}

bool GeometryParameters::_parse( const po::variables_map& vm )
{
    if( vm.count( PARAM_MORPHOLOGY_FOLDER ))
        _morphologyFolder = vm[PARAM_MORPHOLOGY_FOLDER].as< std::string >( );
    if( vm.count( PARAM_NEST_CIRCUIT ))
        _NESTCircuit = vm[PARAM_NEST_CIRCUIT].as< std::string >( );
    if( vm.count( PARAM_NEST_REPORT ))
        _NESTReport = vm[PARAM_NEST_REPORT].as< std::string >( );
    if( vm.count( PARAM_PDB_FILE ))
        _pdbFile = vm[PARAM_PDB_FILE].as< std::string >( );
    if( vm.count( PARAM_PDB_FOLDER ))
        _pdbFolder = vm[PARAM_PDB_FOLDER].as< std::string >( );
    if( vm.count( PARAM_XYZB_FILE ))
        _xyzbFile = vm[PARAM_XYZB_FILE].as< std::string >( );
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
    if( vm.count( PARAM_RADIUS_MULTIPLIER))
        _radiusMultiplier = vm[PARAM_RADIUS_MULTIPLIER].as< float >( );
    if( vm.count( PARAM_RADIUS_CORRECTION))
        _radiusCorrection = vm[PARAM_RADIUS_CORRECTION].as< float >( );
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
    if( vm.count( PARAM_START_SIMULATION_TIME ))
        _startSimulationTime =
            vm[PARAM_START_SIMULATION_TIME].as< float >( );
    if( vm.count( PARAM_END_SIMULATION_TIME ))
        _endSimulationTime =
            vm[PARAM_END_SIMULATION_TIME].as< float >( );
    if( vm.count( PARAM_SIMULATION_RANGE ))
    {
        floats values = vm[PARAM_SIMULATION_RANGE].as< floats >( );
        if( values.size( ) == 2 )
            _simulationValuesRange = Vector2f( values[0], values[1] );
    }
    if( vm.count( PARAM_SIMULATION_CACHE_FILENAME ))
        _simulationCacheFile =
            vm[PARAM_SIMULATION_CACHE_FILENAME].as< std::string >( );
    if( vm.count( PARAM_SIMULATION_HISTOGRAM_SIZE ))
        _simulationHistogramSize = vm[PARAM_SIMULATION_HISTOGRAM_SIZE].as< size_t >( );
    if( vm.count( PARAM_NEST_CACHE_FILENAME ))
        _NESTCacheFile =
            vm[PARAM_NEST_CACHE_FILENAME].as< std::string >( );
    if( vm.count( PARAM_GENERATE_MULTIPLE_MODELS ))
        _generateMultipleModels =
            vm[PARAM_GENERATE_MULTIPLE_MODELS].as< bool >( );
    if( vm.count( PARAM_SPLASH_SCENE_FOLDER ))
        _splashSceneFolder = vm[PARAM_SPLASH_SCENE_FOLDER].as< std::string >( );

    return true;
}

void GeometryParameters::print( )
{
    AbstractParameters::print( );
    BRAYNS_INFO << "Morphology folder          : " <<
        _morphologyFolder << std::endl;
    BRAYNS_INFO << "NEST circuit file          : " <<
        _NESTCircuit << std::endl;
    BRAYNS_INFO << "NEST simulation report file: " <<
        _NESTReport << std::endl;
    BRAYNS_INFO << "NEST cache file            : " <<
        _NESTCacheFile << std::endl;
    BRAYNS_INFO << "PDB file                   : " <<
        _pdbFile << std::endl;
    BRAYNS_INFO << "PDB folder                 : " <<
        _pdbFolder << std::endl;
    BRAYNS_INFO << "XYZB file                  : " <<
        _xyzbFile << std::endl;
    BRAYNS_INFO << "Mesh folder                : " <<
        _meshFolder << std::endl;
    BRAYNS_INFO << "Cache file to load         : " <<
        _loadCacheFile << std::endl;
    BRAYNS_INFO << "Cache file to save         : " <<
        _saveCacheFile << std::endl;
    BRAYNS_INFO << "Circuit configuration      : " <<
        _circuitConfig << std::endl;
    BRAYNS_INFO << "Color scheme               : " <<
        static_cast<size_t>( _colorScheme ) << std::endl;
    BRAYNS_INFO << "Radius multiplier          : " <<
        _radiusMultiplier << std::endl;
    BRAYNS_INFO << "Radius correction          : " <<
        _radiusCorrection << std::endl;
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
    BRAYNS_INFO << "- Start simulation time    : " <<
        _startSimulationTime << std::endl;
    BRAYNS_INFO << "- End simulation time      : " <<
        _endSimulationTime << std::endl;
    BRAYNS_INFO << "- Simulation values range  : " <<
        _simulationValuesRange << std::endl;
    BRAYNS_INFO << "- Simulation cache file    : " <<
        _simulationCacheFile << std::endl;
    BRAYNS_INFO << "- Simulation histogram size: " <<
        _simulationHistogramSize << std::endl;
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
    BRAYNS_INFO << "Splash scene folder        : " <<
        _splashSceneFolder << std::endl;
}

}
