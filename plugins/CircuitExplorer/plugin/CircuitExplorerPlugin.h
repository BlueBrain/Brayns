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

private:
    // Rendering
    void _setCamera(const CameraDefinition&);
    CameraDefinition _getCamera();
    void _setMaterial(const MaterialDescriptor&);
    void _setMaterials(const MaterialsDescriptor&);
    void _setMaterialExtraAttributes(const MaterialExtraAttributes&);

    // Experimental
    void _setSynapseAttributes(const SynapseAttributes&);
    void _setConnectionsPerValue(const ConnectionsPerValue&);
    void _setMetaballsPerSimulationValue(const MetaballsFromSimulationValue&);
    void _saveModelToCache(const SaveModelToCache&);

    // Handlers
    void _attachCellGrowthHandler(const AttachCellGrowthHandler& payload);
    void _attachCircuitSimulationHandler(
        const AttachCircuitSimulationHandler& payload);

    // Movie production
    void _exportFramesToDisk(const ExportFramesToDisk& payload);
    void _doExportFrameToDisk();
    FrameExportProgress _getFrameExportProgress();
    void _makeMovie(const MakeMovieParameters& params);

    // Predefined models
    void _addGrid(const AddGrid& payload);
    void _addColumn(const AddColumn& payload);

    MaterialIds _getMaterialIds(const ModelId& modelId);
    SynapseAttributes _synapseAttributes;

    bool _dirty{false};

    ExportFramesToDisk _exportFramesToDiskPayload;
    bool _exportFramesToDiskDirty{false};
    uint16_t _frameNumber{0};
    int16_t _accumulationFrameNumber{0};
};
#endif
