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
#include <list>

SERIALIZATION_ACCESS(AnimationParameters)

namespace brayns
{
class AnimationParameters : public AbstractParameters
{
public:
    AnimationParameters();

    /** @copydoc AbstractParameters::print */
    void print() final;

    /** Reset to a 'no animation' state: 0 for dt, start and end. */
    void reset();

    /** The current frame number of the animation. */
    void setFrame(uint32_t value)
    {
        _updateValue(_current, _adjustedCurrent(value));
    }
    uint32_t getFrame() const { return _current; }
    /** The (frame) delta to apply for animations to select the next frame. */
    void setDelta(const int32_t delta) { _updateValue(_delta, delta); }
    int32_t getDelta() const { return _delta; }
    void setStart(const uint32_t start)
    {
        _updateValue(_start, start);
        _updateValue(_current, std::max(_current, _start));
    }
    uint32_t getStart() const { return _start; }
    void setEnd(const uint32_t end)
    {
        _updateValue(_end, end);
        _updateValue(_current, std::min(_current, _end));
    }
    uint32_t getEnd() const { return _end; }
    /** The dt of a simulation. */
    void setDt(const double dt) { _updateValue(_dt, dt); }
    double getDt() const { return _dt; }
    /** The time unit of a simulation. */
    void setUnit(const std::string& unit) { _updateValue(_unit, unit); }
    using IsReadyCallback = std::function<bool()>;

    /**
     * Set a callback to report if the current animation frame is ready
     * (e.g. simulation has been loaded) and the animation can advance to the
     * next frame.
     */
    void setIsReadyCallback(const IsReadyCallback& callback)
    {
        _isReadyCallback = callback;
    }

    /** Remove the given callback from the list of IsReadyCallbacks. */
    void removeIsReadyCallback()
    {
        if (_isReadyCallback)
        {
            reset();
            _isReadyCallback = nullptr;
        }
    }

    bool hasIsReadyCallback() const { return !!_isReadyCallback; }
    /** Update the current frame if delta is set and all listeners are ready. */
    void update();

    /** Jump 'frames' from current frame if all listeners are ready. */
    void jumpFrames(int frames);

private:
    void parse(const po::variables_map& vm) final;

    uint32_t _adjustedCurrent(const uint32_t newCurrent) const
    {
        const auto nbFrames = _end - _start;
        return nbFrames == 0 ? 0 : _start + (newCurrent % nbFrames);
    }

    bool _canUpdateFrame() const;

    uint32_t _start{0};
    uint32_t _end{0};
    uint32_t _current{0};
    int32_t _delta{0};
    double _dt{0};
    std::string _unit;

    IsReadyCallback _isReadyCallback;

    SERIALIZATION_FRIEND(AnimationParameters)
};
}
#endif
