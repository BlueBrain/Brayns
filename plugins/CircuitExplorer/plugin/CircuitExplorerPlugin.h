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

#pragma once

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

    CellObjectMapper* getMapperForCircuit(const size_t modelId) noexcept
    {
        for (auto& mapper : _mappers)
        {
            if (mapper->getSourceModelId() == modelId)
                return mapper.get();
        }

        return nullptr;
    }

    template <class T, typename = std::enable_if_t<
                           std::is_base_of<CellObjectMapper, T>::value>>
    void addCircuitMapper(T&& mapper)
    {
        _mappers.emplace_back(std::make_unique<T>(std::forward<T>(mapper)));
    }

    void releaseCircuitMapper(const size_t modelId);

    // Used by entrypoints
    SynapseAttributes& getSynapseAttributes() { return _synapseAttributes; }
    brayns::Message exportFramesToDisk(const ExportFramesToDisk& payload);
    FrameExportProgress getFrameExportProgress();

private:
    brayns::Message _setCamera(const CameraDefinition&);
    void _doExportFrameToDisk();

    SynapseAttributes _synapseAttributes;

    bool _dirty{false};

    ExportFramesToDisk _exportFramesToDiskPayload;
    bool _exportFramesToDiskDirty{false};
    // Flag used to avoid the first frame to be rendered with the wrong camera
    // parameters
    bool _exportFramesToDiskStartFlag{false};
    uint16_t _frameNumber{0};
    uint32_t _accumulationFrameNumber{0};
    size_t _prevAccumulationSetting;
    bool _exportFrameError{false};
    std::string _exportFrameErrorMessage;

    std::vector<std::unique_ptr<CellObjectMapper>> _mappers;
};
