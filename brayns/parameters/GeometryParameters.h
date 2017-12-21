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

SERIALIZATION_ACCESS(GeometryParameters)

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
    size_t nbColumns{0};
    size_t verticalSpacing{0};
    size_t horizontalSpacing{0};
};

struct CircuitConfiguration
{
    std::string _circuitConfiguration;
    bool _circuitUseSimulationModel{false};
    Boxf _circuitBoundingBox{{0, 0, 0}, {0, 0, 0}};
    float _circuitDensity{100};
    std::string _circuitMeshFilenamePattern;
    std::string _circuitMeshFolder;
    std::string _circuitTargets;
    std::string _circuitReport;
    double _circuitStartSimulationTime{0};
    double _circuitEndSimulationTime{std::numeric_limits<float>::max()};
    double _circuitSimulationStep{0};
    Vector2f _circuitSimulationValuesRange{std::numeric_limits<float>::max(),
                                           std::numeric_limits<float>::min()};
    size_t _circuitSimulationHistogramSize{128};
    bool _circuitMeshTransformation{false};
};

struct ConnectivityConfiguration
{
    std::string _connectivityFile;
    size_t _connectivityMatrixId{0};
    bool _connectivityShowConnections{false};
    Vector2ui _connectivityDimensionRange{
        0, std::numeric_limits<unsigned int>::max()};
    Vector3f _connectivityScale{1.f, 1.f, 1.f};
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
    const std::string& getMorphologyFolder() const { return _morphologyFolder; }
    /**
     * @brief getNESTCircuit
     * @return
     */
    const std::string& getNESTCircuit() const { return _NESTCircuit; }
    const std::string& getNESTReport() const { return _NESTReport; }
    const std::string& getNESTCacheFile() const { return _NESTCacheFile; }
    /** PDB file */
    const std::string& getPDBFile() const { return _pdbFile; }
    /** PDB folder */
    const std::string& getPDBFolder() const { return _pdbFolder; }
    /** XYZR file */
    const std::string& getXYZBFile() const { return _xyzbFile; }
    /** folder containing mesh files */
    const std::string& getMeshFolder() const { return _meshFolder; }
    /** Fesh containting mesh*/
    const std::string& getMeshFile() const { return _meshFile; }
    /** file containing circuit configuration */
    const std::string& getCircuitConfiguration() const
    {
        return _circuitConfiguration._circuitConfiguration;
    }
    /** Binary representation of a scene to load */
    const std::string& getLoadCacheFile() const { return _loadCacheFile; }
    /** Binary representation of a scene to save */
    const std::string& getSaveCacheFile() const { return _saveCacheFile; }
    /** Circuit targets */
    const std::string& getCircuitTargets() const
    {
        return _circuitConfiguration._circuitTargets;
    }
    strings getCircuitTargetsAsStrings() const;
    /** Circuit compartment report */
    const std::string& getCircuitReport() const
    {
        return _circuitConfiguration._circuitReport;
    }
    /** Defines the folder where morphologies meshes are stored. Meshes must
     * have the same name as the h5/SWC morphology file, suffixed with an
     * extension supported by the assimp library
     */
    const std::string& getCircuitMeshFolder() const
    {
        return _circuitConfiguration._circuitMeshFolder;
    }
    /** ensity of cells in the circuit in percent (Mainly for testing
     * purposes) */
    float getCircuitDensity() const;

    /**
     * Defines a bounding box outside of which geometry of a circuit will not be
     * loaded
     */
    const Boxf& getCircuitBoundingBox() const
    {
        return _circuitConfiguration._circuitBoundingBox;
    }
    void setCircuitBoundingBox(const Boxf& value)
    {
        _updateValue(_circuitConfiguration._circuitBoundingBox, value);
    }

