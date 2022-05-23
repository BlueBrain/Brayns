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

#include <api/kinds/common/DataFlipper.h>
#include <api/kinds/common/VolumeMeasures.h>
#include <components/BlockVolumeComponent.h>

#include <cstring>

namespace
{
class ColorSpaceConverter
{
public:
    static std::vector<brayns::Vector4f> fromDataToRGBA(NRRDKind kind, const std::vector<float> &data)
    {
        if (kind == NRRDKind::GRADIENT3 || kind == NRRDKind::RGBCOLOR)
        {
            return _fromGenericColorToRGBA(data, 3);
        }

        if (kind == NRRDKind::XYZCOLOR)
        {
            return _fromXYZtoRGBA(data);
        }

        if (kind == NRRDKind::HSVCOLOR)
        {
            return _fromHSVtoRGBA(data);
        }

        return _fromGenericColorToRGBA(data, 4);
    }

private:
    static std::vector<brayns::Vector4f> _fromXYZtoRGBA(const std::vector<float> &xyz)
    {
        std::vector<brayns::Vector4f> rgba;
        const auto numElement = _extractNumberOfElements(xyz.size(), 3);
        rgba.reserve(numElement);

        const auto converter = brayns::Matrix3f(
            {3.2404542f, -0.9692660f, 0.0556434f},
            {-1.5371385f, 1.8760108f, -0.2040259},
            {-0.4985314f, 0.0415560f, 1.0572252f});

        for (size_t i = 0; i < numElement; ++i)
        {
            const auto j = i * 3;
            const auto convertedXYZ = converter * brayns::Vector3f(xyz[j], xyz[j + 1], xyz[j + 2]);
            auto &color = rgba.emplace_back();
            color = brayns::Vector4f(glm::max(brayns::Vector3f(0.f), convertedXYZ), 1.f);
        }

        return rgba;
    }

    static std::vector<brayns::Vector4f> _fromHSVtoRGBA(const std::vector<float> &hsv)
    {
        std::vector<brayns::Vector4f> rgba;
        const auto numElement = _extractNumberOfElements(hsv.size(), 3);
        rgba.reserve(numElement);

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

        for (size_t i = 0; i < numElement; ++i)
        {
            const auto j = i * 3;
            const auto H = hsv[j];
            const auto S = hsv[j + 1];
            const auto V = hsv[j + 2];

            auto &color = rgba.emplace_back();
            color.r = converter(5.f, H, S, V);
            color.g = converter(3.f, H, S, V);
            color.b = converter(1.f, H, S, V);
            color.a = 1.f;
        }

        return rgba;
    }

    static std::vector<brayns::Vector4f> _fromGenericColorToRGBA(const std::vector<float> &data, size_t numChannels)
    {
        assert(numChannels <= 4);

        std::vector<brayns::Vector4f> rgba;
        const auto numElements = _extractNumberOfElements(data.size(), numChannels);
        rgba.reserve(numElements);

        constexpr auto converter = 1.f / 255.f;

        for (size_t i = 0; i < numElements; ++i)
        {
            auto &color = rgba.emplace_back(1.f);
            const auto dataIndex = i * numChannels;
            for (size_t j = 0; j < numChannels; ++j)
            {
                color[j] = data[dataIndex + j] * converter;
            }
        }

        return rgba;
    }

    static size_t _extractNumberOfElements(size_t dataSize, size_t elementDimension)
    {
        auto numElements = dataSize / elementDimension;
        assert(numElements * elementDimension == dataSize);
        return numElements;
    }
};
}

void ColorKind::initialize(const NRRDImage &image, brayns::Model &model) const
{
    const auto &data = image.getData();
    const auto &header = image.getHeader();
    const auto floatData = data.asFloats();
    const auto &kinds = *header.kinds;
    const auto colorKind = kinds[0];
    const auto measures = VolumeMeasuresComputer::compute(header, 1);
    auto dataColors = ColorSpaceConverter::fromDataToRGBA(colorKind, floatData);
    const auto colors = DataFlipper::flipVertically(measures.sizes, std::move(dataColors));
    model.addComponent<BlockVolumeComponent>(measures.sizes, measures.dimensions, colors);
}

void ColorKind::handleUseCase(const NRRDImage &image, const UseCaseInfo &info, brayns::Model &model) const
{
    (void)image;
    (void)info;
    (void)model;
}
