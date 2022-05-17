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

#include "RotationVolumeComponent.h"

#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/GeometricModelHandler.h>
#include <brayns/engine/components/MaterialComponent.h>

#include <cmath>

namespace
{
class ValidRotationChecker
{
public:
    static bool isValid(const brayns::Quaternion &quaternion)
    {
        const auto xValid = std::isfinite(quaternion.x);
        const auto yValid = std::isfinite(quaternion.y);
        const auto zValid = std::isfinite(quaternion.z);
        const auto wValid = std::isfinite(quaternion.w);
        return xValid && yValid && zValid && wValid;
    }

    static size_t count(const std::vector<brayns::Quaternion> &rotations)
    {
        size_t result = 0;
        for (const auto &q : rotations)
        {
            if (isValid(q))
            {
                ++result;
            }
        }
        return result;
    }
};

class RotatedAxisVolumeBuilder
{
public:
    static std::array<std::vector<brayns::Primitive>, 3> build(
        const brayns::Vector3ui &sizes,
        const brayns::Vector3f &dimensions,
        const std::vector<brayns::Quaternion> &rotations)
    {
        const auto validElementCount = ValidRotationChecker::count(rotations);
        auto result = _allocateResult(validElementCount);

        const auto width = sizes.x;
        const auto height = sizes.y;
        const auto depth = sizes.z;
        const auto frameSize = width * height;
        const auto linealSize = width * height * depth;

        for (size_t i = 0; i < linealSize; ++i)
        {
            const auto &quaternion = rotations[i];
            if (!ValidRotationChecker::isValid(quaternion))
            {
                continue;
            }

            const auto z = i / frameSize;
            const auto localFrame = i % frameSize;
            const auto y = localFrame / width;
            const auto x = localFrame % width;

            const auto voxelCenter = _computeVoxelCenter(dimensions, x, y, z);
            for (size_t j = 0; j < 3; ++j)
            {
                auto targetAxis = brayns::Vector3f(0.f);
                targetAxis[j] = dimensions[j];
                auto &buffer = result[j];
                buffer.push_back(_buildAxis(voxelCenter, targetAxis));
            }
        }

        return result;
    }

private:
    static std::array<std::vector<brayns::Primitive>, 3> _allocateResult(size_t elementCount)
    {
        auto result = std::array<std::vector<brayns::Primitive>, 3>();
        for (auto &axis : result)
        {
            axis.reserve(elementCount);
        }
        return result;
    }

    static brayns::Vector3f _computeVoxelCenter(const brayns::Vector3f &dimensions, size_t x, size_t y, size_t z)
    {
        // Bottom front left corner
        const auto worldX = x * dimensions.x;
        const auto worldY = y * dimensions.y;
        const auto worldZ = z * dimensions.z;

        const auto minCorner = brayns::Vector3f(x, y, z - dimensions.z);
        const auto maxCorner = brayns::Vector3f(x + dimensions.x, y + dimensions.y, z);

        return (maxCorner + minCorner) * 0.5f;
    }
    static brayns::Primitive _buildAxis(const brayns::Vector3f &pos, const brayns::Vector3f &axis)
    {
        brayns::Primitive result;
        result.p0 = pos;
        result.r0 = 2.f;
        result.p1 = pos + axis;
        result.r1 = 2.f;
        return result;
    }
};

class RenderableAxisInitializer
{
public:
    static void initialize(RotationVolumeComponent::RenderableAxis &axis, brayns::Model &group)
    {
        auto &model = axis.model;
        auto &geometry = axis.geometry;
        auto &vector = axis.vector;

        geometry.commit();

        model = brayns::GeometricModelHandler::create();
        brayns::GeometricModelHandler::addToGeometryGroup(model, group);
        brayns::GeometricModelHandler::setGeometry(model, geometry);
        brayns::GeometricModelHandler::setColor(model, vector);
        brayns::GeometricModelHandler::commitModel(model);
    }
};
}

RotationVolumeComponent::RotationVolumeComponent(
    const brayns::Vector3ui &sizes,
    const brayns::Vector3f &dimensions,
    std::vector<brayns::Quaternion> &rotations)
{
    if (glm::compMul(sizes) != rotations.size())
    {
        throw std::invalid_argument("Size and rotation count is different");
    }

    auto rawAxes = RotatedAxisVolumeBuilder::build(sizes, dimensions, rotations);
    for (size_t i = 0; i < 3; ++i)
    {
        auto &rawAxisGeometry = rawAxes[i];
        auto &axisGeometry = _axes[i];
        axisGeometry.geometry.set(std::move(rawAxisGeometry));
    }
}

brayns::Bounds RotationVolumeComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
}

void RotationVolumeComponent::onCreate()
{
    auto &group = getModel();
    group.addComponent<brayns::MaterialComponent>();

    _geometry.commit();
    brayns::GeometricModelHandler::setGeometry(_model, _geometry);

    brayns::GeometricModelHandler::commitModel(_model);
}

bool RotationVolumeComponent::commit()
{
}

void RotationVolumeComponent::onDestroy()
{
}
