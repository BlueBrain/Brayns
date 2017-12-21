/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#ifndef ANIMATIONPARAMETERS_H
#define ANIMATIONPARAMETERS_H

#include "AbstractParameters.h"

SERIALIZATION_ACCESS(AnimationParameters)

namespace brayns
{
class AnimationParameters final : public AbstractParameters
{
public:
    AnimationParameters();

    /** @copydoc AbstractParameters::print */
    void print() final;

    uint32_t getFrame() const { return _current; }
    void setFrame(uint32_t value)
    {
        _updateValue(_current, adjustCurrent(value));
    }

    /** The (frame) delta to apply for animations to select the next frame. */
    void setDelta(const int32_t delta) { _updateValue(_delta, delta); }
    int32_t getDelta() const { return _delta; }
    void setEnd(const uint32_t end)
    {
        _updateValue(_end, end);
        _updateValue(_current, 0u);
    }
    uint32_t getEnd() const { return _end; }
    void reset()
    {
        _updateValue(_end, std::numeric_limits<uint32_t>::max());
        _updateValue(_current, std::numeric_limits<uint32_t>::max());
    }

private:
    bool _parse(const po::variables_map& vm) final;

    uint32_t adjustCurrent(const uint32_t newCurrent) const
    {
        const auto nbFrames = _end - _start;
        return nbFrames == 0 ? std::numeric_limits<uint32_t>::max()
                             : _start + (newCurrent % nbFrames);
    }

    uint32_t _start{0};
    uint32_t _end{std::numeric_limits<uint32_t>::max()};
    uint32_t _current{std::numeric_limits<uint32_t>::max()};
    int32_t _delta{0};

    SERIALIZATION_FRIEND(AnimationParameters)
};
}
#endif
