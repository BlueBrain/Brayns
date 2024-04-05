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

#include "SimulationParameters.h"

namespace brayns
{
void SimulationParameters::reset()
{
    setFrame(0);
    setDt(0.);
    setStartFrame(0);
    setEndFrame(0);
}

void SimulationParameters::setStartFrame(const uint32_t startFrame) noexcept
{
    _flag.update(_startFrame, startFrame);
}

void SimulationParameters::setEndFrame(const uint32_t endFrame) noexcept
{
    _flag.update(_endFrame, endFrame);
}

uint32_t SimulationParameters::getStartFrame() const noexcept
{
    return _startFrame;
}

uint32_t SimulationParameters::getEndFrame() const noexcept
{
    return _endFrame;
}

void SimulationParameters::setFrame(const uint32_t value) noexcept
{
    _flag.update(_current, value);
}

uint32_t SimulationParameters::getFrame() const noexcept
{
    return _current;
}

void SimulationParameters::setDt(const double dt) noexcept
{
    _flag.update(_dt, dt);
}

double SimulationParameters::getDt() const noexcept
{
    return _dt;
}

const std::string &SimulationParameters::getTimeUnit() const noexcept
{
    return _unit;
}

void SimulationParameters::build(ArgvBuilder &builder)
{
    (void)builder;
}

void SimulationParameters::resetModified()
{
    _flag.setModified(false);
}

bool SimulationParameters::isModified() const noexcept
{
    return static_cast<bool>(_flag);
}
} // namespace brayns
