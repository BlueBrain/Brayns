/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <utility>

#include <brayns/engine/FrameBuffer.h>

#include <brayns/network/client/ClientManager.h>
#include <brayns/network/common/RateLimiter.h>

#include <brayns/parameters/ApplicationParameters.h>

namespace brayns
{
/**
 * @brief Info to monitor the image stream.
 *
 */
class ImageStreamMonitor
{
public:
    /**
     * @brief Check if the stream is controlled.
     *
     * If true, the client trigger the stream of each image required.
     *
     * @return true Controlled.
     * @return false Automatic.
     */
    bool isControlled() const;

    /**
     * @brief Set the stream control mode.
     *
     * @param controlled True if controlled.
     */
    void setControlled(bool controlled);

    /**
     * @brief Check if the image stream has been triggered.
     *
     * @return true Client triggered the image stream.
     * @return false Nothing triggered.
     */
    bool isTriggered() const;

    /**
     * @brief Trigger the image stream in controlled mode.
     *
     */
    void trigger();

    /**
     * @brief Reset the trigger in control mode.
     *
     */
    void resetTrigger();

    /**
     * @brief Call the given functor with maximum FPS rate limit.
     *
     * @tparam FunctorType Functor type.
     * @param functor Functor like void().
     */
    template<typename FunctorType>
    void callWithFpsLimit(size_t fps, FunctorType functor)
    {
        if (fps != _fps)
        {
            _fps = fps;
            _limiter = RateLimiter::fromFps(fps);
        }
        _limiter.call(std::move(functor));
    }

private:
    size_t _fps = 0;
    RateLimiter _limiter;
    bool _controlled = false;
    bool _triggered = false;
};

/**
 * @brief Stream manager to monitor image streaming.
 *
 */
class StreamManager
{
public:
    /**
     * @brief Load application parameters (jpeg compression, FPS).
     *
     * @param parameters Application parameters to load.
     */
    void setParameters(const ApplicationParameters &parameters);

    /**
     * @brief Broadcast images according to current settings.
     *
     */
    void broadcast(FrameBuffer &framebuffer, ClientManager &clients);

    /**
     * @brief Get the image stream monitor.
     *
     * @return ImageStreamMonitor& Image stream monitor.
     */
    ImageStreamMonitor &getMonitor();

private:
    const ApplicationParameters *_parameters = nullptr;
    ImageStreamMonitor _monitor;
};
} // namespace brayns
