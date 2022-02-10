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

#include "StreamManager.h"

#include <brayns/common/Log.h>

#include <brayns/utils/image/ImageEncoder.h>

namespace
{
class ImageStreamHelper
{
public:
    static bool hasClientsAndNewImage(brayns::ClientManager &clients, brayns::FrameBuffer &framebuffer)
    {
        if (clients.isEmpty())
        {
            return false;
        }
        if (framebuffer.getFrameBufferFormat() == brayns::PixelFormat::NONE)
        {
            return false;
        }
        return framebuffer.isModified();
    }
};

class ImageStream
{
public:
    static void broadcast(
        brayns::FrameBuffer &framebuffer,
        brayns::ClientManager &clients,
        const brayns::ApplicationParameters &parameters)
    {
        auto compression = parameters.getJpegCompression();
        auto image = framebuffer.getImage();
        auto data = brayns::ImageEncoder::encode(image, "jpg", compression);
        clients.broadcast(data);
    }
};

class ControlledImageStream
{
public:
    ControlledImageStream(brayns::ImageStreamMonitor &monitor)
        : _monitor(monitor)
    {
    }

    void broadcast(
        brayns::FrameBuffer &framebuffer,
        brayns::ClientManager &clients,
        const brayns::ApplicationParameters &parameters) const
    {
        if (!_monitor.isTriggered())
        {
            return;
        }
        ImageStream::broadcast(framebuffer, clients, parameters);
        _monitor.resetTrigger();
    }

private:
    brayns::ImageStreamMonitor &_monitor;
};

class AutoImageStream
{
public:
    AutoImageStream(brayns::RateLimiter &limiter)
        : _limiter(limiter)
    {
    }

    void broadcast(
        brayns::FrameBuffer &framebuffer,
        brayns::ClientManager &clients,
        const brayns::ApplicationParameters &parameters) const
    {
        auto fps = parameters.getImageStreamFPS();
        _limiter.setRate(fps);
        _limiter.call([&] { ImageStream::broadcast(framebuffer, clients, parameters); });
    }

private:
    brayns::RateLimiter &_limiter;
};

class StreamDispatcher
{
public:
    StreamDispatcher(brayns::ImageStreamMonitor &monitor, brayns::RateLimiter &limiter)
        : _controlledStream(monitor)
        , _autoStream(limiter)
        , _monitor(monitor)
    {
    }

    void broadcast(
        brayns::FrameBuffer &framebuffer,
        brayns::ClientManager &clients,
        const brayns::ApplicationParameters &parameters) const
    {
        if (!ImageStreamHelper::hasClientsAndNewImage(clients, framebuffer))
        {
            return;
        }
        if (_monitor.isControlled())
        {
            _controlledStream.broadcast(framebuffer, clients, parameters);
            return;
        }
        _autoStream.broadcast(framebuffer, clients, parameters);
    }

private:
    ControlledImageStream _controlledStream;
    AutoImageStream _autoStream;
    brayns::ImageStreamMonitor &_monitor;
};
} // namespace

namespace brayns
{
bool ImageStreamMonitor::isControlled() const
{
    return _controlled;
}

void ImageStreamMonitor::setControlled(bool controlled)
{
    _controlled = controlled;
    _triggered = false;
}

bool ImageStreamMonitor::isTriggered() const
{
    return _triggered;
}

void ImageStreamMonitor::trigger()
{
    _triggered = true;
}

void ImageStreamMonitor::resetTrigger()
{
    _triggered = false;
}

void StreamManager::broadcast(FrameBuffer &framebuffer, ClientManager &clients, const ApplicationParameters &parameters)
{
    StreamDispatcher dispatcher(_monitor, _limiter);
    dispatcher.broadcast(framebuffer, clients, parameters);
}

ImageStreamMonitor &StreamManager::getMonitor()
{
    return _monitor;
}
} // namespace brayns
