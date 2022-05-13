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
        auto packet = brayns::OutputPacket::fromBinary(data);
        clients.broadcast(packet);
    }
};

class ControlledImageStream
{
public:
    ControlledImageStream(bool &triggered)
        : _triggered(triggered)
    {
    }

    void broadcast(
        brayns::FrameBuffer &framebuffer,
        brayns::ClientManager &clients,
        const brayns::ApplicationParameters &parameters) const
    {
        if (!_triggered)
        {
            brayns::Log::debug("JPEG stream not triggered.");
            return;
        }
        if (clients.isEmpty())
        {
            brayns::Log::debug("No clients to broadcast JPEG.");
            return;
        }
        if (!framebuffer.isModified())
        {
            brayns::Log::debug("Framebuffer not modified for JPEG broadcast.");
            return;
        }
        ImageStream::broadcast(framebuffer, clients, parameters);
        _triggered = false;
        brayns::Log::debug("JPEG image broadcasted.");
    }

private:
    bool &_triggered;
};
} // namespace

namespace brayns
{
void StreamManager::broadcast(FrameBuffer &framebuffer, ClientManager &clients, const ApplicationParameters &parameters)
{
    brayns::Log::debug("Broadcasting JPEG image.");
    ControlledImageStream stream(_triggered);
    stream.broadcast(framebuffer, clients, parameters);
}

void StreamManager::trigger()
{
    _triggered = true;
}
} // namespace brayns
