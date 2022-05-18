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

#include "VectorVolumeComponent.h"

#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/GeometricModelHandler.h>
#include <brayns/engine/components/MaterialComponent.h>

namespace
{
class VolumeValidElement
{
public:
    static bool isValid(const brayns::Vector3f &vector)
    {
        for (size_t i = 0; i < 3; ++i)
        {
            if (!std::isfinite(vector[i]))
            {
                return false;
            }
        }
        return true;
    }
    static size_t countValid(const std::vector<brayns::Vector3f> &vectors)
    {
        size_t result = 0;
        for (const auto &vector : vectors)
        {
            if (isValid(vector))
            {
                ++result;
            }
        }
        return result;
    }
};

class SparseVectorVolumeBuilder
{
public:
    static std::vector<brayns::Primitive> build(
        const brayns::Vector3ui &sizes,
        const brayns::Vector3f &dimensions,
        const std::vector<brayns::Vector3f> &vectors)
    {
        const auto width = sizes.x;
        const auto height = sizes.y;
        const auto depth = sizes.z;
        const auto frameSize = width * height;

        const auto linealSize = width * height * depth;

        const auto visibleElementCount = VolumeValidElement::countValid(vectors);
        auto result = std::vector<brayns::Primitive>();
        result.reserve(visibleElementCount);

        for (size_t i = 0; i < linealSize; ++i)
        {
            const auto &srcVector = vectors[i];
            if (!VolumeValidElement::isValid(srcVector))
            {
                continue;
            }

            const auto z = i / frameSize;
            const auto localFrame = i % frameSize;
            const auto y = localFrame / width;
            const auto x = localFrame % width;
            const auto voxelCenter = _computeVoxelCenter(dimensions, x, y, z);
            const auto offset = srcVector * 0.5f;

            result.push_back(brayns::Primitive::cylinder(voxelCenter, voxelCenter + offset, 2.f));
        }

        return result;
    }

private:
    static brayns::Vector3f _computeVoxelCenter(const brayns::Vector3f &dimensions, size_t x, size_t y, size_t z)
    {
        // Bottom front left corner
        const auto worldX = x * dimensions.x;
        const auto worldY = y * dimensions.y;
        const auto worldZ = z * dimensions.z;
        const auto min = brayns::Vector3f(x, y, z - dimensions.z);
        const auto max = brayns::Vector3f(x + dimensions.x, y + dimensions.y, z);
        return (max + min) * 0.5f;
    }
};
}

VectorVolumeComponent::VectorVolumeComponent(
    const brayns::Vector3ui &sizes,
    const brayns::Vector3f &dimensions,
    const std::vector<brayns::Vector3f> &vectors)
{
    auto primitives = SparseVectorVolumeBuilder::build(sizes, dimensions, vectors);
    _geometry.set(std::move(primitives));
}

brayns::Bounds VectorVolumeComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
    return _geometry.computeBounds(transform);
}

void VectorVolumeComponent::onCreate()
{
    auto &group = getModel();

    group.addComponent<brayns::MaterialComponent>();

    _geometry.commit();

    _model = brayns::GeometricModelHandler::create();
    brayns::GeometricModelHandler::addToGeometryGroup(_model, group);
    brayns::GeometricModelHandler::setGeometry(_model, _geometry);
    brayns::GeometricModelHandler::commitModel(_model);
}

bool VectorVolumeComponent::commit()
{
    auto &material = brayns::ExtractModelObject::extractMaterial(getModel());
    if (material.isModified())
    {
        material.commit();
        brayns::GeometricModelHandler::setMaterial(_model, material);
        brayns::GeometricModelHandler::setColor(_model, material.getColor());
        brayns::GeometricModelHandler::commitModel(_model);
        return true;
    }
    return false;
}

void VectorVolumeComponent::onDestroy()
{
    brayns::GeometricModelHandler::removeFromGeometryGroup(_model, getModel());
    brayns::GeometricModelHandler::destroy(_model);
}
