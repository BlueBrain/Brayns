/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
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
#include <brayns/common/log.h>
#include <brayns/common/exceptions.h>

#include <boost/lexical_cast.hpp>

namespace brayns
{

const std::string PARAM_SWC_FOLDER = "--swc-folder";
const std::string PARAM_H5_FOLDER = "--h5-folder";
const std::string PARAM_PDB_FOLDER = "--pdb-folder";
const std::string PARAM_MESH_FOLDER = "--mesh-folder";
const std::string PARAM_RADIUS = "--radius";
const std::string PARAM_COLORED = "--colored";
const std::string PARAM_TIMED_GEOMETRY_INCREMENT = "--timed-geometry-increment";
const std::string PARAM_SCENE_ENVIRONMENT = "--scene-environment";

GeometryParameters::GeometryParameters(int argc, const char **argv)
    : AbstractParameters(argc, argv)
    ,_radius(1), _colored(false), _timedGeometry(false)
    , _timedGeometryIncrement(1), _sceneEnvironment(SE_NONE)
{
    _parameters[PARAM_SWC_FOLDER] =
        {PMT_STRING, "Folder containing SWC files"};
    _parameters[PARAM_H5_FOLDER] =
        {PMT_STRING, "Folder containing H5 files"};
    _parameters[PARAM_PDB_FOLDER] =
        {PMT_STRING, "Folder containing PDB files with positions"};
    _parameters[PARAM_MESH_FOLDER] =
        {PMT_STRING, "Folder containing PARAM_BENCHMARKINGmeshes"};
    _parameters[PARAM_RADIUS] =
        {PMT_FLOAT, "Radius multiplier for spheres, cones and cylinders"};
    _parameters[PARAM_COLORED] =
        {PMT_BOOLEAN, "Sets different color to every morphology"};
    _parameters[PARAM_TIMED_GEOMETRY_INCREMENT] =
        {PMT_INTEGER, "Increment between frames"};
    _parameters[PARAM_SCENE_ENVIRONMENT] =
        {PMT_INTEGER, "Scene environment (0: none, 1: ground, 2: wall, 3: box)"};

    for (int i=1;i<argc;i++)
    {
        std::string arg = argv[i];
        if(arg == PARAM_SWC_FOLDER)
            _swcFolder = argv[++i];
        else if (arg == PARAM_H5_FOLDER)
            _h5Folder = argv[++i];
        else if (arg == PARAM_PDB_FOLDER)
        {
            _pdbFolder = argv[++i];
            _pdbCells = argv[++i];
            _pdbPositions = argv[++i];
        }
        else if (arg == PARAM_MESH_FOLDER)
            _meshFolder = argv[++i];
        else if (arg==PARAM_COLORED)
            _colored = true;
        else if (arg==PARAM_RADIUS)
            _radius = boost::lexical_cast<float>(argv[++i]);
        else if (arg==PARAM_TIMED_GEOMETRY_INCREMENT)
        {
            _timedGeometry = true;
            _timedGeometryIncrement = boost::lexical_cast<float>(argv[++i]);
        }
        else if (arg == PARAM_SCENE_ENVIRONMENT)
            _sceneEnvironment = static_cast<SceneEnvironment>(
                boost::lexical_cast<size_t>(argv[++i]));
    }
}

void GeometryParameters::display() const
{
    BRAYNS_INFO << "Geometry options: " << std::endl;
    BRAYNS_INFO << "- SWC folder    : " << _swcFolder << std::endl;
    BRAYNS_INFO << "- PDB folder    : " << _pdbFolder << std::endl;
    BRAYNS_INFO << "- H5 folder     : " << _h5Folder << std::endl;
    BRAYNS_INFO << "- Mesh folder   : " << _meshFolder << std::endl;
    BRAYNS_INFO << "- Colored                  : " <<
                   (_colored ? "on": "off") << std::endl;
    BRAYNS_INFO << "- Radius                   : " <<
                   _radius << std::endl;
    BRAYNS_INFO << "- Timed geometry increment : " <<
                   _timedGeometryIncrement << std::endl;
}

}
