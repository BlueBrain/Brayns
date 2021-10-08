/* Copyright (c) 2020, EPFL/Blue Brain Project
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

#pragma once

#include <pbrt/core/integrator.h>
#include <pbrt/core/pbrt.h>
#include <pbrt/core/scene.h>

namespace brayns
{
class PBRTDebugIntegrator : public pbrt::SamplerIntegrator
{
public:
    PBRTDebugIntegrator(std::shared_ptr<const pbrt::Camera> cam,
                        std::shared_ptr<pbrt::Sampler> sam,
                        const pbrt::Bounds2i& pb)
        : pbrt::SamplerIntegrator(cam, sam, pb)
    {
        constexpr pbrt::Float whiteRGB[3] = {pbrt::Float(1), pbrt::Float(1),
                                             pbrt::Float(1)};
        _white = pbrt::Spectrum::FromRGB(whiteRGB);

        constexpr pbrt::Float greenRGB[3] = {pbrt::Float(0), pbrt::Float(0.5),
                                             pbrt::Float(0)};
        _green = pbrt::Spectrum::FromRGB(greenRGB);
    }

    pbrt::Spectrum Li(const pbrt::RayDifferential& r, const pbrt::Scene& s,
                      pbrt::Sampler& /* sampler*/,
                      pbrt::MemoryArena& /* arena*/,
                      int /*depth*/) const override;

private:
    pbrt::Spectrum _white;
    pbrt::Spectrum _green;
};
} // namespace brayns

