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

namespace brayns
{
Renderer::Renderer(const Renderer &o)
{
    *this = o;
}

Renderer &Renderer::operator=(const Renderer &o)
{
    _samplesPerPixel = o._samplesPerPixel;
    _maxRayBounces = o._maxRayBounces;
    _backgroundColor = o._backgroundColor;

    markModified();

    return *this;
}

Renderer::~Renderer()
{
    ospRelease(_handle);
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

void Renderer::setBackgroundColor(const Vector4f& background) noexcept
{
    _updateValue(_backgroundColor, background);
}

bool Renderer::commit()
{
    if(!isModified())
    {
        return false;
    }

    if(!_handle)
    {
        const auto handleName = getOSPHandleName();
        _handle = ospNewRenderer(handleName.data());
    }

    ospSetParam(_handle, "pixelSamples", OSPDataType::OSP_INT, &_samplesPerPixel);
    ospSetParam(_handle, "maxPathLength", OSPDataType::OSP_INT, &_maxRayBounces);
    ospSetParam(_handle, "backgroundColor", OSPDataType::OSP_VEC4F, &_backgroundColor);

    commitRendererSpecificParams();

    ospCommit(_handle);

    resetModified();

    return true;
}

OSPRenderer Renderer::handle() const noexcept
{
    return _handle;
}
} // namespace brayns
