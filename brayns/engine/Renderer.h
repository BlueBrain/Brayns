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

#pragma once

#include <brayns/common/MathTypes.h>
#include <brayns/engine/EngineObject.h>

#include <ospray/ospray.h>

#include <memory>

namespace brayns
{
class Renderer : public SerializableEngineObject
{
public:
    using Ptr = std::unique_ptr<Renderer>;

    virtual ~Renderer();

    void commit() final;

    int32_t getSamplesPerPixel() const noexcept;
    int32_t getMaxRayBounces() const noexcept;
    const Vector4f &getBackgroundColor() const noexcept;

    void setSamplesPerPixel(const int32_t spp) noexcept;
    void setMaxRayBounces(const int32_t maxBounces) noexcept;
    void setBackgroundColor(const Vector4f& background) noexcept;

    OSPRenderer handle() const noexcept;

protected:
    virtual void commitRendererSpecificParams() = 0;

private:
    int32_t _samplesPerPixel {1};
    int32_t _maxRayBounces {5};

    Vector4f _backgroundColor {0.004f, 0.016f, 0.102f, 0.f};

protected:
    OSPRenderer _handle {nullptr};
};
} // namespace brayns
