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

#ifndef GEOMETRYPARAMETERS_H
#define GEOMETRYPARAMETERS_H

#include "AbstractParameters.h"

#include <brayns/common/types.h>

namespace brayns
{

/** Manages geometry parameters
 */
class GeometryParameters : public AbstractParameters
{
public:
    /**
       Parse the command line parameters and populates according class members
     */
    GeometryParameters( );

    /** @copydoc AbstractParameters::parse */
    virtual bool parse( int argc, const char **argv ) final;

    /** @copydoc AbstractParameters::print */
    void print( ) final;

    /** folder containing SWC and H5 files */
    std::string getMorphologyFolder( ) const { return _morphologyFolder; }

    /** folder containing PDB files */
    std::string getPDBFolder( ) const { return _pdbFolder; }

    /** folder containing PDB cells */
    std::string getPDBCells( ) const { return _pdbCells; }

    /** folder containing PDB positions */
    std::string getPDBPositions( ) const { return _pdbPositions; }

    /** folder containing mesh files */
    std::string getMeshFolder( ) const { return _meshFolder; }

    /** file containing circuit configuration */
    std::string getCircuitConfiguration( ) const { return _circuitConfig; }

    /** Binary representation of a scene to load */
    std::string getLoadCacheFile( ) const { return _loadCacheFile; }

    /** Binary representation of a scene to save */
    std::string getSaveCacheFile( ) const { return _saveCacheFile; }

    /** Circuit target */
    std::string getTarget( ) const { return _target; }

    /** Radius multiplier applied to spheres, cones and cylinders */
    float getRadius( ) const { return _radius; }
    void setRadius( const float value ) { _radius = value; }

    /** Enables a different color for every morphology/mesh when
    * loading them from a given folder
    */
    ColorScheme getColorScheme( ) const { return _colorScheme; }
    void setColorScheme( const ColorScheme value ) { _colorScheme = value; }

    /** Scene environment (0: none, 1: ground, 2: box ) */
    SceneEnvironment getSceneEnvironment( ) const { return _sceneEnvironment; }

    /** Morphology quality */
    GeometryQuality getGeometryQuality( ) const { return _geometryQuality; }

    /** Morphology section types*/
    size_t getMorphologySectionTypes( ) const
    {
        return _morphologySectionTypes;
    }

protected:
    std::string _morphologyFolder;
    std::string _pdbFolder;
    std::string _pdbCells;
    std::string _pdbPositions;
    std::string _h5Folder;
    std::string _meshFolder;
    std::string _circuitConfig;
    std::string _loadCacheFile;
    std::string _saveCacheFile;
    std::string _target;
    float  _radius;
    ColorScheme _colorScheme;
    SceneEnvironment _sceneEnvironment;
    GeometryQuality _geometryQuality;
    size_t _morphologySectionTypes;
};

}
#endif // GEOMETRYPARAMETERS_H
