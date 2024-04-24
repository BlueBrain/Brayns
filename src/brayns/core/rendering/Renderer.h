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

#pragma once

#include <ospray/ospray_cpp.h>

#include <brayns/core/utils/Math.h>

#include "Object.h"

namespace brayns
{
enum class PixelFilter
{
    Point = OSP_PIXELFILTER_POINT,
    Box = OSP_PIXELFILTER_BOX,
    Gauss = OSP_PIXELFILTER_GAUSS,
    MitchellNetravali = OSP_PIXELFILTER_MITCHELL,
    BlackmanHarris = OSP_PIXELFILTER_BLACKMAN_HARRIS,
};

class BaseRenderer : public Object<ospray::cpp::Renderer>
{
public:
    using Object::Object;

    void setPixelSamples(std::size_t count);
    void setMaxRayRecursion(std::size_t depth);
    void setMinSampleContribution(float intensity);
    void setVarianceThreshold(float threshold);
    void setBackgroundColor(const Color4 &color);
    void setMaterials(const std::vector<ospray::cpp::Material> &materials);
    void setPixelFilter(PixelFilter filter);
};

class AmbientOcclusionRenderer : public BaseRenderer
{
public:
    using BaseRenderer::BaseRenderer;

    static inline const std::string name = "ao";

    void setAoSamples(std::size_t count);
    void setAoDistance(float distance);
    void setAoIntensity(float intensity);
};

class ScivisRenderer : public BaseRenderer
{
public:
    using BaseRenderer::BaseRenderer;

    static inline const std::string name = "scivis";

    void enableShadows(bool enable);
    void setAoSamples(std::size_t count);
    void setAoDistance(float distance);
    void showLights(bool show);
};

class PathTracer : public BaseRenderer
{
public:
    using BaseRenderer::BaseRenderer;

    static inline const std::string name = "pathtracer";
};
}
