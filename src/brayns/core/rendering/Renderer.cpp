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

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace brayns
{
void BaseRenderer::setPixelSamples(std::size_t count)
{
    setParam("pixelSamples", static_cast<int>(count));
}

void BaseRenderer::setMaxRayRecursion(std::size_t depth)
{
    setParam("maxPathLength", static_cast<int>(depth));
}

void BaseRenderer::setMinSampleContribution(float intensity)
{
    setParam("minContribution", intensity);
}

void BaseRenderer::setVarianceThreshold(float threshold)
{
    setParam("varianceThreshold", threshold);
}

void BaseRenderer::setBackgroundColor(const Color4 &color)
{
    setParam("backgroundColor", color);
}

void BaseRenderer::setMaterials(const std::vector<ospray::cpp::Material> &materials)
{
    setParam("material", ospray::cpp::SharedData(materials));
}

void BaseRenderer::setPixelFilter(PixelFilter filter)
{
    setParam("pixelFilter", static_cast<OSPPixelFilterType>(filter));
}

void AmbientOcclusionRenderer::setAoSamples(std::size_t count)
{
    setParam("aoSamples", static_cast<int>(count));
}

void AmbientOcclusionRenderer::setAoDistance(float distance)
{
    setParam("aoDistance", distance);
}

void AmbientOcclusionRenderer::setAoIntensity(float intensity)
{
    setParam("aoIntensity", intensity);
}

void ScivisRenderer::enableShadows(bool enable)
{
    setParam("shadows", enable);
}

void ScivisRenderer::setAoSamples(std::size_t count)
{
    setParam("aoSamples", static_cast<int>(count));
}

void ScivisRenderer::setAoDistance(float distance)
{
    setParam("aoDistance", distance);
}

void ScivisRenderer::showLights(bool show)
{
    setParam("visibleLights", show);
}
}
