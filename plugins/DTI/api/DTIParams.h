/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#ifndef DTIPARAMS_H
#define DTIPARAMS_H

#include "../io/DTITypes.h"

#include <brayns/common/types.h>
#include <brayns/network/json/ActionMessage.h>

/**
 * @brief The Streamlines struct handles a set of streamlines. Indices are used
 * to specify the first point of each streamline
 */
struct StreamlinesDescriptor : public brayns::Message
{
    MESSAGE_BEGIN(StreamlinesDescriptor)
    MESSAGE_ENTRY(std::string, name, "Model name")
    MESSAGE_ENTRY(std::vector<uint64_t>, gids, "List of cell GIDs")
    MESSAGE_ENTRY(std::vector<uint64_t>, indices, "List of indices")
    MESSAGE_ENTRY(std::vector<float>, vertices, "List of vertices (3 components per vertex)")
    MESSAGE_ENTRY(double, radius, "Streamline tube radius")
    MESSAGE_ENTRY(double, opacity, "Color opacity")
    MESSAGE_ENTRY(int32_t, colorScheme, "Color scheme to draw the streamlines")
};

struct SpikeSimulationDescriptor : public brayns::Message
{
    MESSAGE_BEGIN(SpikeSimulationDescriptor)
    MESSAGE_ENTRY(uint64_t, modelId, "The ID of the loaded model")
    MESSAGE_ENTRY(std::vector<uint64_t>, gids, "List of cel GIDs")
    MESSAGE_ENTRY(std::vector<float>, timestamps, "List of spike timestamps")
    MESSAGE_ENTRY(double, dt, "Simulation time step")
    MESSAGE_ENTRY(double, endTime, "Simulation normalized end time")
    MESSAGE_ENTRY(double, timeScale, "Time scale")
    MESSAGE_ENTRY(double, decaySpeed, "Speed of spike decay")
    MESSAGE_ENTRY(double, restIntensity, "Rest intensity")
    MESSAGE_ENTRY(double, spikeIntensity, "Spike intensity")
};

struct SpikeSimulationFromFile : public brayns::Message
{
    MESSAGE_BEGIN(SpikeSimulationFromFile)
    MESSAGE_ENTRY(uint64_t, modelId, "The ID of the loaded model")
    MESSAGE_ENTRY(double, dt, "Simulation time step")
    MESSAGE_ENTRY(double, timeScale, "Simulation time scale")
    MESSAGE_ENTRY(double, decaySpeed, "Speed of spike decay")
    MESSAGE_ENTRY(double, restIntensity, "Rest intensity")
    MESSAGE_ENTRY(double, spikeIntensity, "Spike intensity")
    MESSAGE_ENTRY(std::string, path, "Path to BlueConfig file")
};

#endif // DTIPARAMS_H
