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

namespace
{
using namespace brayns;

class VideoStream
{
public:
    static void broadcast(NetworkContext& context)
    {
    }
};

class JpegStream
{
public:
    static void broadcast(NetworkContext& context)
    {
    }
};

class ControlledJpegStream
{
public:
    static void broadcast(NetworkContext& context)
    {
    }
};

class StreamDispatcher
{
public:
    static void broadcast(NetworkContext& context)
    {
        if (!_hasClients(context))
        {
            return;
        }
        if (_useVideoStream(context))
        {
#ifdef BRAYNS_USE_FFMPEG
            VideoStream::broadcast(context);
#endif
            return;
        }
        if (_isControlled(context))
        {
            ControlledJpegStream::broadcast(context);
            return;
        }
        JpegStream::broadcast(context);
    }

private:
    static bool _hasClients(NetworkContext& context)
    {
        auto& clients = context.getClients();
        return !clients.isEmpty();
    }

    static bool _useVideoStream(NetworkContext& context)
    {
        auto& api = context.getApi();
        auto& manager = api.getParametersManager();
        auto& parameters = manager.getApplicationParameters();
        return parameters.useVideoStreaming();
    }

    static bool _isControlled(NetworkContext& context)
    {
        auto& streamController = context.getStreamController();
        return streamController.isControlled();
    }
};
}

namespace brayns
{
void StreamManager::broadcast(NetworkContext& context)
{
    StreamDispatcher::broadcast(context);
}
}