/* Copyright (c) 2018-2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/favreau/Brayns-UC-CircuitExplorer>
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

#ifndef MEMBRANELESS_ORGANELLES_PLUGIN_H
#define MEMBRANELESS_ORGANELLES_PLUGIN_H

#include <plugin/api/CellObjectMapper.h>
#include <plugin/api/CircuitExplorerParams.h>
#include <plugin/io/AbstractCircuitLoader.h>

#include <array>
#include <brayns/common/types.h>
#include <brayns/pluginapi/ExtensionPlugin.h>
#include <vector>

/**
 * @brief The CircuitExplorerPlugin class manages the loading and visualization
 * of the Blue Brain Project micro-circuits, and allows visualisation of voltage
 * simulations
 */
class CircuitExplorerPlugin : public brayns::ExtensionPlugin
{
public:
    CircuitExplorerPlugin();

    void init() final;

    /**
     * @brief preRender Updates the scene according to latest data load
     */
    void preRender() final;
    void postRender() final;

    CellObjectMapper& getMapperForCircuit(const std::string& circuitFilePath);
    void releaseCircuitMapper(const std::string& circuitFilePath);

private:
    // Rendering
    MessageResult _setCamera(const CameraDefinition&);
    CameraDefinition _getCamera();
    MessageResult _setMaterial(const MaterialDescriptor&);
    MessageResult _setMaterials(const MaterialsDescriptor&);
    MessageResult _setMaterialRange(const MaterialRangeDescriptor&);
    MessageResult _setMaterialExtraAttributes(const MaterialExtraAttributes&);

    // Experimental
    MessageResult _setSynapseAttributes(const SynapseAttributes&);
    MessageResult _setConnectionsPerValue(const ConnectionsPerValue&);
    MessageResult _setMetaballsPerSimulationValue(const MetaballsFromSimulationValue&);
    MessageResult _saveModelToCache(const SaveModelToCache&);

    // Handlers
    MessageResult _attachCellGrowthHandler(const AttachCellGrowthHandler& payload);
    MessageResult _attachCircuitSimulationHandler(
        const AttachCircuitSimulationHandler& payload);

    // Movie production
    MessageResult _exportFramesToDisk(const ExportFramesToDisk& payload);
    void _doExportFrameToDisk();
    FrameExportProgress _getFrameExportProgress();
    ExportLayerToDiskResult _exportLayerToDisk(const ExportLayerToDisk& payload);
    MessageResult _makeMovie(const MakeMovieParameters& params);

    // Anterograde tracing
    MessageResult _traceAnterogrades(const AnterogradeTracing& payload);

    // Add geometry
    void _createShapeMaterial(brayns::ModelPtr& model,
                              const size_t id,
                              const brayns::Vector3d& color,
                              const double& opacity);
    AddShapeResult _addSphere(const AddSphere& payload);
    AddShapeResult _addPill(const AddPill& payload);
    AddShapeResult _addCylinder(const AddCylinder& payload);
    AddShapeResult _addBox(const AddBox& payload);


    // Predefined models
    MessageResult _addGrid(const AddGrid& payload);
    MessageResult _addColumn(const AddColumn& payload);

    // Get material information
    MaterialIds _getMaterialIds(const ModelId& modelId);
    MaterialDescriptor _getMaterial(const ModelMaterialId& mmId);

    // Remap circuit colors to a specific scheme
    MessageResult _remapCircuitToScheme(const RemapCircuit& payload);

    SynapseAttributes _synapseAttributes;

    bool _dirty{false};

    ExportFramesToDisk _exportFramesToDiskPayload;
    bool _exportFramesToDiskDirty{false};
    uint16_t _frameNumber{0};
    int16_t _accumulationFrameNumber{0};
    size_t _prevAccumulationSetting;

    std::unordered_map<std::string, CellObjectMapper> _circuitMappers;
};
#endif
