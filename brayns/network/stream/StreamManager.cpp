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
class ImageStreamPolicy
{
public:
    static bool needsBroadcast(brayns::FrameBuffer &framebuffer, brayns::ClientManager &clients)
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

class ImageStreamer
{
public:
    ImageStreamer(const brayns::ApplicationParameters &parameters)
        : _parameters(parameters)
    {
    }

    void broadcast(brayns::FrameBuffer &framebuffer, brayns::ClientManager &clients) const
    {
        auto compression = _parameters.getJpegCompression();
        auto image = framebuffer.getImage();
        auto data = brayns::ImageEncoder::encode(image, "jpg", compression);
        clients.broadcast(data);
    }

private:
    const brayns::ApplicationParameters &_parameters;
};

class ControlledImageStreamer
{
public:
    ControlledImageStreamer(const brayns::ApplicationParameters &parameters, brayns::ImageStreamMonitor &monitor)
        : _streamer(parameters)
        , _monitor(monitor)
    {
    }

    void broadcast(brayns::FrameBuffer &framebuffer, brayns::ClientManager &clients) const
    {
        if (!_monitor.isTriggered())
        {
            return;
        }
        _streamer.broadcast(framebuffer, clients);
        _monitor.resetTrigger();
    }

private:
    ImageStreamer _streamer;
    brayns::ImageStreamMonitor &_monitor;
};

class AutoImageStreamer
{
public:
    AutoImageStreamer(const brayns::ApplicationParameters &parameters, brayns::ImageStreamMonitor &monitor)
        : _parameters(parameters)
        , _streamer(parameters)
        , _monitor(monitor)
    {
    }

    void broadcast(brayns::FrameBuffer &framebuffer, brayns::ClientManager &clients) const
    {
        auto fps = _parameters.getImageStreamFPS();
        _monitor.callWithFpsLimit(fps, [&] { _streamer.broadcast(framebuffer, clients); });
    }

private:
    const brayns::ApplicationParameters &_parameters;
    ImageStreamer _streamer;
    brayns::ImageStreamMonitor &_monitor;
};

class StreamDispatcher
{
public:
    StreamDispatcher(const brayns::ApplicationParameters &parameters, brayns::ImageStreamMonitor &monitor)
        : _controlledStreamer(parameters, monitor)
        , _autoStreamer(parameters, monitor)
        , _monitor(monitor)
    {
    }

    void broadcast(brayns::FrameBuffer &framebuffer, brayns::ClientManager &clients) const
    {
        if (!ImageStreamPolicy::needsBroadcast(framebuffer, clients))
        {
            return;
        }
        if (_monitor.isControlled())
        {
            _controlledStreamer.broadcast(framebuffer, clients);
            return;
        }
        _autoStreamer.broadcast(framebuffer, clients);
    }

private:
    ControlledImageStreamer _controlledStreamer;
    AutoImageStreamer _autoStreamer;
    brayns::ImageStreamMonitor _monitor;
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

void StreamManager::setParameters(const ApplicationParameters &parameters)
{
    _parameters = &parameters;
}

void StreamManager::broadcast(FrameBuffer &framebuffer, ClientManager &clients)
{
    StreamDispatcher dispatcher(*_parameters, _monitor);
    dispatcher.broadcast(framebuffer, clients);
}

ImageStreamMonitor &StreamManager::getMonitor()
{
    return _monitor;
}
} // namespace brayns
