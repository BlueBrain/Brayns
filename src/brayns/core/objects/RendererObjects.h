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

#include <any>
#include <concepts>
#include <functional>

#include <brayns/core/engine/Renderer.h>
#include <brayns/core/jsonrpc/PayloadReflector.h>
#include <brayns/core/manager/ObjectManager.h>

#include "MaterialObjects.h"
#include "TextureObjects.h"

namespace brayns
{
using BackgroundStorage = BackgroundOf<Stored<UserTexture>>;

struct RendererStorage
{
    RendererSettings settings;
    std::vector<Stored<UserMaterial>> materials;
    BackgroundStorage background;
    std::optional<Stored<UserTexture>> maxDepth;
};

struct UserRenderer
{
    RendererStorage storage;
    std::any value;
    std::function<Renderer()> get;
};

struct BackgroundTexture
{
    ObjectId texture2d;
};

template<>
struct JsonObjectReflector<BackgroundTexture>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<BackgroundTexture>();
        builder.field("texture2d", [](auto &object) { return &object.texture2d; }).description("ID of the texture 2D to use as background");
        return builder.build();
    }
};

using BackgroundParams = BackgroundOf<BackgroundTexture>;

struct RendererParams
{
    RendererSettings value;
    std::vector<ObjectId> materials;
    BackgroundParams background;
    std::optional<ObjectId> maxDepth;
};

template<>
struct EnumReflector<PixelFilter>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<PixelFilter>();
        builder.field("Point", PixelFilter::Point).description("Only sample the center of the pixel (width = 0)");
        builder.field("Box", PixelFilter::Box).description("Sample pixel uniformly (width = 1)");
        builder.field("Gauss", PixelFilter::Gauss).description("Sample pixel with a truncated Gauss profile (sigma = 0.5, width = 3)");
        builder.field("MitchellNetravali", PixelFilter::MitchellNetravali).description("Sample pixel with a Mitchell-Netravali profile (width = 4)");
        builder.field("BlackmanHarris", PixelFilter::BlackmanHarris).description("Sample pixel with a Blackman-Harris profile (width = 3)");
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<RendererParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<RendererParams>();
        builder.field("materials", [](auto &object) { return &object.materials; })
            .description("Materials that can be used by the renderer and indexed in the scene")
            .minItems(1);
        builder.field("samples", [](auto &object) { return &object.value.samples; })
            .description("Number of rays per pixel")
            .defaultValue(1)
            .minimum(1);
        builder.field("maxRecursion", [](auto &object) { return &object.value.maxRecursion; })
            .description("Maximum ray recursion depth")
            .defaultValue(20)
            .minimum(1);
        builder.field("minContribution", [](auto &object) { return &object.value.minContribution; })
            .description("Samples contribution below this value will be neglected to speed-up rendering")
            .defaultValue(0.001F)
            .minimum(0);
        builder.field("varianceThreshold", [](auto &object) { return &object.value.varianceThreshold; })
            .description("Variance threshold for adaptative accumulation (based on variance channel in framebuffer)")
            .defaultValue(0.0F)
            .minimum(0);
        builder.field("background", [](auto &object) { return &object.background; })
            .description("Background linear alpha, RGB, RGBA or texture 2D")
            .defaultValue(Color4{0.0F, 0.0F, 0.0F, 0.0F});
        builder.field("maxDepth", [](auto &object) { return &object.maxDepth; })
            .description("Optional screen-sized texture 2D with max ray distance per pixel (avoid rendering masked regions)");
        builder.field("pixelFilter", [](auto &object) { return &object.value.pixelFilter; })
            .description("Antialiasing type")
            .defaultValue(PixelFilter::Gauss);
        return builder.build();
    }
};

using GetRendererResult = GetResultOf<RendererParams>;
using UpdateRendererParams = UpdateParamsOf<RendererParams>;

GetRendererResult getRenderer(ObjectManager &objects, const GetObjectParams &params);
void updateRenderer(ObjectManager &objects, Device &device, const UpdateRendererParams &params);

