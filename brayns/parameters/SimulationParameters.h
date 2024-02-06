/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/utils/ModifiedFlag.h>

namespace brayns
{
class SimulationParameters : public AbstractParameters
{
public:
    /**
     * @brief reset resets the parameters to no simulation state:
     * - dt = 0.0
     * - start frame = 0
     * - end frame = 0
     */
    void reset();

    /**
     * @brief Sets the first absolute frame
     * numbers of the loaded simulations. It is computed
     * from the loaded simulations, using the simulation with the earliest
     * start time.
     *
     * @param startFrame uint32_t First absolute frame.
     */
    void setStartFrame(const uint32_t startFrame) noexcept;

    /**
     * @brief Sets the last absolute frame
     * numbers of the loaded simulations. It is computed
     * from the loaded simulations, using the simulation with the latest
     * end time.
     *
     * @param startFrame uint32_t First absolute frame.
     */
    void setEndFrame(const uint32_t endFrame) noexcept;

    /**
     * @brief Returns the first absolute frame number.
     *
     * @return uint32_t Start frame index.
     */
    uint32_t getStartFrame() const noexcept;

    /**
     * @brief getEndFrame returns the last absolute frame number.
     *
     * @return uint32_t End frame index.
     */
    uint32_t getEndFrame() const noexcept;

    /**
     * @brief Sets the current frame.
     *
     * @param value uint32_t Current frame index.
     */
    void setFrame(const uint32_t value) noexcept;

    /**
     * @brief Returns the current frame.
     *
     * @return uint32_t Current frame index.
     */
    uint32_t getFrame() const noexcept;

    /**
     * @brief Sets the simulation timestep (in milliseconds).
     *
     * @param dt double timestep in milliseconds.
     */
    void setDt(const double dt) noexcept;

    /**
     * @brief Returns the simulation timestep (in milliseconds).
     *
     * @return double Timestep.
     */
    double getDt() const noexcept;

    /**
     * @brief Return "ms".
     *
     * @return const std::string& Time unit used as string.
     */
    const std::string &getTimeUnit() const noexcept;

    /**
     * @brief Register argv properties of the parameter set.
     *
     * @param builder Helper class to register argv properties.
     */
    virtual void build(ArgvBuilder &builder) override;

    /**
     * @brief Resets the modified status of the simulation parameters
     */
    void resetModified() override;

    /**
     * @brief Returns wether the parameters were modified since the last call to resetModified()
     *
     * @return true if no changes have happened since the last call to resetModified()
     * @return false if anything changed since the last call to resetModified()
     */
    bool isModified() const noexcept;

private:
    uint32_t _startFrame{0};
    uint32_t _endFrame{0};
    uint32_t _current{0};
    double _dt{0.0};
    const std::string _unit = "ms";
    ModifiedFlag _flag;
};
} // namespace brayns
