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

#include <brayns/common/simulation/AbstractSimulationHandler.h>

#include <messages/SetSpikeSimulationMessage.h>

namespace dti
{
using Positions = std::vector<brayns::Vector3d>;
using Indices = std::vector<uint64_t>;

/**
 * @brief The DTISimulationHandler class handles simulation frames for DTI
 * connectivity
 */
class DTISimulationHandler : public brayns::AbstractSimulationHandler
{
public:
    /**
     * @brief Default constructor
     */
    DTISimulationHandler(const Indices& indices,
                         const SetSpikeSimulationMessage& spikeSimulation);

    std::vector<float> getFrameDataImpl(const uint32_t frame) final;

    brayns::AbstractSimulationHandlerPtr clone() const final;
    std::map<uint64_t, float>& getSpikes() { return _spikes; }
    void setTimeScale(const float scale)
    {
        _spikeSimulation.time_scale = scale;
    }
    void setDecaySpeed(const float value)
    {
        _spikeSimulation.decay_speed = value;
    }
    void setRestIntensity(const float value)
    {
        _spikeSimulation.rest_intensity = value;
    }
    void setSpikeIntensity(const float value)
    {
        _spikeSimulation.spike_intensity = value;
    }

private:
    std::map<uint64_t, float> _spikes;
    Indices _indices;
    SetSpikeSimulationMessage _spikeSimulation;
};

typedef std::shared_ptr<DTISimulationHandler> DTISimulationHandlerPtr;
} // namespace dti
