/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#pragma once

#include <brayns/json/JsonAdapterMacro.h>

#include <brayns/parameters/ApplicationParameters.h>

namespace brayns
{
BRAYNS_JSON_ADAPTER_BEGIN(ApplicationParameters)
BRAYNS_JSON_ADAPTER_GET("plugins", getPlugins, "Loaded plugins")
BRAYNS_JSON_ADAPTER_GETSET("jpeg_compression", getJpegCompression,
                           setJpegCompression, "JPEG compression rate")
BRAYNS_JSON_ADAPTER_GETSET("image_stream_fps", getImageStreamFPS,
                           setImageStreamFPS, "Framerate of image stream")
BRAYNS_JSON_ADAPTER_GETSET("viewport", getWindowSize, setWindowSize,
                           "Window size")
BRAYNS_JSON_ADAPTER_END()
} // namespace brayns
