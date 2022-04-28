/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/engine/ModelComponents.h>

namespace brayns
{
class SimulationComponent final : public brayns::Component
{
public:
    /**
     * @brief Creates the simulation component for the given simulation information
     * @param start start time of the simulation
     * @param end end time of the simulation
     * @param dt timestep of the simulation
     * @param timeUnit unit in which the specified time is
     */
    SimulationComponent(float start, float end, float dt, std::string timeUnit);

    /**
     * @brief Returns the start time of the simulation
     * @return float
     */
    float getStartTime() const noexcept;

    /**
     * @brief Returns the end time of the simulation
     * @return float
     */
    float getEndTime() const noexcept;

    /**
     * @brief Returns the simulation timestep
     * @return float
     */
    float getDT() const noexcept;

    /**
     * @brief Returns the time unit of the time parameters of this component
     * @return const std::string &
     */
    const std::string &getTimeUnit() const noexcept;

    /**
     * @brief Returns wether the simulation of the model is enabled or not
     * @return bool
     */
    bool enabled() const noexcept;

    /**
     * @brief Sets wether the simulation of the model is enabled or not
     * @param val
     */
    void setEnabled(const bool val) noexcept;

private:
    const float _startTime{};
    const float _endTime{};
    const float _dt{};
    const std::string _timeUnit;
    bool _enabled{true};
};
}
