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
        : nbColumns(0)
        , verticalSpacing(0)
        , horizontalSpacing(0)
    {
    }

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
    GeometryParameters();

    /** @copydoc AbstractParameters::print */
    void print() final;

    /** folder containing SWC and H5 files */
    std::string getMorphologyFolder() const { return _morphologyFolder; }
    /**
     * @brief getNESTCircuit
     * @return
     */
    std::string getNESTCircuit() const { return _NESTCircuit; }
    std::string getNESTReport() const { return _NESTReport; }
    std::string getNESTCacheFile() const { return _NESTCacheFile; }
    /** PDB file */
    std::string getPDBFile() const { return _pdbFile; }
    /** PDB folder */
    std::string getPDBFolder() const { return _pdbFolder; }
    /** XYZR file */
    std::string getXYZBFile() const { return _xyzbFile; }
    /** folder containing mesh files */
    std::string getMeshFolder() const { return _meshFolder; }
    /** Fesh containting mesh*/
    std::string getMeshFile() const { return _meshFile; }
    /** file containing circuit configuration */
    std::string getCircuitConfiguration() const
    {
        return _circuitConfiguration;
    }
    /** Binary representation of a scene to load */
    std::string getLoadCacheFile() const { return _loadCacheFile; }
    /** Binary representation of a scene to save */
    std::string getSaveCacheFile() const { return _saveCacheFile; }
    /** Circuit target */
    std::string getCircuitTarget() const { return _circuitTarget; }
    /** Circuit compartment report */
    std::string getCircuitReport() const { return _circuitReport; }
    /** Defines the folder where morphologies meshes are stored. Meshes must
     * have the same name as the h5/SWC morphology file, suffixed with an
     * extension supported by the assimp library
     */
    std::string getCircuitMeshFolder() const { return _circuitMeshFolder; }
    /** ensity of cells in the circuit in percent (Mainly for testing
     * purposes) */
    float getCircuitDensity() const;

    /**
     * Defines a bounding box outside of which geometry of a circuit will not be
     * loaded
     */
    const Boxf& getCircuitBoundingBox() const { return _circuitBoundingBox; }
    void setCircuitBoundingBox(const Boxf& value)
    {
        _circuitBoundingBox = value;
    }

    /**
     * Defines if a different model is used to handle the simulation geometry.
     * If set to True, the shading of the main geometry model will be done
     * using information stored in a secondary model that contains the
     * simulation information. See OSPRay simulation renderer for more details.
     */
    bool getCircuitUseSimulationModel() const
    {
        return _circuitUseSimulationModel;
    }
    void setCircuitUseSimulationModel(const bool value)
    {
        _circuitUseSimulationModel = value;
    }
    /**
     * Return the filename pattern use to load meshes
     */
    std::string getCircuitMeshFilenamePattern() const
    {
        return _circuitMeshFilenamePattern;
    };
    /** Radius multiplier applied to spheres, cones and cylinders.
     * @param value Radius multiplier. Multiplies the radius contained in the
     *        data source by the specified value.
     */
    void setRadiusMultiplier(const float value) { _radiusMultiplier = value; }
    float getRadiusMultiplier() const { return _radiusMultiplier; }
    /** Radius correction applied to spheres and cylinders.
     * @param value Radius value. The radius contained in the data source is
     *        ignored and all geometries use the specified value.
     */
    void setRadiusCorrection(const float value) { _radiusCorrection = value; }
    float getRadiusCorrection() const { return _radiusCorrection; }
    /** Enables a different color for every molecule/morphology/mesh when
    * loading them from a given folder
    */
    ColorScheme getColorScheme() const { return _colorScheme; }
    const std::string& getColorSchemeAsString(const ColorScheme value) const;
    void setColorScheme(const ColorScheme value) { _colorScheme = value; }
    /** Scene environment (none, ground, wall, bounding_box ) */
    SceneEnvironment getSceneEnvironment() const { return _sceneEnvironment; }
    const std::string& getSceneEnvironmentAsString(
        const SceneEnvironment value) const;

    /** Morphology quality */
    GeometryQuality getGeometryQuality() const { return _geometryQuality; }
    const std::string& getGeometryQualityAsString(
        const GeometryQuality value) const;

    /** Morphology section types*/
    size_t getMorphologySectionTypes() const { return _morphologySectionTypes; }
    /** Morphology layout */
    const MorphologyLayout& getMorphologyLayout() const
    {
        return _morphologyLayout;
    }

    /** Defines if cells with no simulation data should be loaded */
    size_t getCircuitNonSimulatedCells() const
    {
        return _circuitNonSimulatedCells;
    }
    /** Defines the range of frames to be loaded for the simulation */
    float getCircuitEndSimulationTime() const
    {
        return _circuitEndSimulationTime;
    }
    float getCircuitStartSimulationTime() const
    {
        return _circuitStartSimulationTime;
    }
    Vector2f getCircuitSimulationValuesRange() const
    {
        return _circuitSimulationValuesRange;
    }
    /** File containing simulation data */
    const std::string& getCircuitSimulationCacheFile() const
    {
        return _circuitSimulationCacheFile;
    }

    /** Size of the simulation histogram */
    size_t getCircuitSimulationHistogramSize() const
    {
        return _circuitSimulationHistogramSize;
    }

    /** Size of the simulation histogram */
    size_t getCircuitMeshTransformation() const
    {
        return _circuitMeshTransformation;
    }

    /** Defines if multiple models should be generated to increase the
        rendering performance */
    bool getGenerateMultipleModels() const { return _generateMultipleModels; }
    /** Splash scene folder */
    void setSplashSceneFolder(const std::string& value)
    {
        _splashSceneFolder = value;
    }
    std::string getSplashSceneFolder() const { return _splashSceneFolder; }
    /** Biological assembly */
    const std::string& getMolecularSystemConfig() const
    {
        return _molecularSystemConfig;
    }

    /** Metaballs grid size */
    size_t getMetaballsGridSize() const { return _metaballsGridSize; }
    /** Metaballs threshold */
    float getMetaballsThreshold() const { return _metaballsThreshold; }
    /** Metaballs samples from soma */
    size_t getMetaballsSamplesFromSoma() const
    {
        return _metaballsSamplesFromSoma;
    }

    /** Metaballs enabled? */
    bool useMetaballs() const { return _metaballsGridSize != 0; }
    /**
     * Defines what memory mode should be used between Brayns and the
     * underlying renderer
     */
    MemoryMode getMemoryMode() const { return _memoryMode; };
    /**
     * Return the full path of the file containing a scene description
     */
    std::string getSceneFile() const { return _sceneFile; };
