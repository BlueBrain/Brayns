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

#include <optional>
#include <variant>

#include "Data.h"
#include "Material.h"
#include "Object.h"
#include "Texture.h"

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

using Background = std::variant<Color4, Texture2D>;

struct RendererSettings
{
    std::size_t pixelSamples = 1;
    std::size_t maxRayRecursionDepth = 20;
    float minSampleContribution = 0.001F;
    float varianceThreshold = 0.0F;
    Background background = Color4(0.0F, 0.0F, 0.0F, 0.0F);
    std::optional<Texture2D> maxDepth = std::nullopt;
    std::span<Material> materials = {};
    PixelFilter pixelFilter = PixelFilter::Gauss;
};

class Renderer : public Managed<OSPRenderer>
{
public:
    using Managed::Managed;
};

struct AmbientOcclusionRendererSettings
{
    RendererSettings base = {};
    std::size_t aoSamples = 1;
    float aoDistance = 1e20F;
    float aoIntensity = 1.0F;
    float volumeSamplingRate = 1.0F;
};

class AmbientOcclusionRenderer : public Renderer
{
public:
    using Renderer::Renderer;
};

template<>
struct ObjectReflector<AmbientOcclusionRenderer>
{
    using Settings = AmbientOcclusionRendererSettings;

    static OSPRenderer createHandle(OSPDevice device, const Settings &settings);
};

struct ScivisRendererSettings
{
    RendererSettings base = {};
    bool shadows = false;
    std::size_t aoSamples = 0;
    float aoDistance = 1e20F;
    float volumeSamplingRate = 1.0F;
    bool showVisibleLights = false;
};

class ScivisRenderer : public Renderer
{
public:
    using Renderer::Renderer;
};

template<>
struct ObjectReflector<ScivisRenderer>
{
    using Settings = ScivisRendererSettings;

    static OSPRenderer createHandle(OSPDevice device, const Settings &settings);
};

struct PathTracerSettings
{
    RendererSettings base = {};
};

class PathTracer : public Renderer
{
public:
    using Renderer::Renderer;
};

template<>
struct ObjectReflector<PathTracer>
{
    using Settings = PathTracerSettings;

    static OSPRenderer createHandle(OSPDevice device, const Settings &settings);
};
}
