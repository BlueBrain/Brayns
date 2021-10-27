/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#pragma once

#include "AbstractParameters.h"
#include <list>

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

    /** The earliest frame index (computed from the lowest simulation start
     * time) */
    void setStartFrame(const uint32_t startFrame)
    {
        _updateValue(_startFrame, startFrame);
        _updateValue(_numFrames,
                     _startFrame > _endFrame ? 0 : _endFrame - _startFrame,
                     true);
        if (_current >= _numFrames)
            _updateValue(_current, _numFrames > 0 ? _numFrames - 1 : 0, true);
    }

    auto getStartFrame() const noexcept { return _startFrame; }

    void setEndFrame(const uint32_t endFrame)
    {
        _updateValue(_endFrame, endFrame);
        _updateValue(_numFrames,
                     _startFrame > _endFrame ? 0 : _endFrame - _startFrame,
                     true);
        if (_current >= _numFrames)
            _updateValue(_current, _numFrames > 0 ? _numFrames - 1 : 0, true);
    }

    auto getEndFrame() const noexcept { return _endFrame; }

    /** The current frame number of the animation. */
    void setFrame(uint32_t value)
    {
        _updateValue(_current, _adjustedCurrent(value));
    }

    uint32_t getFrame() const noexcept { return _current; }

    /**
     * @brief returns the current simulation frame index taking into account the
     * global simulation start frame index
     */
    uint32_t getAbsoluteFrame() const noexcept
    {
        return _startFrame + _current;
    }

    /** The (frame) delta to apply for animations to select the next frame. */
    void setDelta(const int32_t delta);

    int32_t getDelta() const noexcept { return _delta; }

    void setNumFrames(const uint32_t numFrames,
                      const bool triggerCallback = true)
    {
        _updateValue(_numFrames, numFrames, triggerCallback);
        _updateValue(_current, std::min(_current, _numFrames), triggerCallback);
    }
    uint32_t getNumFrames() const noexcept { return _numFrames; }

    /** The dt of a simulation. */
    void setDt(const double dt, const bool triggerCallback = true)
    {
        _updateValue(_dt, dt, triggerCallback);
    }

    double getDt() const noexcept { return _dt; }

    /** The time unit of a simulation. */
    void setUnit(const std::string& unit, const bool triggerCallback = true)
    {
        _updateValue(_unit, unit, triggerCallback);
    }

    const std::string& getUnit() const { return _unit; }

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

    void togglePlayback() noexcept { _playing = !_playing; }

    void setPlaying(bool playing) { _updateValue(_playing, playing); }

    bool isPlaying() const noexcept { return _playing; }

private:
    uint32_t _adjustedCurrent(const uint32_t newCurrent) const noexcept
    {
        return _numFrames == 0 ? 0 : newCurrent % _numFrames;
    }

    bool _canUpdateFrame() const;

    uint32_t _startFrame{0};
    uint32_t _endFrame{0};
    uint32_t _numFrames{0};
    uint32_t _current{0};
    int32_t _delta{1};
    bool _playing{false};
    double _dt{0};
    std::string _unit;

    IsReadyCallback _isReadyCallback;
};
} // namespace brayns
