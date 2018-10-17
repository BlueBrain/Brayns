/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Juan Hernando <juan.hernando@epfl.ch>
 *
 * This file is part of https://github.com/BlueBrain/ospray-modules
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

#include "Texture2D.h"

#include "../Context.h"

namespace bbp
{
namespace optix
{
std::string Texture2D::toString() const
{
    return "optix::texture2d";
}

void Texture2D::commit()
{
    auto size = getParam<ospray::vec2i>("size", ospray::vec2i(-1, -1));

    const uint16_t nx = size.x;
    const uint16_t ny = size.y;

    uint16_t channels = 3;
    const auto type = static_cast<OSPTextureFormat>(
        getParam1i("type", OSP_TEXTURE_FORMAT_INVALID));
    switch (type)
    {
    case OSP_TEXTURE_RGBA8:
    case OSP_TEXTURE_SRGBA:
    case OSP_TEXTURE_RGBA32F:
        channels = 4;
        break;
    default:
        channels = 3;
    }
    const uint16_t optixChannels = 4;

    _sampler->setWrapMode(0, RT_WRAP_REPEAT);
    _sampler->setWrapMode(1, RT_WRAP_REPEAT);
    _sampler->setWrapMode(2, RT_WRAP_REPEAT);
    _sampler->setIndexingMode(RT_TEXTURE_INDEX_NORMALIZED_COORDINATES);
    _sampler->setReadMode(RT_TEXTURE_READ_NORMALIZED_FLOAT);
    _sampler->setMaxAnisotropy(1.0f);
    _sampler->setMipLevelCount(1u);
    _sampler->setArraySize(1u);
    _sampler->setFilteringModes(RT_FILTER_LINEAR, RT_FILTER_LINEAR,
                                RT_FILTER_NONE);

    // Create buffer and populate with texture data
    auto context = Context::get().getOptixContext();
    ::optix::Buffer buffer =
        context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT4, nx, ny);

    auto dataObj = getParamData("data", nullptr);
    if (!dataObj->data)
        throw std::runtime_error("no texel data provided to Texture2D");

    float* bufferData = static_cast<float*>(buffer->map());
    size_t sourceIdx = 0;
    size_t destIdx = 0;
    for (uint16_t y = 0; y < ny; ++y)
        for (uint16_t x = 0; x < nx; ++x)
        {
            switch (type)
            {
            case OSP_TEXTURE_RGBA32F:
            case OSP_TEXTURE_RGB32F:
            case OSP_TEXTURE_R32F:
            {
                auto data = (float*)dataObj->data;
                bufferData[destIdx + 0] = data[sourceIdx + 2];
                bufferData[destIdx + 1] = data[sourceIdx + 1];
                bufferData[destIdx + 2] = data[sourceIdx + 0];
                bufferData[destIdx + 3] =
                    (channels == optixChannels) ? data[sourceIdx + 4] : 1.f;
            }
            break;
            default:
            {
                auto data = (unsigned char*)dataObj->data;
                bufferData[destIdx + 0] = float(data[sourceIdx + 2]) / 255.f;
                bufferData[destIdx + 1] = float(data[sourceIdx + 1]) / 255.f;
                bufferData[destIdx + 2] = float(data[sourceIdx + 0]) / 255.f;
                bufferData[destIdx + 3] =
                    (channels == optixChannels)
                        ? float(data[sourceIdx + 4]) / 255.f
                        : 1.f;
            }
            }
            destIdx += 4;
            sourceIdx += (channels == optixChannels) ? 4 : 3;
        }

    buffer->unmap();

    // Assign buffer to sampler
    _sampler->setBuffer(buffer);
}
}
}
