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

#include <brayns/common/material/Texture2D.h>
#include <brayns/common/types.h>

#include <pbrt/core/mipmap.h>
#include <pbrt/core/spectrum.h>
#include <pbrt/core/texture.h>

namespace brayns
{
// From PBRT src/textures/imagemap.h
// Concrete implementation. Allows only for RGB Textures
template <typename Return>
class PBRTTextureImpl : public pbrt::Texture<Return>
{
public:
    PBRTTextureImpl(std::unique_ptr<pbrt::TextureMapping2D> mapping,
                    const Texture2D& srcTexture,
                    pbrt::ImageWrap warpMethod = pbrt::ImageWrap::Repeat,
                    bool doTrilinearFilter = true,
                    pbrt::Float maxAnisotropicFilter = 8.f,
                    pbrt::Float scale = 1.f, bool gamma = true);
    ~PBRTTextureImpl() {}

    Return Evaluate(const pbrt::SurfaceInteraction& si) const
    {
        pbrt::Vector2f dstdx, dstdy;
        auto st = _mapping->Map(si, &dstdx, &dstdy);
        return _mipMap->Lookup(st, dstdx, dstdy);
    }

private:
    void _convertData(const uint8_t* data);

    std::unique_ptr<pbrt::TextureMapping2D> _mapping;
    std::unique_ptr<pbrt::MIPMap<Return>> _mipMap;
};

template <>
PBRTTextureImpl<pbrt::RGBSpectrum>::PBRTTextureImpl(
    std::unique_ptr<pbrt::TextureMapping2D> mapping,
    const Texture2D& srcTexture, pbrt::ImageWrap warpMethod,
    bool doTrilinearFilter, pbrt::Float maxAnisotropicFilter, pbrt::Float scale,
    bool gamma)
    : _mapping(std::move(mapping))
{
    // We need at least 3 channels
    if (srcTexture.channels < 3)
        throw std::runtime_error(
            "PBRTTextureImpl: "
            "Cannot create RGBSpectrum texture of a "
            "texture with less than 3 channels");

    const auto resx = srcTexture.width;
    const auto resy = srcTexture.height;
    const auto data = srcTexture.getRawData<uint8_t>();

    std::unique_ptr<pbrt::RGBSpectrum> converted(
        new pbrt::RGBSpectrum[resx * resy]);

    const uint32_t size = resx * resy;
    const pbrt::Float div = static_cast<pbrt::Float>(1.f / 255.f);
    for (uint32_t i = 0; i < size; ++i)
    {
        const uint32_t idx = i * srcTexture.channels;
        // Byte to normalized float
        pbrt::Float srcVals[] = {static_cast<float>(data[idx]) * div,
                                 static_cast<float>(data[idx + 1]) * div,
                                 static_cast<float>(data[idx + 2]) * div};
        // scale + gamma correct
        pbrt::RGBSpectrum& result = converted.get()[i];
        result[0] = scale * (gamma ? pbrt::InverseGammaCorrect(srcVals[0])
                                   : srcVals[0]);
        result[1] = scale * (gamma ? pbrt::InverseGammaCorrect(srcVals[1])
                                   : srcVals[1]);
        result[2] = scale * (gamma ? pbrt::InverseGammaCorrect(srcVals[2])
                                   : srcVals[2]);
    }
    // Create mip mapping
    pbrt::Point2i pbrtRes(static_cast<int>(resx), static_cast<int>(resy));
    _mipMap = std::unique_ptr<pbrt::MIPMap<pbrt::RGBSpectrum>>(
        new pbrt::MIPMap<pbrt::RGBSpectrum>(pbrtRes, converted.get(),
                                            doTrilinearFilter,
                                            maxAnisotropicFilter, warpMethod));
}

template <>
PBRTTextureImpl<pbrt::Float>::PBRTTextureImpl(
    std::unique_ptr<pbrt::TextureMapping2D> mapping,
    const Texture2D& srcTexture, pbrt::ImageWrap warpMethod,
    bool doTrilinearFilter, pbrt::Float maxAnisotropicFilter, pbrt::Float scale,
    bool gamma)
    : _mapping(std::move(mapping))
{
    const auto resx = srcTexture.width;
    const auto resy = srcTexture.height;
    const auto data = srcTexture.getRawData<uint8_t>();

    std::unique_ptr<pbrt::Float> converted(new pbrt::Float[resx * resy]);

    const uint32_t size = resx * resy;
    const pbrt::Float div = static_cast<pbrt::Float>(1.f / 255.f);
    for (uint32_t i = 0; i < size; ++i)
    {
        const uint32_t idx = i * srcTexture.channels;
        // Byte to normalized float
        pbrt::Float srcVal = static_cast<float>(data[idx]) * div;
        // scale + gamma correct
        converted.get()[i] =
            scale * (gamma ? pbrt::InverseGammaCorrect(srcVal) : srcVal);
    }
    // Create mip mapping
    pbrt::Point2i pbrtRes(static_cast<int>(resx), static_cast<int>(resy));
    _mipMap = std::unique_ptr<pbrt::MIPMap<pbrt::Float>>(
        new pbrt::MIPMap<pbrt::Float>(pbrtRes, converted.get(),
                                      doTrilinearFilter, maxAnisotropicFilter,
                                      warpMethod));
}

} // namespace brayns
