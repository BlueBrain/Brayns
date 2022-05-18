/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "ColorKind.h"

#include <cstring>

namespace
{
class ColorSpaceConverter
{
public:
    static std::vector<brayns::Vector4f> fromXYZtoRGB(const std::vector<float> &xyz)
    {
        std::vector<brayns::Vector4f> rgb;
        rgb.reserve(xyz.size() / 3);
        assert(xyz.size() * 3 == rgb.size());

        const auto converter = brayns::Matrix3f(
            {3.2404542f, -0.9692660f, 0.0556434f},
            {-1.5371385f, 1.8760108f, -0.2040259},
            {-0.4985314f, 0.0415560f, 1.0572252f});

        for (size_t i = 0; i < rgb.size(); ++i)
        {
            const auto j = i * 3;
            const auto convertedXYZ = converter * brayns::Vector3f(xyz[j], xyz[j + 1], xyz[j + 2]);
            auto &color = rgb.emplace_back();
            color = brayns::Vector4f(glm::max(brayns::Vector3f(0.f), convertedXYZ), 1.f);
        }

        return rgb;
    }

    static std::vector<brayns::Vector4f> fromHSVtoRGB(const std::vector<float> &hsv)
    {
        std::vector<brayns::Vector4f> rgb;
        rgb.reserve(hsv.size() / 3);
        assert(hsv.size() * 3 == rgb.size());

        const auto floatModulo = [](float value, int32_t dividend)
        {
            const auto decimals = value - std::floor(value);
            const auto intModule = static_cast<int32_t>(value) % dividend;
            return intModule + decimals;
        };
        const auto k = [&floatModulo](float n, float H) { return floatModulo(n + H / 60.f, 6); };
        const auto converter = [&k](float n, float H, float V, float S)
        {
            const auto kv = k(n, H);
            const auto multiplier = std::min<float>(kv, std::min(4.f - kv, 1.f));
            return V - V * S * std::max(0.f, multiplier);
        };

        for (size_t i = 0; i < rgb.size(); ++i)
        {
            const auto j = i * 3;
            const auto H = hsv[j];
            const auto S = hsv[j + 1];
            const auto V = hsv[j + 2];

            auto &color = rgb.emplace_back();
            color.r = converter(5.f, H, S, V);
            color.g = converter(3.f, H, S, V);
            color.b = converter(1.f, H, S, V);
            color.a = 1.f;
        }

        return rgb;
    }
};

class ColorBuilder
{
public:
    static std::vector<brayns::Vector4f> build(const std::vector<float> &data, size_t numChannels)
    {
        const auto numColors = data.size() / numChannels;
        std::vector<brayns::Vector4f> result;
        result.reserve(numColors);

        for (size_t i = 0; i < data.size(); i = i + numChannels)
        {
            auto &color = result.emplace_back(1.f);
            for (size_t j = 0; j < numChannels; ++j)
            {
                color[j] = data[i + j];
            }
        }

        return result;
    }
};
}

void ColorKind::createComponent(const NRRDHeader &header, const INRRDData &data, brayns::Model &model) const
{
    auto floatData = data.asFloats();

    const auto dimensions = header.dimensions;
    const auto &sizes = header.sizes;
    const auto &kinds = *header.kinds;
    const auto colorKind = kinds[0];
}
