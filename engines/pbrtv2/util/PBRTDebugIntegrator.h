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

#include <pbrtv2/core/integrator.h>
#include <pbrtv2/core/pbrt.h>
#include <pbrtv2/core/renderer.h>
#include <pbrtv2/core/scene.h>
#include <pbrtv2/core/volume.h>

namespace brayns
{
class PBRTDebugSurfaceIntegrator : public pbrt::SurfaceIntegrator
{
public:
    PBRTDebugSurfaceIntegrator()
    {
        constexpr float greenRGB[3] = {0.f, 0.5f, 0.f};
        _green = pbrt::Spectrum::FromRGB(greenRGB);
    }

    pbrt::Spectrum Li(const pbrt::Scene *scene, const pbrt::Renderer *renderer,
                      const pbrt::RayDifferential &ray,
                      const pbrt::Intersection &isect,
                      const pbrt::Sample *sample, pbrt::RNG &rng,
                      pbrt::MemoryArena &arena) const final;

private:
    pbrt::Spectrum _green;
};

class PBRTDebugVolumeIntegrator : public pbrt::VolumeIntegrator
{
public:
    PBRTDebugVolumeIntegrator()
    {
        constexpr float whiteRGB[3] = {1.f, 1.f, 1.f};
        _white = pbrt::Spectrum::FromRGB(whiteRGB);

        constexpr float blackRGB[3] = {0.f, 0.f, 0.f};
        _black = pbrt::Spectrum::FromRGB(blackRGB);

        constexpr float redRGB[3] = {0.5f, 0.0f, 0.f};
        _red = pbrt::Spectrum::FromRGB(redRGB);
    }

    pbrt::Spectrum Li(const pbrt::Scene *scene, const pbrt::Renderer *renderer,
                      const pbrt::RayDifferential &ray,
                      const pbrt::Sample *sample, pbrt::RNG &rng,
                      pbrt::Spectrum *transmittance,
                      pbrt::MemoryArena &arena) const final;
    pbrt::Spectrum Transmittance(const pbrt::Scene *scene,
                                 const pbrt::Renderer *renderer,
                                 const pbrt::RayDifferential &ray,
                                 const pbrt::Sample *sample, pbrt::RNG &rng,
                                 pbrt::MemoryArena &arena) const final;

private:
    pbrt::Spectrum _white;
    pbrt::Spectrum _black;
    pbrt::Spectrum _red;
};
} // namespace brayns

