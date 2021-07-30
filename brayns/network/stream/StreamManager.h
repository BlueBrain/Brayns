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

#pragma once

#include <memory>

namespace brayns
{
class NetworkContext;
class Encoder;

class ImageStreamMonitor
{
public:
    bool isControlled() const { return _controlled; }

    void setControlled(bool controlled)
    {
        _controlled = controlled;
        _triggered = false;
    }

    bool isTriggered() const { return _triggered; }

    void trigger() { _triggered = true; }

    void resetTrigger() { _triggered = false; }

private:
    bool _controlled = false;
    bool _triggered = false;
};

class VideoStreamMonitor
{
public:
    bool isEnabled() const { return _enabled; }

    void setEnabled(bool enabled) { _enabled = enabled; }

    uint32_t getKbps() const { return _kbps; }

    void setKbps(uint32_t kbps) { _kbps = kbps; }

    bool operator==(const VideoStreamMonitor& other) const
    {
        return _enabled == other._enabled && _kbps == other._kbps;
    }

    bool operator!=(const VideoStreamMonitor& other) const
    {
        return !(*this == other);
    }

private:
    bool _enabled = false;
    uint32_t _kbps = 5000;
};

class StreamManager
{
public:
    StreamManager(NetworkContext& context);
    ~StreamManager();

    void update();

    ImageStreamMonitor& getImageStream() { return _imageStream; }

    VideoStreamMonitor& getVideoStream() { return _videoStream; }

private:
    NetworkContext* _context;
    ImageStreamMonitor _imageStream;
    VideoStreamMonitor _videoStream;
    std::unique_ptr<Encoder> _encoder;
};
} // namespace brayns