/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "AbstractParameters.h"

namespace brayns
{
class SimulationParameters : public AbstractParameters
{
public:
    SimulationParameters();

    /**
     * @brief print implementation to print this parameters
     */
    void print() final;

    /**
     * @brief reset resets the parameters to no simulation state:
     * - dt = 0.0
     * - start frame = 0
     * - end frame = 0
     */
    void reset();

    /**
     * @brief setStartFrame sets the first absolute frame
     * numbers of the loaded simulations. It is computed
     * from the loaded simulations, using the simulation with the earliest
     * start time.
     * @param startFrame uint32_t first absolute frame
     */
    void setStartFrame(const uint32_t startFrame) noexcept;

    /**
     * @brief setEndFrame sets the last absolute frame
     * numbers of the loaded simulations. It is computed
     * from the loaded simulations, using the simulation with the latest
     * end time.
     * @param startFrame uint32_t first absolute frame
     */
    void setEndFrame(const uint32_t endFrame) noexcept;

    /**
     * @brief getStartFrame returns the first absolute frame number
     * @return uint32_t
     */
    uint32_t getStartFrame() const noexcept;

    /**
     * @brief getEndFrame returns the last absolute frame number
     * @return uint32_t
     */
    uint32_t getEndFrame() const noexcept;

    /**
     * @brief sets the current frame.
     * @param value uint32_t
     */
    void setFrame(const uint32_t value) noexcept;

    /**
     * @brief getFrame returns the current frame.
     * @return uint32_t
     */
    uint32_t getFrame() const noexcept;

    /**
     * @brief setDt sets the simulation timestep (in milliseconds)
     * @param dt double timestep in milliseconds
     */
    void setDt(const double dt) noexcept;

    /**
     * @brief getDt returns the simulation timestep (in milliseconds)
     * @return double
     */
    double getDt() const noexcept;

    /**
     * @brief getTimeUnit returns a string representing the time unit in which
     * the simulation runs. It is always millisecconds ("ms")
     * @return const std::string&
     */
    const std::string &getTimeUnit() const noexcept;

private:
    uint32_t _startFrame{0};
    uint32_t _endFrame{0};
    uint32_t _current{0};
    double _dt{0};
    const std::string _unit = "ms";
};
} // namespace brayns
