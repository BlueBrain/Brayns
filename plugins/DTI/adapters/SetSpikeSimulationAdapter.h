/* Copyright (c) 2021 EPFL/Blue Brain Project
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#pragma once

#include <brayns/network/json/Message.h>

#include <api/DTIParams.h>

namespace brayns
{
BRAYNS_ADAPTER_BEGIN(SpikeSimulationDescriptor)
BRAYNS_ADAPTER_NAMED_ENTRY("model_id", modelId, "The ID of the loaded model")
BRAYNS_ADAPTER_NAMED_ENTRY("gids", gids, "List of cel GIDs")
BRAYNS_ADAPTER_NAMED_ENTRY("timestamps", timestamps, "List of spike timestamps")
BRAYNS_ADAPTER_NAMED_ENTRY("dt", dt, "Simulation time step")
BRAYNS_ADAPTER_NAMED_ENTRY("end_time", endTime, "Simulation end time")
BRAYNS_ADAPTER_NAMED_ENTRY("time_scale", timeScale, "Time scale")
BRAYNS_ADAPTER_NAMED_ENTRY("decay_speed", decaySpeed, "Speed of spike decay")
BRAYNS_ADAPTER_NAMED_ENTRY("rest_intensity", restIntensity, "Rest intensity")
BRAYNS_ADAPTER_NAMED_ENTRY("spike_intensity", spikeIntensity, "Spike intensity")
BRAYNS_ADAPTER_END()
} // namespace brayns