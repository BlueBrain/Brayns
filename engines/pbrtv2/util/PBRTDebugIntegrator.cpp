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
#include "PBRTDebugIntegrator.h"

namespace brayns
{
pbrt::Spectrum PBRTDebugSurfaceIntegrator::Li(const pbrt::Scene *scene,
                                              const pbrt::Renderer *,
                                              const pbrt::RayDifferential &ray,
                                              const pbrt::Intersection &,
                                              const pbrt::Sample *, pbrt::RNG &,
                                              pbrt::MemoryArena &) const
{
    return _green;
}

pbrt::Spectrum PBRTDebugVolumeIntegrator::Li(const pbrt::Scene *scene,
                                             const pbrt::Renderer *,
                                             const pbrt::RayDifferential &ray,
                                             const pbrt::Sample *, pbrt::RNG &,
                                             pbrt::Spectrum *T,
                                             pbrt::MemoryArena &) const
{
    *T = _white;
    float t0, t1;
    return scene->volumeRegion == nullptr
               ? _black
               : scene->volumeRegion->IntersectP(ray, &t0, &t1) ? _red : _black;
}

pbrt::Spectrum PBRTDebugVolumeIntegrator::Transmittance(
    const pbrt::Scene *, const pbrt::Renderer *, const pbrt::RayDifferential &,
    const pbrt::Sample *, pbrt::RNG &, pbrt::MemoryArena &) const
{
    return _white;
}
} // namespace brayns
