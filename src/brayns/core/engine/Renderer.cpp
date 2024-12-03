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
using namespace brayns;

void setBackgroundParam(OSPRenderer handle, const auto &color)
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
    setObjectParam(handle, "material", settings.materials);

    setObjectParam(handle, "pixelSamples", static_cast<int>(settings.samples));
    setObjectParam(handle, "maxPathLength", static_cast<int>(settings.maxRecursion));
    setObjectParam(handle, "minContribution", settings.minContribution);
    setObjectParam(handle, "varianceThreshold", settings.varianceThreshold);
    setBackground(handle, settings.background);

    if (settings.maxDepth)
    {
        setObjectParam(handle, "maxDepth", *settings.maxDepth);
    }

    setObjectParam(handle, "pixelFilter", static_cast<OSPPixelFilterType>(settings.pixelFilter));
}

void setAoRendererParams(OSPRenderer handle, const AoRendererSettings &settings)
{
    setObjectParam(handle, "aoSamples", static_cast<int>(settings.aoSamples));
    setObjectParam(handle, "aoDistance", settings.aoDistance);
    setObjectParam(handle, "aoIntensity", settings.aoIntensity);
    setObjectParam(handle, "volumeSamplingRate", settings.volumeSamplingRate);
}

void setScivisRendererParams(OSPRenderer handle, const ScivisRendererSettings &settings)
{
    setObjectParam(handle, "shadows", settings.shadows);
    setObjectParam(handle, "aoSamples", static_cast<int>(settings.aoSamples));
    setObjectParam(handle, "aoDistance", settings.aoDistance);
    setObjectParam(handle, "volumeSamplingRate", settings.volumeSamplingRate);
    setObjectParam(handle, "visibleLights", settings.showVisibleLights);
}

void setPathTracerParams(OSPRenderer handle, const PathTracerSettings &settings)
{
    (void)handle;
    (void)settings;
}
}

namespace brayns
{
void Renderer::update(const RendererSettings &settings)
{
    auto handle = getHandle();
    setRendererParams(handle, settings);
    commitObject(handle);
}

void AoRenderer::update(const AoRendererSettings &settings)
{
    auto handle = getHandle();
    setAoRendererParams(handle, settings);
    commitObject(handle);
}

AoRenderer createAoRenderer(Device &device, const RendererSettings &settings, const AoRendererSettings &ao)
{
    auto handle = ospNewRenderer("ao");
    auto renderer = wrapObjectHandleAs<AoRenderer>(device, handle);

    setRendererParams(handle, settings);
    setAoRendererParams(handle, ao);

    commitObject(device, handle);

    return renderer;
}

void ScivisRenderer::update(const ScivisRendererSettings &settings)
{
    auto handle = getHandle();
    setScivisRendererParams(handle, settings);
    commitObject(handle);
}

ScivisRenderer createScivisRenderer(Device &device, const RendererSettings &settings, const ScivisRendererSettings &scivis)
{
    auto handle = ospNewRenderer("scivis");
    auto renderer = wrapObjectHandleAs<ScivisRenderer>(device, handle);

    setRendererParams(handle, settings);
    setScivisRendererParams(handle, scivis);

    commitObject(device, handle);

    return renderer;
}

void PathTracer::update(const PathTracerSettings &settings)
{
    (void)settings;
}

PathTracer createPathTracer(Device &device, const RendererSettings &settings, const PathTracerSettings &pathTracer)
{
    auto handle = ospNewRenderer("pathtracer");
    auto renderer = wrapObjectHandleAs<PathTracer>(device, handle);

    setRendererParams(handle, settings);
    setPathTracerParams(handle, pathTracer);

    commitObject(device, handle);

    return renderer;
}
}
