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

#include <pbrtv2/core/mipmap.h>
#include <pbrtv2/core/spectrum.h>
#include <pbrtv2/core/texture.h>

#include <cmath>

namespace brayns
{
// From PBRT src/textures/imagemap.h
// Concrete implementation. Allows only for RGB Textures

template <typename Return>
class PBRTTextureImpl : public pbrt::Texture<Return>
{
public:
    PBRTTextureImpl(
        pbrt::TextureMapping2D* mapping, const Texture2D& srcTexture,
        pbrt::ImageWrap warpMethod = pbrt::ImageWrap::TEXTURE_REPEAT,
        const bool doTrilinearFilter = true,
        const float maxAnisotropicFilter = 8.f, const float scale = 1.f,
        const float gamma = 1.f);
    ~PBRTTextureImpl()
    {
        delete _mapping;
        delete _mipMap;
    }

    Return Evaluate(const pbrt::DifferentialGeometry& dg) const
    {
        float s, t, dsdx, dtdx, dsdy, dtdy;
        _mapping->Map(dg, &s, &t, &dsdx, &dtdx, &dsdy, &dtdy);
        return _mipMap->Lookup(s, t, dsdx, dtdx, dsdy, dtdy);
    }

private:
    void _convertData(const uint8_t* data);

    pbrt::TextureMapping2D* _mapping;
    pbrt::MIPMap<Return>* _mipMap;
};

template <>
PBRTTextureImpl<pbrt::RGBSpectrum>::PBRTTextureImpl(
    pbrt::TextureMapping2D* mapping, const Texture2D& srcTexture,
    pbrt::ImageWrap warpMethod, bool doTrilinearFilter,
    const float maxAnisotropicFilter, const float scale, const float gamma)
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

    std::vector<pbrt::RGBSpectrum> converted(resx * resy);

    const uint32_t size = resx * resy;
    const float div = 1.f / 255.f;
    for (uint32_t i = 0; i < size; ++i)
    {
        const uint32_t idx = i * srcTexture.channels;
        // Byte to normalized float
        float srcVals[] = {data[idx] * div, data[idx + 1] * div,
                           data[idx + 2] * div};
        // scale + gamma correct
        pbrt::RGBSpectrum& result = converted[i];
        const float rbgVals[3] = {
            (gamma != 1.f ? powf(scale * srcVals[0], gamma)
                          : scale * srcVals[0]),
            (gamma != 1.f ? powf(scale * srcVals[1], gamma)
                          : scale * srcVals[1]),
            (gamma != 1.f ? powf(scale * srcVals[2], gamma)
                          : scale * srcVals[2])};
        result = pbrt::RGBSpectrum::FromRGB(rbgVals);
    }
    // Create mip mapping
    _mipMap =
        new pbrt::MIPMap<pbrt::RGBSpectrum>(static_cast<int>(resx),
                                            static_cast<int>(resy),
                                            converted.data(), doTrilinearFilter,
                                            maxAnisotropicFilter, warpMethod);
}

template <>
PBRTTextureImpl<pbrt::SampledSpectrum>::PBRTTextureImpl(
    pbrt::TextureMapping2D* mapping, const Texture2D& srcTexture,
    pbrt::ImageWrap warpMethod, bool doTrilinearFilter,
    const float maxAnisotropicFilter, const float scale, const float gamma)
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

    std::vector<pbrt::SampledSpectrum> converted(resx * resy);

    const uint32_t size = resx * resy;
    const float div = 1.f / 255.f;
    for (uint32_t i = 0; i < size; ++i)
    {
        const uint32_t idx = i * srcTexture.channels;
        // Byte to normalized float
        float srcVals[] = {data[idx] * div, data[idx + 1] * div,
                           data[idx + 2] * div};
        // scale + gamma correct
        auto& result = converted[i];
        const float rbgVals[3] = {
            (gamma != 1.f ? powf(scale * srcVals[0], gamma)
                          : scale * srcVals[0]),
            (gamma != 1.f ? powf(scale * srcVals[1], gamma)
                          : scale * srcVals[1]),
            (gamma != 1.f ? powf(scale * srcVals[2], gamma)
                          : scale * srcVals[2])};
        result = pbrt::SampledSpectrum::FromRGB(rbgVals);
    }
    // Create mip mapping
    _mipMap = new pbrt::MIPMap<pbrt::SampledSpectrum>(
        static_cast<int>(resx), static_cast<int>(resy), converted.data(),
        doTrilinearFilter, maxAnisotropicFilter, warpMethod);
}

template <>
PBRTTextureImpl<float>::PBRTTextureImpl(pbrt::TextureMapping2D* mapping,
                                        const Texture2D& srcTexture,
                                        pbrt::ImageWrap warpMethod,
                                        bool doTrilinearFilter,
                                        const float maxAnisotropicFilter,
                                        const float scale, const float gamma)
    : _mapping(std::move(mapping))
{
    const auto resx = srcTexture.width;
    const auto resy = srcTexture.height;
    const auto data = srcTexture.getRawData<uint8_t>();

    std::vector<float> converted(resx * resy);

    const uint32_t size = resx * resy;
    const float div = 1.f / 255.f;
    for (uint32_t i = 0; i < size; ++i)
    {
        const uint32_t idx = i * srcTexture.channels;
        // Byte to normalized float
        float srcVal = static_cast<float>(data[idx]) * div;
        // scale + gamma correct
        converted[i] =
            gamma != 1.f ? powf(scale * srcVal, gamma) : scale * srcVal;
    }
    // Create mip mapping
    // pbrt::Point2i pbrtRes (static_cast<int>(resx), static_cast<int>(resy));
    _mipMap =
        new pbrt::MIPMap<float>(static_cast<int>(resx), static_cast<int>(resy),
                                converted.data(), doTrilinearFilter,
                                maxAnisotropicFilter, warpMethod);
}

} // namespace brayns
