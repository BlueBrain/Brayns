/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <brayns/network/adapters/VideoStreamAdapter.h>
#include <brayns/network/entrypoint/Entrypoint.h>

namespace brayns
{
class VideoStreamSupport
{
public:
    static void checkEnabled(PluginAPI& api)
    {
#if BRAYNS_USE_FFMPEG
        auto& manager = api.getParametersManager();
        auto& applicationParameters = manager.getApplicationParameters();
        if (!applicationParameters.useVideoStreaming())
        {
            throw EntrypointException(
                "Brayns was not started with videostream support enabled");
        }
#else
        throw EntrypointException(
            "Brayns was not build with videostream support");
#endif
    }
};

class GetVideoStreamEntrypoint
    : public Entrypoint<EmptyMessage, VideoStreamMonitor>
{
public:
    virtual std::string getName() const override { return "get-videostream"; }

    virtual std::string getDescription() const override
    {
        return "Get the current state of the video stream parameters";
    }

    virtual void onRequest(const Request& request) override
    {
        VideoStreamSupport::checkEnabled(getApi());
        auto& videoStream = getStream().getVideoStream();
        request.reply(videoStream);
    }
};

class SetVideoStreamEntrypoint
    : public Entrypoint<VideoStreamMonitor, EmptyMessage>
{
public:
    virtual std::string getName() const override { return "set-videostream"; }

    virtual std::string getDescription() const override
    {
        return "Set the current state of the video stream parameters";
    }

    virtual void onRequest(const Request& request) override
    {
        VideoStreamSupport::checkEnabled(getApi());
        auto params = request.getParams();
        auto& videoStream = getStream().getVideoStream();
        if (params == videoStream)
        {
            request.reply(EmptyMessage());
            return;
        }
        videoStream = params;
        triggerRender();
        request.notify(videoStream);
        request.reply(EmptyMessage());
    }
};
} // namespace brayns