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

#include "ScalarVolume.h"

#include <brayns/engine/components/VolumeRendererComponent.h>
#include <brayns/engine/volumes/RegularVolume.h>

#include <api/usecases/OutlineShell.h>

namespace
{
class DataMinMax
{
public:
    template<typename T>
    static std::pair<T, T> compute(const std::vector<T> &input)
    {
        T min = std::numeric_limits<T>::max();
        T max = std::numeric_limits<T>::lowest();

        for (size_t i = 0; i < data.size(); ++i)
        {
            min = std::min(data[i], min);
            max = std::max(data[i], max);
        }

        return std::make_pair(min, max);
    }
};

class DataToBytes
{
public:
    template<typename T>
    static std::vector<uint8_t> convert(const std::vector<T> &input)
    {
        std::vector<uint8_t> result(input.size() * sizeof(T));
        const auto src = input.data();
        auto dst = result.data();
        std::memcpy(dst, src, result.size());
        return result;
    }
};

class OutlineShellBuiler
{
public:
    static void build(brayns::Model &model, const ScalarVolume &volume)
    {
        const auto &data = volume.getData();
        const auto doubles = data.asDoubles();

        OutlineShellData outlineData;
        outlineData.data = _convert(doubles);
        outlineData.gridSize = volume.getGridSize();
        outlineData.gridSpacing = volume.getGridSpacing();
        OutlineShell::generate(model, std::move(outlineData));
    }

private:
    static std::vector<uint8_t> _convert(const std::vector<double> &data)
    {
        const auto minMax = DataMinMax::compute(data);

        std::vector<uint8_t> result(data.size(), 0u);
        for (size_t i = 0; i < data.size(); ++i)
        {
            if (data[i] == minMax.first)
            {
                continue;
            }

            result[i] = 255;
        }

        return result;
    }
};

class DensityVolumeBuilder
{
public:
    static void build(brayns::Model &model, const ScalarVolume &volume)
    {
        const auto &data = volume.getData();
        // const auto dataType = _getBestDataType(data);
    }

private:
    struct ProcessedData
    {
        brayns::VolumeDataType type;
        std::vector<uint8_t> bytes;
        brayns::Vector2f minMax;
    };

    static ProcessedData _processData(const IDataMangler &data)
    {
        const auto baseType = data.getBaseType();
        if (baseType == typeid(char) || baseType == typeid(short))
        {
            const auto shorts = data.asShorts();
            return _buildProcessedData(brayns::VolumeDataType::SHORT, shorts);
        }

        if (baseType == typeid(uint8_t))
        {
            const auto bytes = data.asBytes();
            return _buildProcessedData(brayns::VolumeDataType::UNSIGNED_CHAR, bytes);
        }
        if (baseType == typeid(uint16_t))
        {
            const auto unsignedShorts = data.asUnsingedShorts();
            return _buildProcessedData(brayns::VolumeDataType::UNSIGNED_SHORT, unsignedShorts);
        }
        if (baseType == typeid(float))
        {
            const auto floats = data.asFloats();
            return _buildProcessedData(brayns::VolumeDataType::FLOAT, floats);
        }

        return _buildProcessedData(brayns::VolumeDataType::DOUBLE, data.asDoubles());
    }

    template<typename T>
    static ProcessedData _buildProcessedData(brayns::VolumeDataType type, const std::vector<T> &values)
    {
        const auto minMax = DataMinMax::compute(values);

        ProcessedData result;
        result.type = type;
        result.bytes = DataToBytes::convert(values);
        result.minMax = _castMinMax(minMax);
        return result;
    }

    template<typename T>
    static brayns::Vector2f _castMinMax(const std::pair<T, T> &input)
    {
        const auto maxFlt = std::numeric_limits<float>::max();
        const auto minFlt = std::numeric_limits<float>::min();
        if (input.first < minFlot || input.second > maxFlt)
        {
            throw std::runtime_error("Cannot handle volume values");
        }

        return {static_cast<float>(input.first), static_cast<float>(input.second)};
    }
};
}
}

ScalarVolume::ScalarVolume(
    const brayns::Vector3ui &size,
    const brayns::Vector3f &spacing,
    std::unique_ptr<IDataMangler> data)
    : _gridSize(size)
    , _gridSpacing(spacing)
    , _data(std::move(data))
{
}

void ScalarVolume::handleUseCase(VisualizationUseCase useCase, brayns::Model &model) const
{
    if (useCase == VisualizationUseCase::OUTLINE_SHELL)
    {
        OutlineShellBuiler::build(model, *this);
        return;
    }

    if (useCase == VisualizationUseCase::DENSITY)
    {
        DensityVolumeBuilder::build(model, *this);
        return;
    }

    throw std::runtime_error("Scalar volume cannot handle " + brayns::enumToString(useCase));
}

const brayns::Vector3ui &ScalarVolume::getGridSize() const noexcept
{
    return _gridSize;
}

const brayns::Vector3f &ScalarVolume::getGridSpacing() const noexcept
{
    return _gridSpacing
}

const IDataMangler &ScalarVolume::getData() const noexcept
{
    return *_data;
}
