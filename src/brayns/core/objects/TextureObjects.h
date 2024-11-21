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

#include <brayns/core/engine/Texture.h>
#include <brayns/core/jsonrpc/PayloadReflector.h>
#include <brayns/core/manager/ObjectManager.h>

#include "TransferFunctionObjects.h"
#include "VolumeObjects.h"

namespace brayns
{
struct UserTexture
{
    std::any value;
    std::function<Texture()> get;
};

template<typename Storage, std::derived_from<Texture> T>
struct UserTextureOf
{
    Storage storage;
    T value;
};

template<>
struct EnumReflector<TextureFormat>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<TextureFormat>();
        builder.field("Rgba8", TextureFormat::Rgba8).description("8 bit linear RGBA");
        builder.field("Srgba8", TextureFormat::Srgba8).description("8 bit gamma-encoded RGBA");
        builder.field("Rgba32F", TextureFormat::Rgba32F).description("32 bit float RGBA");
        builder.field("Rgb8", TextureFormat::Rgb8).description("8 bit linear RGB");
        builder.field("Srgb8", TextureFormat::Srgb8).description("8 bit gamma-encoded RGB");
        builder.field("Rgb32F", TextureFormat::Rgb32F).description("32 bit float RGB");
        builder.field("R8", TextureFormat::R8).description("8 bit R");
        builder.field("Ra8", TextureFormat::Ra8).description("8 bit RA");
        builder.field("L8", TextureFormat::L8).description("8 bit luminance");
        builder.field("La8", TextureFormat::La8).description("8 bit luminance and alpha");
        builder.field("R32F", TextureFormat::R32F).description("32 bit float R");
        builder.field("Rgba16", TextureFormat::Rgba16).description("16 bit linear RGBA");
        builder.field("Rgb16", TextureFormat::Rgb16).description("16 bit linear RGB");
        builder.field("Ra16", TextureFormat::Ra16).description("16 bit linear RA");
        builder.field("R16", TextureFormat::R16).description("16 bit linear R");
        return builder.build();
    }
};

template<>
struct EnumReflector<TextureFilter>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<TextureFilter>();
        builder.field("Linear", TextureFilter::Linear).description("Sample with linear interpolation");
        builder.field("Nearest", TextureFilter::Nearest).description("Sample with nearest texel");
        return builder.build();
    }
};

template<>
struct EnumReflector<TextureWrap>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<TextureWrap>();
        builder.field("Repeat", TextureWrap::Repeat).description("Wrap texture by repeating the texels");
        builder.field("MirroredRepeat", TextureWrap::MirroredRepeat).description("Wrap texture by repeating the mirrored texels");
        builder.field("ClampToEdge", TextureWrap::ClampToEdge).description("Wrap texture by clamping to the texture edges");
        return builder.build();
    }
};

template<>
struct JsonObjectReflector<Texture2DSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<Texture2DSettings>();
        builder.field("filter", [](auto &object) { return &object.filter; })
            .description("How to sample the texture")
            .defaultValue(TextureFilter::Linear);
        builder.field("wrap", [](auto &object) { return &object.wrap; }).description("How to wrap the texture").defaultValue(TextureWrap::Repeat);
        return builder.build();
    }
};

struct Texture2DParams
{
    TextureFormat format;
    Size2 size;
    Texture2DSettings value;
};

template<>
struct JsonObjectReflector<Texture2DParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<Texture2DParams>();
        builder.field("format", [](auto &object) { return &object.format; }).description("Texture format");
        builder.field("size", [](auto &object) { return &object.size; }).description("Texture size XY");
        builder.extend([](auto &object) { return &object.value; });
        return builder.build();
    }
};

struct Texture2DStorage
{
    TextureData2D data;
    Texture2DSettings settings;
};

using CreateTexture2DParams = Params<CreateParamsOf<Texture2DParams>>;
using GetTexture2DResult = GetResultOf<Texture2DParams>;
using UpdateTexture2DParams = UpdateParamsOf<Texture2DSettings>;
using UserTexture2D = UserTextureOf<Texture2DStorage, Texture2D>;

CreateObjectResult createTexture2D(ObjectManager &objects, Device &device, CreateTexture2DParams params);
GetTexture2DResult getTexture2D(ObjectManager &objects, const GetObjectParams &params);
void updateTexture2D(ObjectManager &objects, Device &device, const UpdateTexture2DParams &params);

struct VolumeTextureParams
{
    ObjectId volume;
    ObjectId transferFunction;
};

template<>
struct JsonObjectReflector<VolumeTextureParams>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<VolumeTextureParams>();
        builder.field("volume", [](auto &object) { return &object.volume; }).description("Volume to use as texture data");
        builder.field("transferFunction", [](auto &object) { return &object.transferFunction; })
            .description("Transfer function to convert volume samples to colors");
        return builder.build();
    }
};

struct VolumeTextureStorage
{
    Stored<UserVolume> volume;
    Stored<UserTransferFunction> transferFunction;
};

using CreateVolumeTextureParams = CreateParamsOf<VolumeTextureParams>;
using GetVolumeTextureResult = GetResultOf<VolumeTextureParams>;
using UpdateVolumeTextureParams = UpdateParamsOf<VolumeTextureParams>;
using UserVolumeTexture = UserTextureOf<VolumeTextureStorage, VolumeTexture>;

CreateObjectResult createVolumeTexture(ObjectManager &objects, Device &device, const CreateVolumeTextureParams &params);
GetVolumeTextureResult getVolumeTexture(ObjectManager &objects, const GetObjectParams &params);
void updateVolumeTexture(ObjectManager &objects, Device &device, const UpdateVolumeTextureParams &params);
}
