/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "GeometryParameters.h"
#include <brayns/common/types.h>
#include <brayns/common/log.h>
#include <brayns/common/exceptions.h>

#include <boost/lexical_cast.hpp>

namespace
{

const std::string PARAM_MORPHOLOGY_FOLDER = "morphology-folder";
const std::string PARAM_PDB_FOLDER = "pdb-folder";
const std::string PARAM_MESH_FOLDER = "mesh-folder";
const std::string PARAM_CIRCUIT_CONFIG = "circuit-config";
const std::string PARAM_LOAD_CACHE_FILE = "load-cache-file";
const std::string PARAM_SAVE_CACHE_FILE = "save-cache-file";
const std::string PARAM_RADIUS = "radius";
const std::string PARAM_COLOR_SCHEME = "color-scheme";
const std::string PARAM_SCENE_ENVIRONMENT = "scene-environment";
const std::string PARAM_GEOMETRY_QUALITY = "geometry-quality";
const std::string PARAM_TARGET = "target";
const std::string PARAM_MORPHOLOGY_SECTION_TYPES = "morphology-section-types";
const std::string PARAM_MORPHOLOGY_LAYOUT = "morphology-layout";

}

namespace brayns
{

namespace po = boost::program_options;

GeometryParameters::GeometryParameters( )
    : AbstractParameters( "Geometry" )
    , _radius(1)
    , _colorScheme( CS_NONE )
    , _sceneEnvironment( SE_NONE )
    , _geometryQuality( GQ_MAX_QUALITY )
    , _morphologySectionTypes( MST_ALL )
{
    _parameters.add_options()
        ( PARAM_MORPHOLOGY_FOLDER.c_str(), po::value< std::string >( ),
            "Folder containing SWC and H5 files" )
        ( PARAM_MESH_FOLDER.c_str(), po::value< std::string >( ),
            "Folder containing mesh files" )
        ( PARAM_PDB_FOLDER.c_str(), po::value< strings >( )->multitoken(),
            "Folder containing PDB files" )
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
        ( PARAM_MORPHOLOGY_SECTION_TYPES.c_str(), po::value< size_t > ( ),
            "Morphology section types (1: soma, 2: axon, 4: dendrite, "
            "8: apical dendrite). Values can be added to select more than "
            "one type of section" )
        ( PARAM_MORPHOLOGY_LAYOUT.c_str(), po::value< size_ts >()->multitoken(),
            "Morphology layout (number of lines, number of columns, "
            "vertical spacing, horizontal spacing)" );
}

bool GeometryParameters::parse( int argc, const char **argv )
{
    AbstractParameters::parse( argc, argv );

    if( _vm.count( PARAM_MORPHOLOGY_FOLDER ))
        _morphologyFolder = _vm[PARAM_MORPHOLOGY_FOLDER].as< std::string >( );
    if( _vm.count( PARAM_PDB_FOLDER ))
    {
        strings values = _vm[PARAM_PDB_FOLDER].as< strings >( );
        if( values.size( ) == 3 )
        {
            _pdbFolder = values[0];
            _pdbCells = values[1];
            _pdbPositions = values[2];
        }
    }
    if( _vm.count( PARAM_MESH_FOLDER ))
        _meshFolder = _vm[PARAM_MESH_FOLDER].as< std::string >( );
    if( _vm.count( PARAM_CIRCUIT_CONFIG ))
        _circuitConfig = _vm[PARAM_CIRCUIT_CONFIG].as< std::string >( );
    if( _vm.count( PARAM_LOAD_CACHE_FILE ))
        _loadCacheFile = _vm[PARAM_LOAD_CACHE_FILE].as< std::string >( );
    if( _vm.count( PARAM_SAVE_CACHE_FILE ))
        _saveCacheFile = _vm[PARAM_SAVE_CACHE_FILE].as< std::string >( );
    if( _vm.count( PARAM_COLOR_SCHEME ))
        _colorScheme = static_cast< ColorScheme >(
            _vm[PARAM_COLOR_SCHEME].as< size_t >( ));
    if( _vm.count( PARAM_RADIUS))
        _radius = _vm[PARAM_RADIUS].as< float >( );
    if( _vm.count( PARAM_SCENE_ENVIRONMENT ))
        _sceneEnvironment = static_cast< SceneEnvironment >(
            _vm[PARAM_SCENE_ENVIRONMENT].as< size_t >( ));
    if( _vm.count( PARAM_GEOMETRY_QUALITY ))
        _geometryQuality = static_cast< GeometryQuality >(
            _vm[PARAM_GEOMETRY_QUALITY].as< size_t >( ));
    if( _vm.count( PARAM_TARGET ))
        _target = _vm[PARAM_TARGET].as< std::string >( );
    if( _vm.count( PARAM_MORPHOLOGY_SECTION_TYPES ))
        _morphologySectionTypes =
            _vm[PARAM_MORPHOLOGY_SECTION_TYPES].as< size_t >( );
    if( _vm.count( PARAM_MORPHOLOGY_LAYOUT ))
    {
        size_ts values = _vm[PARAM_MORPHOLOGY_LAYOUT].as< size_ts >( );
        if( values.size( ) == 3 )
        {
            _morphologyLayout.type = ML_GRID;
            _morphologyLayout.nbColumns = values[0];
            _morphologyLayout.verticalSpacing = values[1];
            _morphologyLayout.horizontalSpacing = values[2];
        }
    }

    return true;
}

void GeometryParameters::print( )
{
    AbstractParameters::print( );
    BRAYNS_INFO << "Morphology folder       : " <<
        _morphologyFolder << std::endl;
    BRAYNS_INFO << "PDB datasource          : " << std::endl;
    BRAYNS_INFO << "- Folder                : " << _pdbFolder << std::endl;
    BRAYNS_INFO << "- Cells                 : " << _pdbCells << std::endl;
    BRAYNS_INFO << "- Positions             : " << _pdbPositions << std::endl;
    BRAYNS_INFO << "Mesh folder             : " << _meshFolder << std::endl;
    BRAYNS_INFO << "Cache file to load      : " << _loadCacheFile << std::endl;
    BRAYNS_INFO << "Cache file to save      : " << _saveCacheFile << std::endl;
    BRAYNS_INFO << "Circuit configuration   : " << _circuitConfig << std::endl;
    BRAYNS_INFO << "Color scheme            : " <<
        static_cast<size_t>( _colorScheme ) << std::endl;
    BRAYNS_INFO << "Radius                  : " << _radius << std::endl;
    BRAYNS_INFO << "Scene environment       : " <<
        static_cast<size_t>( _sceneEnvironment ) << std::endl;
    BRAYNS_INFO << "Geometry quality        : " <<
        static_cast<size_t>( _geometryQuality ) << std::endl;
    BRAYNS_INFO << "Target                  : " <<
        _target << std::endl;
    BRAYNS_INFO << "Morphology section types: " <<
        _morphologySectionTypes << std::endl;
    BRAYNS_INFO << "Morphology Layout       : " << std::endl;
    BRAYNS_INFO << " - Columns              : " <<
        _morphologyLayout.nbColumns << std::endl;
    BRAYNS_INFO << " - Vertical spacing     : " <<
        _morphologyLayout.verticalSpacing << std::endl;
    BRAYNS_INFO << " - Horizontal spacing   : " <<
        _morphologyLayout.horizontalSpacing << std::endl;
}

}
