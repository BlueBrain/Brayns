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

#include <brayns/network/message/Message.h>

#include <brayns/parameters/ApplicationParameters.h>

namespace brayns
{
BRAYNS_MESSAGE_BEGIN(ApplicationParametersMessage)
BRAYNS_MESSAGE_ENTRY(std::string, engine, "Application rendering engine")
BRAYNS_MESSAGE_ENTRY(std::vector<std::string>, plugins, "Loaded plugins")
BRAYNS_MESSAGE_ENTRY(size_t, jpeg_compression, "JPEG compression rate")
BRAYNS_MESSAGE_ENTRY(size_t, image_stream_fps, "Framerate of image stream")
BRAYNS_MESSAGE_ENTRY(Vector2d, viewport, "Window size")

static ApplicationParameters& extract(PluginAPI& api)
{
    auto& parametersManager = api.getParametersManager();
    return parametersManager.getApplicationParameters();
}

void dump(ApplicationParameters& applicationParameters) const
{
    applicationParameters.setJpegCompression(jpeg_compression);
    applicationParameters.setImageStreamFPS(image_stream_fps);
    applicationParameters.setWindowSize(viewport);
}

void load(const ApplicationParameters& applicationParameters)
{
    engine = applicationParameters.getEngine();
    plugins = applicationParameters.getPlugins();
    jpeg_compression = applicationParameters.getJpegCompression();
    image_stream_fps = applicationParameters.getImageStreamFPS();
    viewport = applicationParameters.getWindowSize();
}

BRAYNS_MESSAGE_END()
} // namespace brayns