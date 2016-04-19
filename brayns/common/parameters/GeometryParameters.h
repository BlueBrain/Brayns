/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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
    MorphologyLayout() : type( ML_NONE ) {}

    MorphologyLayoutType type;
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

    /** Morphology layout */
    const MorphologyLayout& getMorphologyLayout() const
    {
        return _morphologyLayout;
    }

protected:

    bool _parse( const po::variables_map& vm ) final;

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
    MorphologyLayout _morphologyLayout;
};

}
#endif // GEOMETRYPARAMETERS_H
