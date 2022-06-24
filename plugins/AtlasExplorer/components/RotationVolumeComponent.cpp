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
#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/MaterialComponent.h>

#include <cmath>

namespace
{
class QuaternionVerifier
{
public:
    static bool isValid(const brayns::Quaternion &quaternion)
    {
        size_t zeroComponents = 0;
        for (size_t i = 0; i < 4; ++i)
        {
            if (!std::isfinite(quaternion[i]))
            {
                return false;
            }

            if (quaternion[i] == 0.f)
            {
                ++zeroComponents;
            }
        }
        return zeroComponents < 4;
    }
};

class QuaternionVolumeVerifier
{
public:
    static std::vector<size_t> computeValidIndices(const std::vector<brayns::Quaternion> &quaternions)
    {
        std::vector<size_t> result;
        result.reserve(quaternions.size());

        for (size_t i = 0; i < quaternions.size(); ++i)
        {
            if (!QuaternionVerifier::isValid(quaternions[i]))
            {
                continue;
            }

            result.push_back(i);
        }

        return result;
    }
};

struct RenderableAxisGeometry
{
    brayns::Vector3f vector = brayns::Vector3f(0.f);
    std::vector<brayns::Primitive> geometry;
};

class SparseRotationVolumeBuilder
{
public:
    static std::vector<RenderableAxisList> build(
        const brayns::Vector3ui &sizes,
        const brayns::Vector3f &dimensions,
        const std::vector<brayns::Quaternion> &rotations)
    {
        const auto minDimension = glm::compMin(dimensions);
        const float radius = minDimension * 0.05f;

        const auto validQuaternions = QuaternionVolumeVerifier::computeValidIndices(rotations);
        auto result = _allocateTemporary(validQuaternions.size());

        const auto width = sizes.x;
        const auto height = sizes.y;
        const auto frameSize = width * height;

#pragma omp parallel for
        for (size_t i = 0; i < validQuaternions.size(); ++i)
        {
            const auto index = validQuaternions[i];
            const auto &srcQuaternion = rotations[index];
            const auto quaternion = glm::normalize(srcQuaternion);

            const auto z = index / frameSize;
            const auto localFrame = index % frameSize;
            const auto y = localFrame / width;
            const auto x = localFrame % width;

            const auto voxelCenter = _computeVoxelCenter(dimensions, x, y, z);
            for (auto &axis : result)
            {
                // * 0.5f so that the axis length does not invade surronding voxels
                const auto vector = (quaternion * axis.vector) * minDimension * 0.5f;
                auto &buffer = axis.geometry;
                buffer[i] = brayns::Primitive::cylinder(voxelCenter, voxelCenter + vector, radius);
            }
        }

        return _buildResult(std::move(result));
    }

private:
    static std::vector<RenderableAxisGeometry> _allocateTemporary(size_t elementCount)
    {
        auto result = std::vector<RenderableAxisGeometry>(3);
        for (size_t i = 0; i < 3; ++i)
        {
            auto &axis = result[i];
            axis.vector[i] = 1.f;
            axis.geometry.resize(elementCount);
        }
        return result;
    }

    static brayns::Vector3f _computeVoxelCenter(const brayns::Vector3f &dimensions, size_t x, size_t y, size_t z)
    {
        // Bottom front left corner
        const auto worldX = x * dimensions.x;
        const auto worldY = y * dimensions.y;
        const auto worldZ = z * dimensions.z;
        const auto minCorner = brayns::Vector3f(worldX, worldY, worldZ - dimensions.z);
        const auto maxCorner = brayns::Vector3f(worldX + dimensions.x, worldY + dimensions.y, worldZ);
        return (maxCorner + minCorner) * 0.5f;
    }

    static std::vector<RenderableAxisList> _buildResult(std::vector<RenderableAxisGeometry> buildData)
    {
        std::vector<RenderableAxisList> axes;
        axes.reserve(buildData.size());
        for (auto &rawAxis : buildData)
        {
            RenderableAxisList renderableAxis{rawAxis.vector, {std::move(rawAxis.geometry)}};
            axes.push_back(std::move(renderableAxis));
        }
        return axes;
    }
};
}

RenderableAxes::RenderableAxes(std::vector<RenderableAxisList> geometry)
    : _geometry(std::move(geometry))
{
}

RotationVolumeComponent::RotationVolumeComponent(
    const brayns::Vector3ui &sizes,
    const brayns::Vector3f &dimensions,
    const std::vector<brayns::Quaternion> &rotations)
    : _axes(SparseRotationVolumeBuilder::build(sizes, dimensions, rotations))
{
}

brayns::Bounds RotationVolumeComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
    brayns::Bounds bounds;
    _axes.forEach(
        [&bounds, &transform](const RenderableAxisList &axis)
        {
            const auto &axisGeometry = axis.geometry;
            auto axisBounds = axisGeometry.computeBounds(transform);
            bounds.expand(axisBounds);
        });
    return bounds;
}

void RotationVolumeComponent::onCreate()
{
    auto &model = getModel();
    model.addComponent<brayns::MaterialComponent>();

    auto &group = model.getGroup();
    _axes.forEach(
        [&](RenderableAxisList &axis)
        {
            auto &geometry = axis.geometry;
            geometry.setColor(axis.vector);
            group.addGeometry(geometry);
        });
}

bool RotationVolumeComponent::commit()
{
    auto &material = brayns::ExtractModelObject::extractMaterial(getModel());
    if (!material.commit())
    {
        return false;
    }

    _axes.forEach(
        [&](RenderableAxisList &axis)
        {
            auto &geometry = axis.geometry;
            geometry.setMaterial(material);
            geometry.commit();
        });

    return true;
}

void RotationVolumeComponent::onDestroy()
{
    auto &model = getModel();
    auto &group = model.getGroup();
    _axes.forEach([&](RenderableAxisList &axis) { group.removeGeometry(axis.geometry); });
}
