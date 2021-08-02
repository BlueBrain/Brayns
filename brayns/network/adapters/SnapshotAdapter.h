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

#include <brayns/tasks/SnapshotTask.h>

namespace brayns
{
BRAYNS_ADAPTER_BEGIN(SnapshotParams)
BRAYNS_ADAPTER_NAMED_ENTRY(std::unique_ptr<AnimationParameters>,
                           "animation_parameters", animParams,
                           "Animation parameters");
BRAYNS_ADAPTER_NAMED_ENTRY(std::unique_ptr<Camera>, "camera", camera,
                           "Camera parameters");
BRAYNS_ADAPTER_NAMED_ENTRY(std::string, "format", format,
                           "Image format from FreeImage");
BRAYNS_ADAPTER_NAMED_ENTRY(std::string, "name", name, "Name of the snapshot");
BRAYNS_ADAPTER_NAMED_ENTRY(size_t, "quality", quality,
                           "Image quality from 0 to 100");
BRAYNS_ADAPTER_NAMED_ENTRY(std::unique_ptr<RenderingParameters>, "renderer",
                           renderingParams, "Renderer parameters");
BRAYNS_ADAPTER_NAMED_ENTRY(int, "samples_per_pixel", samplesPerPixel,
                           "Samples per pixel");
BRAYNS_ADAPTER_NAMED_ENTRY(Vector2ui, "size", size, "Image dimensions");
BRAYNS_ADAPTER_NAMED_ENTRY(std::string, "file_path", filePath,
                           "Path if saved on disk");
BRAYNS_ADAPTER_END()
} // namespace brayns