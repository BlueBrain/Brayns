/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include "Renderer.h"

namespace
{
using brayns::Color4;
using namespace brayns::experimental;

void setBackgroundParam(OSPRenderer handle, const Color4 &color)
{
    setObjectParam(handle, "backgroundColor", color);
}

void setBackgroundParam(OSPRenderer handle, const Texture2D &texture)
{
    setObjectParam(handle, "map_backplate", texture);
}

void setBackground(OSPRenderer handle, const Background &background)
{
    std::visit([=](const auto &value) { setBackgroundParam(handle, value); }, background);
}

void setRendererParams(OSPRenderer handle, const RendererSettings &settings)
{
    setObjectParam(handle, "pixelSamples", settings.pixelSamples);
    setObjectParam(handle, "maxPathLength", settings.maxRayRecursionDepth);
    setObjectParam(handle, "minContribution", settings.minSampleContribution);
    setObjectParam(handle, "varianceThreshold", settings.varianceThreshold);
    setBackground(handle, settings.background);
    setObjectData(handle, "material", settings.materials);
    setObjectParam(handle, "pixelFilter", static_cast<OSPPixelFilterType>(settings.pixelFilter));
    commitObject(handle);
}
}

namespace brayns::experimental
{
void loadRendererParams(OSPRenderer handle, const AmbientOcclusionRendererSettings &settings)
{
    setRendererParams(handle, settings);
    setObjectParam(handle, "aoSamples", settings.aoSamples);
    setObjectParam(handle, "aoDistance", settings.aoDistance);
    setObjectParam(handle, "aoIntensity", settings.aoIntensity);
    setObjectParam(handle, "volumeSamplingRate", settings.volumeSamplingRate);
    commitObject(handle);
}

void loadRendererParams(OSPRenderer handle, const ScivisRendererSettings &settings)
{
    setRendererParams(handle, settings);
    setObjectParam(handle, "shadows", settings.shadows);
    setObjectParam(handle, "aoSamples", settings.aoSamples);
    setObjectParam(handle, "aoDistance", settings.aoDistance);
    setObjectParam(handle, "volumeSamplingRate", settings.volumeSamplingRate);
    setObjectParam(handle, "visibleLights", settings.showVisibleLights);
    commitObject(handle);
}

void loadRendererParams(OSPRenderer handle, const PathTracerSettings &settings)
{
    setRendererParams(handle, settings);
    commitObject(handle);
}
}
