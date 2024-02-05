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

#include <brayns/tasks/SnapshotTask.h>

namespace brayns
{
BRAYNS_JSON_ADAPTER_BEGIN(SnapshotParams)
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("animation_parameters", animParams,
                                "Animation parameters", Required(false));
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("camera", camera, "Camera parameters",
                                Required(false));
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("format", format,
                                "Image format (extension without the dot)");
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("name", name, "Name of the snapshot",
                                Required(false));
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("quality", quality,
                                "Image quality from 0 to 100", Required(false));
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("renderer", renderingParams,
                                "Renderer parameters", Required(false));
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("samples_per_pixel", samplesPerPixel,
                                "Samples per pixel", Required(false));
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("size", size, "Image dimensions");
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("file_path", filePath, "Path if saved on disk",
                                Required(false));
BRAYNS_JSON_ADAPTER_END()
} // namespace brayns
