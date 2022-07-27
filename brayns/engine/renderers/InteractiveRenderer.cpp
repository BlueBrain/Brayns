/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "InteractiveRenderer.h"

namespace
{
struct InteractiveParameters
{
    inline static const std::string osprayName = "scivis";
    inline static const std::string shadow = "shadows";
    inline static const std::string aoSamples = "aoSamples";
};
}

namespace brayns
{
InteractiveRenderer::InteractiveRenderer()
    : Renderer(InteractiveParameters::osprayName)
{
}

std::string InteractiveRenderer::getName() const noexcept
{
    return typeName;
}

std::unique_ptr<Renderer> InteractiveRenderer::clone() const noexcept
{
    auto result = std::make_unique<InteractiveRenderer>();
    result->setAmbientOcclusionSamples(getAmbientOcclusionSamples());
    result->setBackgroundColor(getBackgroundColor());
    result->setMaxRayBounces(getMaxRayBounces());
    result->setSamplesPerPixel(getSamplesPerPixel());
    result->setShadowsEnabled(getShadowsEnabled());
    return result;
}

void InteractiveRenderer::setShadowsEnabled(const bool enabled) noexcept
{
    getModifiedFlag().update(_shadowsEnabled, enabled);
}

bool InteractiveRenderer::getShadowsEnabled() const noexcept
{
    return _shadowsEnabled;
}

void InteractiveRenderer::setAmbientOcclusionSamples(const int32_t numSamples) noexcept
{
    getModifiedFlag().update(_aoSamples, numSamples);
}

int32_t InteractiveRenderer::getAmbientOcclusionSamples() const noexcept
{
    return _aoSamples;
}

void InteractiveRenderer::commitRendererSpecificParams()
{
    auto &osprayRenderer = getOsprayRenderer();
    osprayRenderer.setParam(InteractiveParameters::shadow, _shadowsEnabled);
    osprayRenderer.setParam(InteractiveParameters::aoSamples, _aoSamples);
}
}
