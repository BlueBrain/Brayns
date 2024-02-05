/* Copyright 2018-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns
 * <https://github.com/BlueBrain/Brayns-UC-DTI>
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

#include <array>
#include <vector>

#include <brayns/pluginapi/ExtensionPlugin.h>

#include <messages/SetSpikeSimulationFromFileMessage.h>
#include <messages/SetSpikeSimulationMessage.h>

namespace dti
{
/**
 * @brief The DTIPlugin class manages the loading of
 * RAW volumes as point clouds
 */
class DTIPlugin : public brayns::ExtensionPlugin
{
public:
    DTIPlugin();

    void init() final;

    /**
     * @brief preRender Updates the scene according to latest data load
     */
    void preRender() final;

    // Entrypoint access
    void updateSpikeSimulation(const SetSpikeSimulationMessage &);
    void updateSpikeSimulationFromFile(
        const SetSpikeSimulationFromFileMessage &);

private:
    void _updateSpikeSimulation();
    void _updateSimulationFrame();

    SetSpikeSimulationMessage _spikeSimulation;

    bool _simulationDirty{false};
    uint64_t _currentFrame{std::numeric_limits<uint64_t>::max()};
    std::vector<size_t> _registeredModelsForSpikeSimulation;
};
} // namespace dti
