/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/common/Timer.h>

namespace brayns
{
class FPSCounter
{
public:
    /**
     * @brief startFrame starts measuring time
     */
    void startFrame() noexcept;

    /**
     * @brief endFrame stops time measuring and accumulates the result
     */
    void endFrame() noexcept;

    /**
     * @brief getAverageFPS returns the current average frames per second measurement made by this counter
     */
    uint32_t getAverageFPS() const noexcept;

    /**
     * @brief getLastFPS return the FPS ratio measured by the last take
     */
    uint32_t getLastFPS() const noexcept;

    /**
     * @brief getLastFrameTimeMillis return the time, in milliseconds, that it took to render the last frame
     */
    int64_t getLastFrameTimeMillis() const noexcept;

private:
    Timer _timer;
    int64_t _lastFrameTimeMillis {0u};
    uint32_t _lastFPS {0u};
    double _smoothFPS {0};
};
}
