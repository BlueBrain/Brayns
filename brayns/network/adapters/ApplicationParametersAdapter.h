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

#include <brayns/network/json/MessageAdapter.h>

#include <brayns/parameters/ApplicationParameters.h>

namespace brayns
{
BRAYNS_ADAPTER_BEGIN(ApplicationParameters)
BRAYNS_ADAPTER_GET(std::string, "engine", getEngine, "Application engine")
BRAYNS_ADAPTER_GET(std::vector<std::string>, "plugins", getPlugins,
                   "Loaded plugins")
BRAYNS_ADAPTER_GETSET(size_t, "jpeg_compression", getJpegCompression,
                      setJpegCompression, "JPEG compression rate")
BRAYNS_ADAPTER_GETSET(size_t, "image_stream_fps", getImageStreamFPS,
                      setImageStreamFPS, "Framerate of image stream")
BRAYNS_ADAPTER_GETSET(Vector2ui, "viewport", getWindowSize, setWindowSize,
                      "Window size")
BRAYNS_ADAPTER_END()
} // namespace brayns