template<typename Storage, std::derived_from<Renderer> T>
struct UserRendererOf
{
    Storage storage;
    T value;
};

template<ReflectedJsonObject T>
using CreateRendererParamsOf = CreateParamsOf<ComposedParamsOf<RendererParams, T>>;

template<>
struct JsonObjectReflector<AoRendererSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<AoRendererSettings>();
        builder.field("aoSamples", [](auto &object) { return &object.aoSamples; }).description("Number of samples to use for AO").defaultValue(1);
        builder.field("aoDistance", [](auto &object) { return &object.aoDistance; })
            .description("Max distance to compute AO")
            .defaultValue(1e20F)
            .minimum(0);
        builder.field("aoIntensity", [](auto &object) { return &object.aoIntensity; }).description("AO intensity").defaultValue(1).minimum(0);
        builder.field("volumeSamplingRate", [](auto &object) { return &object.volumeSamplingRate; })
            .description("Sampling rate for volumes")
            .defaultValue(1)
            .minimum(1);
        return builder.build();
    }
};

using CreateAoRendererParams = CreateRendererParamsOf<AoRendererSettings>;
using GetAoRendererResult = GetResultOf<AoRendererSettings>;
using UpdateAoRendererParams = UpdateParamsOf<AoRendererSettings>;
using UserAoRenderer = UserRendererOf<AoRendererSettings, AoRenderer>;

CreateObjectResult createAoRenderer(ObjectManager &objects, Device &device, const CreateAoRendererParams &params);
GetAoRendererResult getAoRenderer(ObjectManager &objects, const GetObjectParams &params);
void updateAoRenderer(ObjectManager &objects, Device &device, const UpdateAoRendererParams &params);

template<>
struct JsonObjectReflector<ScivisRendererSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<ScivisRendererSettings>();
        builder.field("shadows", [](auto &object) { return &object.shadows; }).description("Wether to compute shadows").defaultValue(false);
        builder.field("aoSamples", [](auto &object) { return &object.aoSamples; }).description("Number of samples to use for AO").defaultValue(0);
        builder.field("aoDistance", [](auto &object) { return &object.aoDistance; })
            .description("Max distance to compute AO")
            .defaultValue(1e20F)
            .minimum(0);
        builder.field("volumeSamplingRate", [](auto &object) { return &object.volumeSamplingRate; })
            .description("Sampling rate for volumes")
            .defaultValue(1)
            .minimum(1);
        builder.field("showVisibleLights", [](auto &object) { return &object.showVisibleLights; })
            .description("Wether to render lights when their visible flag is true")
            .defaultValue(false);
        return builder.build();
    }
};

using CreateScivisRendererParams = CreateRendererParamsOf<ScivisRendererSettings>;
using GetScivisRendererResult = GetResultOf<ScivisRendererSettings>;
using UpdateScivisRendererParams = UpdateParamsOf<ScivisRendererSettings>;
using UserScivisRenderer = UserRendererOf<ScivisRendererSettings, ScivisRenderer>;

CreateObjectResult createScivisRenderer(ObjectManager &objects, Device &device, const CreateScivisRendererParams &params);
GetScivisRendererResult getScivisRenderer(ObjectManager &objects, const GetObjectParams &params);
void updateScivisRenderer(ObjectManager &objects, Device &device, const UpdateScivisRendererParams &params);

template<>
struct JsonObjectReflector<PathTracerSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<PathTracerSettings>();
        return builder.build();
    }
};

using CreatePathTracerParams = CreateRendererParamsOf<PathTracerSettings>;
using GetPathTracerResult = GetResultOf<PathTracerSettings>;
using UpdatePathTracerParams = UpdateParamsOf<PathTracerSettings>;
using UserPathTracer = UserRendererOf<PathTracerSettings, PathTracer>;

CreateObjectResult createPathTracer(ObjectManager &objects, Device &device, const CreatePathTracerParams &params);
GetPathTracerResult getPathTracer(ObjectManager &objects, const GetObjectParams &params);
void updatePathTracer(ObjectManager &objects, Device &device, const UpdatePathTracerParams &params);
}
