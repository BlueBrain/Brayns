/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#include <brayns/network/context/NetworkContext.h>

namespace
{
using namespace brayns;

class ImageStream
{
public:
    static void broadcast(NetworkContext& context)
    {
        auto& api = context.getApi();
        auto& engine = api.getEngine();
        auto& framebuffer = engine.getFrameBuffer();
        auto& manager = api.getParametersManager();
        auto& parameters = manager.getApplicationParameters();
        auto quality = parameters.getJpegCompression();
        auto& generator = context.getImageGenerator();
        framebuffer.map();
        auto colorBuffer = framebuffer.getColorBuffer();
        auto format = framebuffer.getFrameBufferFormat();
        auto size = framebuffer.getSize();
        auto image = generator.createJPEG(colorBuffer, format, size, quality);
        framebuffer.unmap();
        if (image.size == 0)
        {
            return;
        }
        _trySendImage(context, image);
    }

private:
    static void _trySendImage(NetworkContext& context,
                              const ImageGenerator::ImageJPEG& image)
    {
        auto& connections = context.getConnections();
        try
        {
            connections.broadcast({image.data.get(), int(image.size)});
        }
        catch (const ConnectionClosedException& e)
        {
            Log::debug("Connection closed during image broadcast: {}.",
                       e.what());
        }
    }
};

class ControlledImageStream
{
public:
    static void broadcast(NetworkContext& context)
    {
        if (!_isTriggered(context))
        {
            return;
        }
        ImageStream::broadcast(context);
        _resetTrigger(context);
    }

private:
    static bool _isTriggered(NetworkContext& context)
    {
        auto& stream = context.getStream();
        auto& monitor = stream.getMonitor();
        return monitor.isTriggered();
    }

    static void _resetTrigger(NetworkContext& context)
    {
        auto& stream = context.getStream();
        auto& monitor = stream.getMonitor();
        monitor.resetTrigger();
    }
};

class AutoImageStream
{
public:
    static void broadcast(NetworkContext& context)
    {
        auto& stream = context.getStream();
        auto& monitor = stream.getMonitor();
        monitor.callWithFpsLimit([&] { ImageStream::broadcast(context); });
    }
};

class StreamDispatcher
{
public:
    static void broadcast(NetworkContext& context)
    {
        if (!_needsBroadcast(context))
        {
            return;
        }
        if (_isImageStreamControlled(context))
        {
            ControlledImageStream::broadcast(context);
            return;
        }
        AutoImageStream::broadcast(context);
    }

private:
    static bool _needsBroadcast(NetworkContext& context)
    {
        return _hasClients(context) && _hasNewImage(context);
    }

    static bool _hasClients(NetworkContext& context)
    {
        auto& connections = context.getConnections();
        return !connections.isEmpty();
    }

    static bool _hasNewImage(NetworkContext& context)
    {
        auto& api = context.getApi();
        auto& engine = api.getEngine();
        auto& framebuffer = engine.getFrameBuffer();
        if (framebuffer.getFrameBufferFormat() == FrameBufferFormat::none)
        {
            return false;
        }
        return framebuffer.isModified();
    }

    static bool _isImageStreamControlled(NetworkContext& context)
    {
        auto& stream = context.getStream();
        auto& monitor = stream.getMonitor();
        return monitor.isControlled();
    }
};
} // namespace

namespace brayns
{
StreamManager::StreamManager(NetworkContext& context)
    : _context(&context)
{
    auto& api = context.getApi();
    auto& manager = api.getParametersManager();
    auto& parameters = manager.getApplicationParameters();
    auto fps = parameters.getImageStreamFPS();
    _imageStream.setFps(fps);
}

void StreamManager::broadcast()
{
    StreamDispatcher::broadcast(*_context);
}
} // namespace brayns
