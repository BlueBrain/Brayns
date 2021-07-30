/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

//#include <brayns/common/utils/encoder.h>

namespace brayns
{
class Encoder
{
};
} // namespace brayns

#include <brayns/network/context/NetworkContext.h>

namespace
{
using namespace brayns;

class VideoStream
{
public:
    static void broadcast(NetworkContext& context)
    {
        BRAYNS_DEBUG << "Video broadcast\n";
        /*if (!_videoParams.enabled)
        {
            _encoder.reset();
            if (_videoUpdatedResponse)
                _videoUpdatedResponse();
            _videoUpdatedResponse = nullptr;
            return;
        }

        const auto& params = _parametersManager.getApplicationParameters();
        const auto fps = params.getImageStreamFPS();
        if (fps == 0)
            return;

        if (_encoder && _encoder->kbps != _videoParams.kbps)
            _encoder.reset();

        auto& frameBuffer = _engine.getFrameBuffer();
        if (!_encoder)
        {
            int width = frameBuffer.getFrameSize().x;
            if (width % 2 != 0)
                width += 1;
            int height = frameBuffer.getFrameSize().y;
            if (height % 2 != 0)
                height += 1;

            _encoder =
                std::make_unique<Encoder>(width, height, fps, _videoParams.kbps,
                                          [&rs = _rocketsServer](auto a, auto b)
                                          { rs->broadcastBinary(a, b); });
        }

        if (_videoUpdatedResponse)
            _videoUpdatedResponse();
        _videoUpdatedResponse = nullptr;

        if (frameBuffer.getFrameBufferFormat() == FrameBufferFormat::none ||
            !frameBuffer.isModified())
        {
            return;
        }

        _encoder->encode(frameBuffer);*/
    }
};

class ImageStream
{
public:
    static void broadcast(NetworkContext& context)
    {
        BRAYNS_DEBUG << "Image broadcast\n";
    }
};

class ControlledImageStream
{
public:
    static void broadcast(NetworkContext& context)
    {
        auto& stream = context.getStream();
        auto& image = stream.getImageStream();
        if (!image.isTriggered())
        {
            return;
        }
        BRAYNS_DEBUG << "Controlled image broadcast\n";
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
        if (_useVideoStream(context))
        {
            VideoStream::broadcast(context);
            return;
        }
        if (_isImageStreamControlled(context))
        {
            ControlledImageStream::broadcast(context);
            return;
        }
        ImageStream::broadcast(context);
    }

private:
    static bool _needsBroadcast(NetworkContext& context)
    {
        auto& connections = context.getConnections();
        return !connections.isEmpty();
    }

    static bool _useVideoStream(NetworkContext& context)
    {
        auto& api = context.getApi();
        auto& manager = api.getParametersManager();
        auto& parameters = manager.getApplicationParameters();
        return parameters.useVideoStreaming();
    }

    static bool _isImageStreamControlled(NetworkContext& context)
    {
        auto& stream = context.getStream();
        auto& info = stream.getImageStream();
        return info.isControlled();
    }
};
} // namespace

namespace brayns
{
StreamManager::StreamManager(NetworkContext& context)
    : _context(&context)
{
}

StreamManager::~StreamManager() {}

void StreamManager::update()
{
    StreamDispatcher::broadcast(*_context);
    _imageStream.resetTrigger();
}
} // namespace brayns