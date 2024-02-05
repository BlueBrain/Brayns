/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/network/common/RateLimiter.h>

#include <memory>

namespace brayns
{
class NetworkContext;

/**
 * @brief Info to monitor the image stream.
 *
 */
class ImageStreamMonitor
{
public:
    /**
     * @brief Set the max FPS of the stream.
     *
     * @param fps
     */
    void setFps(size_t fps) { _limiter = RateLimiter::fromFps(fps); }

    /**
     * @brief Check if the stream is controlled.
     *
     * If true, the client trigger the stream of each image required.
     *
     * @return true Controlled.
     * @return false Automatic.
     */
    bool isControlled() const { return _controlled; }

    /**
     * @brief Set the stream control mode.
     *
     * @param controlled True if controlled.
     */
    void setControlled(bool controlled)
    {
        _controlled = controlled;
        _triggered = false;
    }

    /**
     * @brief Check if the image stream has been triggered.
     *
     * @return true Client triggered the image stream.
     * @return false Nothing triggered.
     */
    bool isTriggered() const { return _triggered; }

    /**
     * @brief Trigger the image stream in controlled mode.
     *
     */
    void trigger() { _triggered = true; }

    /**
     * @brief Reset the trigger in control mode.
     *
     */
    void resetTrigger() { _triggered = false; }

    /**
     * @brief Call the given functor with maximum FPS rate limit.
     *
     * @tparam FunctorType Functor type.
     * @param functor Functor like void().
     */
    template <typename FunctorType>
    void callWithFpsLimit(FunctorType functor)
    {
        _limiter.call(std::move(functor));
    }

private:
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
     * @brief Construct a stream manager with NetworkContext access.
     *
     * @param context Context of the network manager.
     */
    StreamManager(NetworkContext& context);

    /**
     * @brief Broadcast images according to current settings.
     *
     */
    void broadcast();

    /**
     * @brief Get the image stream monitor.
     *
     * @return ImageStreamMonitor& Image stream monitor.
     */
    ImageStreamMonitor& getMonitor() { return _imageStream; }

private:
    NetworkContext* _context;
    ImageStreamMonitor _imageStream;
};
} // namespace brayns