protected:
    bool _parse(const po::variables_map& vm) final;

    // Nest
    std::string _NESTCircuit;
    std::string _NESTReport;
    std::string _NESTCacheFile;

    // PDB
    std::string _pdbFile;
    std::string _pdbFolder;

    // XYZ
    std::string _xyzbFile;

    // Mesh
    std::string _meshFolder;
    std::string _meshFile;

    // Circuit
    std::string _circuitConfiguration;
    bool _circuitUseSimulationModel;
    Boxf _circuitBoundingBox;
    float _circuitDensity;
    std::string _circuitMeshFilenamePattern;
    std::string _circuitMeshFolder;
    std::string _circuitTarget;
    std::string _circuitReport;
    size_t _circuitNonSimulatedCells;
    float _circuitStartSimulationTime;
    float _circuitEndSimulationTime;
    Vector2f _circuitSimulationValuesRange;
    std::string _circuitSimulationCacheFile;
    size_t _circuitSimulationHistogramSize;
    bool _circuitMeshTransformation;

    // Scene
    std::string _loadCacheFile;
    std::string _saveCacheFile;
    SceneEnvironment _sceneEnvironment;
    std::string _splashSceneFolder;
    std::string _sceneFile;

    // Morphology
    std::string _morphologyFolder;
    float _radiusMultiplier;
    float _radiusCorrection;
    ColorScheme _colorScheme;
    GeometryQuality _geometryQuality;
    size_t _morphologySectionTypes;
    MorphologyLayout _morphologyLayout;
    bool _generateMultipleModels;
    std::string _molecularSystemConfig;
    size_t _metaballsGridSize;
    float _metaballsThreshold;
    size_t _metaballsSamplesFromSoma;

    // System parameters
    MemoryMode _memoryMode;
};
}
#endif // GEOMETRYPARAMETERS_H
