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

const std::string PARAM_SWC_FOLDER = "swc-folder";
const std::string PARAM_H5_FOLDER = "h5-folder";
const std::string PARAM_PDB_FOLDER = "pdb-folder";
const std::string PARAM_MESH_FOLDER = "mesh-folder";
const std::string PARAM_RADIUS = "radius";
const std::string PARAM_COLOR_SCHEME = "color-scheme";
const std::string PARAM_TIMED_GEOMETRY_INCREMENT = "timed-geometry-increment";
const std::string PARAM_SCENE_ENVIRONMENT = "scene-environment";

}

namespace brayns
{

namespace po = boost::program_options;

GeometryParameters::GeometryParameters( )
    : AbstractParameters( "Geometry" )
    ,_radius(1), _colorScheme(CS_NONE), _timedGeometry(false)
    , _timedGeometryIncrement(1), _sceneEnvironment(SE_NONE)
{
    _parameters.add_options()
        ( PARAM_SWC_FOLDER.c_str(), po::value< std::string >( ),
            "Folder containing SWC files" )
        ( PARAM_H5_FOLDER.c_str(), po::value< std::string >( ),
            "Folder containing H5 files" )
        ( PARAM_MESH_FOLDER.c_str(), po::value< std::string >( ),
            "Folder containing mesh files" )
        ( PARAM_PDB_FOLDER.c_str(), po::value< strings >( )->multitoken(),
            "Folder containing PDB files" )
        ( PARAM_RADIUS.c_str(), po::value< float >( ),
            "Radius multiplier for spheres, cones and cylinders" )
        ( PARAM_COLOR_SCHEME.c_str( ), po::value< size_t >( ),
            "Color scheme to be applied to the geometry" )
        ( PARAM_TIMED_GEOMETRY_INCREMENT.c_str(), po::value< int >( ),
            "Increment between frames" )
        ( PARAM_SCENE_ENVIRONMENT.c_str(), po::value< int >( ),
            "Scene environment (0: none, 1: ground, 2: wall, 3: box)" );
}

bool GeometryParameters::parse( int argc, const char **argv )
{
    AbstractParameters::parse( argc, argv );

    if( _vm.count( PARAM_SWC_FOLDER ))
        _swcFolder = _vm[PARAM_SWC_FOLDER].as< std::string >( );
    if( _vm.count( PARAM_H5_FOLDER ))
        _h5Folder = _vm[PARAM_H5_FOLDER].as< std::string >( );
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
    if( _vm.count( PARAM_COLOR_SCHEME ))
        _colorScheme = static_cast< ColorScheme >(
            _vm[PARAM_COLOR_SCHEME].as< size_t >( ));
    if( _vm.count( PARAM_RADIUS))
        _radius = _vm[PARAM_RADIUS].as< float >( );
    if( _vm.count( PARAM_TIMED_GEOMETRY_INCREMENT ))
    {
        _timedGeometry = true;
        _timedGeometryIncrement = _vm[PARAM_TIMED_GEOMETRY_INCREMENT].as< float >();
    }
    if( _vm.count( PARAM_SCENE_ENVIRONMENT ))
        _sceneEnvironment = static_cast< SceneEnvironment >(
            _vm[PARAM_SCENE_ENVIRONMENT].as< size_t >( ));

    return true;
}

void GeometryParameters::print( )
{
    AbstractParameters::print( );
    BRAYNS_INFO << "SWC folder              : " << _swcFolder << std::endl;
    BRAYNS_INFO << "H5 folder               : " << _h5Folder << std::endl;
    BRAYNS_INFO << "PDB datasource          : " << std::endl;
    BRAYNS_INFO << "- Folder                : " << _pdbFolder << std::endl;
    BRAYNS_INFO << "- Cells                 : " << _pdbCells << std::endl;
    BRAYNS_INFO << "- Positions             : " << _pdbPositions << std::endl;
    BRAYNS_INFO << "Mesh folder             : " << _meshFolder << std::endl;
    BRAYNS_INFO << "Color scheme            : " <<
        static_cast<size_t>( _colorScheme ) << std::endl;
    BRAYNS_INFO << "Radius                  : " << _radius << std::endl;
    BRAYNS_INFO << "Timed geometry          : " <<
        ( _timedGeometry ? "on" : "off" ) << std::endl;
    BRAYNS_INFO << "Scene environment       : " <<
        static_cast<size_t>( _sceneEnvironment ) << std::endl;
}

}