    /**
     * Defines if a different model is used to handle the simulation geometry.
     * If set to True, the shading of the main geometry model will be done
     * using information stored in a secondary model that contains the
     * simulation information. See OSPRay simulation renderer for more details.
     */
    bool getCircuitUseSimulationModel() const
    {
        return _circuitConfiguration._circuitUseSimulationModel;
    }
    void setCircuitUseSimulationModel(const bool value)
    {
        _updateValue(_circuitConfiguration._circuitUseSimulationModel, value);
    }
    /**
     * Return the filename pattern use to load meshes
     */
    const std::string& getCircuitMeshFilenamePattern() const
    {
        return _circuitConfiguration._circuitMeshFilenamePattern;
    }
    /** Radius multiplier applied to spheres, cones and cylinders.
     * @param value Radius multiplier. Multiplies the radius contained in the
     *        data source by the specified value.
     */
    void setRadiusMultiplier(const float value)
    {
        _updateValue(_radiusMultiplier, value);
    }
    float getRadiusMultiplier() const { return _radiusMultiplier; }
    /** Radius correction applied to spheres and cylinders.
     * @param value Radius value. The radius contained in the data source is
     *        ignored and all geometries use the specified value.
     */
    void setRadiusCorrection(const float value)
    {
        _updateValue(_radiusCorrection, value);
    }
    float getRadiusCorrection() const { return _radiusCorrection; }
    /** Enables a different color for every molecule/morphology/mesh when
    * loading them from a given folder
    */
    ColorScheme getColorScheme() const { return _colorScheme; }
    const std::string& getColorSchemeAsString(const ColorScheme value) const;
    void setColorScheme(const ColorScheme value)
    {
        _updateValue(_colorScheme, value);
    }
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

    /** Defines the range of frames to be loaded for the simulation */
    double getCircuitEndSimulationTime() const
    {
        return _circuitConfiguration._circuitEndSimulationTime;
    }
    double getCircuitStartSimulationTime() const
    {
        return _circuitConfiguration._circuitStartSimulationTime;
    }
    double getCircuitSimulationStep() const
    {
        return _circuitConfiguration._circuitSimulationStep;
    }
    Vector2f getCircuitSimulationValuesRange() const
    {
        return _circuitConfiguration._circuitSimulationValuesRange;
    }

    /** Size of the simulation histogram */
    size_t getCircuitSimulationHistogramSize() const
    {
        return _circuitConfiguration._circuitSimulationHistogramSize;
    }

    /** Size of the simulation histogram */
    size_t getCircuitMeshTransformation() const
    {
        return _circuitConfiguration._circuitMeshTransformation;
    }

    /** Splash scene folder */
    void setSplashSceneFolder(const std::string& value)
    {
        _updateValue(_splashSceneFolder, value);
    }
    const std::string& getSplashSceneFolder() const
    {
        return _splashSceneFolder;
    }
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

    /** Realistic somas enabled? */
    bool useRealisticSomas() const { return _metaballsGridSize != 0; }
    /**
     * Defines what memory mode should be used between Brayns and the
     * underlying renderer
     */
    MemoryMode getMemoryMode() const { return _memoryMode; };
    /**
     * Return the full path of the file containing a scene description
     */
    const std::string& getSceneFile() const { return _sceneFile; };
    /** File containing neuron matrix */
    const std::string& getConnectivityFile() const
    {
        return _connectivityConfiguration._connectivityFile;
    }
    /** Matrix id */
    size_t getConnectivityMatrixId() const
    {
        return _connectivityConfiguration._connectivityMatrixId;
    }
    /** Show/Hide connection in matrix */
    size_t getConnectivityShowConnections() const
    {
        return _connectivityConfiguration._connectivityShowConnections;
    }
    /** Range of dimensions */
    Vector2ui getConnectivityDimensionRange() const
    {
        return _connectivityConfiguration._connectivityDimensionRange;
    }
    /** Range of dimensions */
    Vector3f getConnectivityScale() const
    {
        return _connectivityConfiguration._connectivityScale;
    }

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
    CircuitConfiguration _circuitConfiguration;

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

    ConnectivityConfiguration _connectivityConfiguration;

    SERIALIZATION_FRIEND(GeometryParameters)
};
}
#endif // GEOMETRYPARAMETERS_H
