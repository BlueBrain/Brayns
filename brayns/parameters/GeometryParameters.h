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

#ifndef GEOMETRYPARAMETERS_H
#define GEOMETRYPARAMETERS_H

#include "AbstractParameters.h"

#include <brayns/common/types.h>

namespace brayns
{

/**
 * Defines how morphologies should be organized in space when the layout mode is
 * selected. The idea is to present the morphology in a grid with a given number
 * of columns, and a spacing in between. The spacing scale is the same as the
 * one from the morphologies.
 */
struct MorphologyLayout
{
    MorphologyLayout()
    : nbColumns( 0 )
    , verticalSpacing( 0 )
    , horizontalSpacing( 0 )
      {}

    size_t nbColumns;
    size_t verticalSpacing;
    size_t horizontalSpacing;
};

/** Manages geometry parameters
 */
class GeometryParameters : public AbstractParameters
{
public:
    /**
       Parse the command line parameters and populates according class members
     */
    GeometryParameters( );

    /** @copydoc AbstractParameters::print */
    void print( ) final;

    /** folder containing SWC and H5 files */
    std::string getMorphologyFolder( ) const { return _morphologyFolder; }

    /**
     * @brief getNESTCircuit
     * @return
     */
    std::string getNESTCircuit( ) const { return _NESTCircuit; }
    std::string getNESTReport( ) const { return _NESTReport; }
    std::string getNESTCacheFile( ) const { return _NESTCacheFile; }

    /** PDB file */
    std::string getPDBFile( ) const { return _pdbFile; }

    /** PDB folder */
    std::string getPDBFolder( ) const { return _pdbFolder; }

    /** XYZR file */
    std::string getXYZBFile( ) const { return _xyzbFile; }

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

    /** Circuit compartment report */
    std::string getReport( ) const { return _report; }

    /** Radius multiplier applied to spheres, cones and cylinders.
     * @param value Radius multiplier. Multiplies the radius contained in the
     *        data source by the specified value.
     */
    void setRadiusMultiplier( const float value ) { _radiusMultiplier = value; }
    float getRadiusMultiplier( ) const { return _radiusMultiplier; }

    /** Radius correction applied to spheres and cylinders.
     * @param value Radius value. The radius contained in the data source is
     *        ignored and all geometries use the specified value.
     */
    void setRadiusCorrection( const float value ) { _radiusCorrection = value; }
    float getRadiusCorrection( ) const { return _radiusCorrection; }

    /** Enables a different color for every molecule/morphology/mesh when
    * loading them from a given folder
    */
    ColorScheme getColorScheme( ) const { return _colorScheme; }
    const std::string& getColorSchemeAsString( const ColorScheme value ) const;
    void setColorScheme( const ColorScheme value ) { _colorScheme = value; }

    /** Scene environment (none, ground, wall, bounding_box ) */
    SceneEnvironment getSceneEnvironment() const { return _sceneEnvironment; }
    const std::string& getSceneEnvironmentAsString( const SceneEnvironment value ) const;

    /** Morphology quality */
    GeometryQuality getGeometryQuality( ) const { return _geometryQuality; }
    const std::string& getGeometryQualityAsString( const GeometryQuality value ) const;

    /** Morphology section types*/
    size_t getMorphologySectionTypes( ) const
    {
        return _morphologySectionTypes;
    }

    /** Morphology layout */
    const MorphologyLayout& getMorphologyLayout() const
    {
        return _morphologyLayout;
    }

    /** Defines if cells with no simulation data should be loaded */
    size_t getNonSimulatedCells() const { return _nonSimulatedCells; }

    /** Defines the range of frames to be loaded for the simulation */
    float getEndSimulationTime() const { return _endSimulationTime; }
    float getStartSimulationTime() const { return _startSimulationTime; }
    Vector2f getSimulationValuesRange() const { return _simulationValuesRange; }

    /** File containing simulation data */
    const std::string& getSimulationCacheFile() const { return _simulationCacheFile; }

    /** Size of the simulation histogram */
    size_t getSimulationHistogramSize() const { return _simulationHistogramSize; }

    /** Defines if multiple models should be generated to increase the
        rendering performance */
    bool getGenerateMultipleModels() const { return _generateMultipleModels; }

    /** Splash scene folder */
    void setSplashSceneFolder( const std::string& value ) { _splashSceneFolder = value; }
    std::string getSplashSceneFolder() const { return _splashSceneFolder; }

    /** Biological assembly */
    const std::string& getMolecularSystemConfig() const { return _molecularSystemConfig; }

protected:

    bool _parse( const po::variables_map& vm ) final;

    std::string _morphologyFolder;
    std::string _NESTCircuit;
    std::string _NESTReport;
    std::string _NESTCacheFile;
    std::string _pdbFile;
    std::string _pdbFolder;
    std::string _xyzbFile;
    std::string _h5Folder;
    std::string _meshFolder;
    std::string _circuitConfig;
    std::string _loadCacheFile;
    std::string _saveCacheFile;
    std::string _target;
    std::string _report;
    float  _radiusMultiplier;
    float  _radiusCorrection;
    ColorScheme _colorScheme;
    SceneEnvironment _sceneEnvironment;
    GeometryQuality _geometryQuality;
    size_t _morphologySectionTypes;
    MorphologyLayout _morphologyLayout;
    size_t _nonSimulatedCells;
    float _startSimulationTime;
    float _endSimulationTime;
    Vector2f _simulationValuesRange;
    std::string _simulationCacheFile;
    size_t _simulationHistogramSize;
    bool _generateMultipleModels;
    std::string _splashSceneFolder;
    std::string _molecularSystemConfig;

};

}
#endif // GEOMETRYPARAMETERS_H
