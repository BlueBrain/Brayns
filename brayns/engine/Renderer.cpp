/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <brayns/engine/ospray/OsprayMathtypesTraits.h>

namespace
{
class RendererParameterUpdater
{
public:
    static void update(const brayns::Renderer &renderer)
    {
        static const std::string pixelParameter = "pixelSamples";
        static const std::string pathLengthParameter = "maxPathLength";
        static const std::string backgroundParameter = "backgroundColor";

        auto spp = renderer.getSamplesPerPixel();
        auto maxBounces = renderer.getMaxRayBounces();
        auto &background = renderer.getBackgroundColor();

        auto &osprayRenderer = renderer.getOsprayRenderer();
        osprayRenderer.setParam(pixelParameter, spp);
        osprayRenderer.setParam(pathLengthParameter, maxBounces);
        osprayRenderer.setParam(backgroundParameter, background);
    }
};
}

namespace brayns
{
Renderer::Renderer(const std::string &handleID)
    : _osprayRenderer(handleID)
{
}

int32_t Renderer::getSamplesPerPixel() const noexcept
{
    return _samplesPerPixel;
}

int32_t Renderer::getMaxRayBounces() const noexcept
{
    return _maxRayBounces;
}

const Vector4f &Renderer::getBackgroundColor() const noexcept
{
    return _backgroundColor;
}

void Renderer::setSamplesPerPixel(const int32_t spp) noexcept
{
    _updateValue(_samplesPerPixel, spp);
}

void Renderer::setMaxRayBounces(const int32_t maxBounces) noexcept
{
    _updateValue(_maxRayBounces, maxBounces);
}

void Renderer::setBackgroundColor(const Vector4f &background) noexcept
{
    _updateValue(_backgroundColor, background);
}

bool Renderer::commit()
{
    if (!isModified())
    {
        return false;
    }

    RendererParameterUpdater::update(*this);

    commitRendererSpecificParams();

    _osprayRenderer.commit();

    resetModified();

    return true;
}

const ospray::cpp::Renderer &Renderer::getOsprayRenderer() const noexcept
{
    return _osprayRenderer;
}
} // namespace brayns
