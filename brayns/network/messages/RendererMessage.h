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

#include <brayns/parameters/RenderingParameters.h>

namespace brayns
{
BRAYNS_MESSAGE_BEGIN(RendererMessage)
BRAYNS_MESSAGE_ENTRY(bool, accumulation, "Multiple render passes")
BRAYNS_MESSAGE_ENTRY(Vector3d, background_color, "Background color RGB")
BRAYNS_MESSAGE_ENTRY(std::string, current, "Current renderer name")
BRAYNS_MESSAGE_ENTRY(bool, head_light, "Light source follow camera origin")
BRAYNS_MESSAGE_ENTRY(size_t, max_accum_frames, "Max render passes")
BRAYNS_MESSAGE_ENTRY(uint32_t, samples_per_pixel, "Samples per pixel")
BRAYNS_MESSAGE_ENTRY(uint32_t, subsampling, "Subsampling")
BRAYNS_MESSAGE_ENTRY(std::deque<std::string>, types, "Available renderers")
BRAYNS_MESSAGE_ENTRY(double, variance_threshold, "Stop accumulation threshold")

static RenderingParameters& extract(PluginAPI& api)
{
    auto& parametersManager = api.getParametersManager();
    return parametersManager.getRenderingParameters();
}

void dump(RenderingParameters& renderingParameters) const
{
    renderingParameters.setAccumulation(accumulation);
    renderingParameters.setBackgroundColor(background_color);
    renderingParameters.setCurrentRenderer(current);
    renderingParameters.setHeadLight(head_light);
    renderingParameters.setMaxAccumFrames(max_accum_frames);
    renderingParameters.setSamplesPerPixel(samples_per_pixel);
    renderingParameters.setSubsampling(subsampling);
    renderingParameters.setVarianceThreshold(variance_threshold);
}

void load(const RenderingParameters& renderingParameters)
{
    accumulation = renderingParameters.getAccumulation();
    background_color = renderingParameters.getBackgroundColor();
    current = renderingParameters.getCurrentRenderer();
    head_light = renderingParameters.getHeadLight();
    max_accum_frames = renderingParameters.getMaxAccumFrames();
    samples_per_pixel = renderingParameters.getSamplesPerPixel();
    subsampling = renderingParameters.getSubsampling();
    types = renderingParameters.getRenderers();
    variance_threshold = renderingParameters.getVarianceThreshold();
}

BRAYNS_MESSAGE_END()
} // namespace brayns