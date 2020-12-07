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

    template<class T,
             typename = std::enable_if_t<std::is_base_of<CellObjectMapper, T>::value>
            >
    void addCircuitMapper(T&& mapper)
    {
        _mappers.emplace_back(std::make_unique<T>(std::forward<T>(mapper)));
    }
    void releaseCircuitMapper(const size_t modelId);

private:
    CellObjectMapper* getMapperForCircuit(const size_t modelId) noexcept
    {
        for(auto& mapper : _mappers)
        {
            if(mapper->getSourceModelId() == modelId)
                return mapper.get();
        }

        return nullptr;
    }

    // Rendering
    brayns::Message _setCamera(const CameraDefinition&);
    CameraDefinition _getCamera();
    brayns::Message _setMaterial(const MaterialDescriptor&);
    brayns::Message _setMaterials(const MaterialsDescriptor&);
    brayns::Message _setMaterialRange(const MaterialRangeDescriptor&);
    brayns::Message _setMaterialExtraAttributes(const MaterialExtraAttributes&);
    MaterialProperties _getMaterialProperties();
    brayns::Message _updateMaterialProperties(const UpdateMaterialProperties&);

    // Experimental
    brayns::Message _setSynapseAttributes(const SynapseAttributes&);
    brayns::Message _setConnectionsPerValue(const ConnectionsPerValue&);
    brayns::Message _setMetaballsPerSimulationValue(const MetaballsFromSimulationValue&);
    brayns::Message _saveModelToCache(const SaveModelToCache&);

    // Handlers
    brayns::Message _attachCellGrowthHandler(const AttachCellGrowthHandler& payload);
    brayns::Message _attachCircuitSimulationHandler(
        const AttachCircuitSimulationHandler& payload);

    // Movie production
    brayns::Message _exportFramesToDisk(const ExportFramesToDisk& payload);
    void _doExportFrameToDisk();
    FrameExportProgress _getFrameExportProgress();
    ExportLayerToDiskResult _exportLayerToDisk(const ExportLayerToDisk& payload);
    brayns::Message _makeMovie(const MakeMovieParameters& params);

    // Anterograde tracing
    brayns::Message _traceAnterogrades(const AnterogradeTracing& payload);

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
    brayns::Message _addGrid(const AddGrid& payload);
    brayns::Message _addColumn(const AddColumn& payload);

    // Get material information
    MaterialIds _getMaterialIds(const ModelId& modelId);
    MaterialDescriptor _getMaterial(const ModelMaterialId& mmId);

    // Remap circuit colors to a specific scheme
    brayns::Message _remapCircuitToScheme(const RemapCircuit& payload);
    brayns::Message _colorCells(const ColorCells& payload);

    brayns::Message _mirrorModel(const MirrorModel& payload);

    SynapseAttributes _synapseAttributes;

    bool _dirty{false};

    ExportFramesToDisk _exportFramesToDiskPayload;
    bool _exportFramesToDiskDirty{false};
    // Flag used to avoid the first frame to be rendered with the wrong camera parameters
    bool _exportFramesToDiskStartFlag{false};
    uint16_t _frameNumber{0};
    uint32_t _accumulationFrameNumber{0};
    size_t _prevAccumulationSetting;

    std::vector<std::unique_ptr<CellObjectMapper>> _mappers;
};
#endif
