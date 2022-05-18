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
class VolumeValidElement
{
public:
    static bool isValid(const brayns::Quaternion &quaternion)
    {
        for (size_t i = 0; i < 4; ++i)
        {
            if (!std::isfinite(quaternion[i]))
            {
                return false;
            }
        }
        return true;
    }

    static size_t countValid(const std::vector<brayns::Quaternion> &rotations)
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
        const auto validElementCount = VolumeValidElement::countValid(rotations);
        auto result = _allocateTemporary(validElementCount);

        const auto width = sizes.x;
        const auto height = sizes.y;
        const auto depth = sizes.z;
        const auto frameSize = width * height;
        const auto linealSize = width * height * depth;

        for (size_t i = 0; i < linealSize; ++i)
        {
            const auto &quaternion = rotations[i];
            if (!VolumeValidElement::isValid(quaternion))
            {
                continue;
            }

            const auto z = i / frameSize;
            const auto localFrame = i % frameSize;
            const auto y = localFrame / width;
            const auto x = localFrame % width;

            const auto voxelCenter = _computeVoxelCenter(dimensions, x, y, z);
            for (auto &axis : result)
            {
                const auto vector = axis.vector * 0.5f;
                auto &buffer = axis.geometry;
                buffer.push_back(brayns::Primitive::cylinder(voxelCenter, voxelCenter + vector, 2.f));
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
            axis.geometry.reserve(elementCount);
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

    static std::vector<RenderableAxisList> _buildResult(std::vector<RenderableAxisGeometry> buildData)
    {
        std::vector<RenderableAxisList> axes;
        axes.reserve(buildData.size());
        for (auto &rawAxis : buildData)
        {
            auto &renderableAxis = axes.emplace_back();
            renderableAxis.vector = rawAxis.vector;
            renderableAxis.geometry.set(std::move(rawAxis.geometry));
        }
        return axes;
    }
};
}

RenderableAxes::RenderableAxes(std::vector<RenderableAxisList> geometry)
    : _geometry(std::move(geometry))
{
}

void RenderableAxes::forEach(const std::function<void(RenderableAxisList &)> &callback)
{
    for (auto &axis : _geometry)
    {
        callback(axis);
    }
}

void RenderableAxes::forEach(const std::function<void(const RenderableAxisList &)> &callback) const
{
    for (const auto &axis : _geometry)
    {
        callback(axis);
    }
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
    auto &group = getModel();
    group.addComponent<brayns::MaterialComponent>();

    _axes.forEach(
        [&group](RenderableAxisList &axis)
        {
            auto &model = axis.model;
            auto &geometry = axis.geometry;
            auto &vector = axis.vector;

            geometry.commit();

            model = brayns::GeometricModelHandler::create();
            brayns::GeometricModelHandler::setGeometry(model, geometry);
            brayns::GeometricModelHandler::setColor(model, vector);
            brayns::GeometricModelHandler::commitModel(model);
            brayns::GeometricModelHandler::addToGeometryGroup(axis.model, group);
        });
}

bool RotationVolumeComponent::commit()
{
    auto &material = brayns::ExtractModelObject::extractMaterial(getModel());
    if (!material.isModified())
    {
        return false;
    }

    material.commit();
    _axes.forEach(
        [&material](RenderableAxisList &axis)
        {
            brayns::GeometricModelHandler::setMaterial(axis.model, material);
            brayns::GeometricModelHandler::commitModel(axis.model);
        });

    return true;
}

void RotationVolumeComponent::onDestroy()
{
    auto &group = getModel();
    _axes.forEach(
        [&group](RenderableAxisList &axis)
        {
            brayns::GeometricModelHandler::removeFromGeometryGroup(axis.model, group);
            brayns::GeometricModelHandler::destroy(axis.model);
        });
}
