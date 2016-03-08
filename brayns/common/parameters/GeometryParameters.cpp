/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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
const std::string PARAM_PDB_FOLDER = "pdb-folder";
const std::string PARAM_MESH_FOLDER = "mesh-folder";
const std::string PARAM_CIRCUIT_CONFIG = "circuit-config";
const std::string PARAM_RADIUS = "radius";
const std::string PARAM_COLOR_SCHEME = "color-scheme";
const std::string PARAM_SCENE_ENVIRONMENT = "scene-environment";
const std::string PARAM_GEOMETRY_QUALITY = "geometry-quality";
const std::string PARAM_TARGET = "target";
const std::string PARAM_MORPHOLOGY_SECTION_TYPES = "morphology-section-types";

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
        ( PARAM_RADIUS.c_str(), po::value< float >( ),
            "Radius multiplier for spheres, cones and cylinders" )
        ( PARAM_COLOR_SCHEME.c_str( ), po::value< size_t >( ),
            "Color scheme to be applied to the geometry" )
        ( PARAM_SCENE_ENVIRONMENT.c_str(), po::value< int >( ),
            "Scene environment (0: none, 1: ground, 2: wall, 3: box)" )
        ( PARAM_GEOMETRY_QUALITY.c_str(), po::value< size_t >( ),
            "Geometry rendering quality (0: Fast rendering, "
                "1: Medium rendering, 2: Max quality)" )
        ( PARAM_TARGET.c_str(), po::value< std::string >( ),
            "Circuit target to load" )
        ( PARAM_MORPHOLOGY_SECTION_TYPES.c_str(), po::value< size_t > ( ),
            "Morphology section types (1: soma, 2: axon, 4: dendrite, "
            "8: apical dendrite). Values can be added to select more than "
            "one type of section" );
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
}

}
