/* Copyright (c) 2018-2019, EPFL/Blue Brain Project
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

#ifndef DTISimulationHandler_H
#define DTISimulationHandler_H

#include "../api/DTIParams.h"

#include <brayns/api.h>
#include <brayns/common/simulation/AbstractSimulationHandler.h>

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
                         const SpikeSimulationDescriptor& spikeSimulation);

    void* getFrameDataImpl(const uint32_t frame) final;

    brayns::AbstractSimulationHandlerPtr clone() const final;
    std::map<uint64_t, float>& getSpikes() { return _spikes; }
    void setTimeScale(const float scale) { _spikeSimulation.timeScale = scale; }
    void setDecaySpeed(const float value)
    {
        _spikeSimulation.decaySpeed = value;
    }
    void setRestIntensity(const float value)
    {
        _spikeSimulation.restIntensity = value;
    }
    void setSpikeIntensity(const float value)
    {
        _spikeSimulation.spikeIntensity = value;
    }

private:
    std::vector<float> _data;
    std::map<uint64_t, float> _spikes;
    Indices _indices;
    SpikeSimulationDescriptor _spikeSimulation;
};

typedef std::shared_ptr<DTISimulationHandler> DTISimulationHandlerPtr;
} // namespace dti
#endif // DTISimulationHandler_H
