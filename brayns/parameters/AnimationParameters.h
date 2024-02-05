/* Copyright 2015-2024 Blue Brain Project/EPFL
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
class AnimationParameters : public AbstractParameters
{
public:
    AnimationParameters();

    /**
     * @brief print implementation to print this parameters
     */
    void print() final;

    /**
     * @brief reset resets the parameters to no animation state:
     * - dt = 0.0
     * - start frame = 0
     * - end frame = 0
     */
    void reset();

    /**
     * @brief setStartAndEndFrame sets the first and last absolute frame
     * numbers of the loaded simulations. These frame numbers are computed
     * from the loaded simulations, using the simulation with the earliest
     * starting time and the simulation with the latest end time
     * @param startFrame uint32_t first absolute frame
     * @param endFrame uint32_t last absolute frame
     */
    void setStartAndEndFrame(const uint32_t startFrame,
                             const uint32_t endFrame) noexcept;

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
     * @brief setFrame sets the current normalized frame. A normalized frame
     * number is in the interval [ 0, getEndFrame() - getStartFrame() ). If
     * the provided value is outside the interval, it will be clampped
     * @param value uint32_t normalized frame to set
     */
    void setFrame(const uint32_t value) noexcept;

    /**
     * @brief getFrame returns the current normalized frame. A normalized frame
     * number is in the interval [ 0, getEndFrame() - getStartFrame() )
     * @return uint32_t
     */
    uint32_t getFrame() const noexcept;

    /**
     * @brief getAbsoluteFrame returns the current absolute frame. An absolute
     * frame is calculated as getStartFrame() + getFrame(). It is in the
     * interval [ getStartFrame(), getEndFrame() )
     * @return uint32_t
     */
    uint32_t getAbsoluteFrame() const noexcept;

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
    const std::string& getTimeUnit() const noexcept;

private:
    uint32_t _startFrame{0};
    uint32_t _endFrame{0};
    uint32_t _current{0};
    double _dt{0};
    const std::string _unit = "ms";
};
} // namespace brayns
