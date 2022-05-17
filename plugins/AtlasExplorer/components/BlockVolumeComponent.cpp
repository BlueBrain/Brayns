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

#include "BlockVolumeComponent.h"

#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/GeometricModelHandler.h>
#include <brayns/engine/components/MaterialComponent.h>

namespace
{
class VolumeVisibleElementCounter
{
public:
    static size_t count(const std::vector<brayns::Vector4f> &colors)
    {
        size_t result = 0;
        for (const auto &color : colors)
        {
            if (color.a > 0.f)
            {
                ++result;
            }
        }
        return result;
    }
};

class SparseBlockVolumeBuilder
{
public:
    struct SparseBlockVolume
    {
        std::vector<brayns::Box> blocks;
        std::vector<brayns::Vector4f> colors;
    };

    static SparseBlockVolume build(
        const brayns::Vector3ui &sizes,
        const brayns::Vector3f &dimensions,
        const std::vector<brayns::Vector4f> &srcColors)
    {
        const auto width = sizes.x;
        const auto height = sizes.y;
        const auto depth = sizes.z;
        const auto frameSize = width * height;

        const auto linealSize = width * height * depth;

        const auto visibleElementCount = VolumeVisibleElementCounter::count(srcColors);
        auto result = SparseBlockVolume();

        auto &blocks = result.blocks;
        blocks.reserve(visibleElementCount);

        auto &colors = result.colors;
        colors.reserve(visibleElementCount);

        for (size_t i = 0; i < linealSize; ++i)
        {
            const auto &srcColor = srcColors[i];
            if (srcColor.a <= 0.f)
            {
                continue;
            }

            const auto z = i / frameSize;
            const auto localFrame = i % frameSize;
            const auto y = localFrame / width;
            const auto x = localFrame % width;

            blocks.push_back(_createVoxel(dimensions, x, y, z));
            colors.push_back(srcColor);
        }

        return result;
    }

private:
    static brayns::Box _createVoxel(const brayns::Vector3f &dimensions, size_t x, size_t y, size_t z)
    {
        // Bottom front left corner
        const auto worldX = x * dimensions.x;
        const auto worldY = y * dimensions.y;
        const auto worldZ = z * dimensions.z;

        brayns::Box voxel;
        // Bottom back left corner
        voxel.min = {x, y, z - dimensions.z};
        voxel.max = {x + dimensions.x, y + dimensions.y, z};

        return voxel;
    }
};
}

BlockVolumeComponent::BlockVolumeComponent(
    const brayns::Vector3ui &sizes,
    const brayns::Vector3f &dimensions,
    std::vector<brayns::Vector4f> colors)
{
    const auto linealSize = glm::compMul(sizes);
    if (linealSize != _colors.size())
    {
        throw std::invalid_argument("Size and color count is different");
    }

    auto volumeStructure = SparseBlockVolumeBuilder::build(sizes, dimensions, colors);

    _geometry.set(std::move(volumeStructure.blocks));
    _colors = std::move(volumeStructure.colors);
}

brayns::Bounds BlockVolumeComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
    return _geometry.computeBounds(transform);
}

void BlockVolumeComponent::onCreate()
{
    _model = brayns::GeometricModelHandler::create();

    auto &group = getModel();
    brayns::GeometricModelHandler::addToGeometryGroup(_model, group);

    group.addComponent<brayns::MaterialComponent>();

    _geometry.commit();
    brayns::GeometricModelHandler::setGeometry(_model, _geometry);

    auto sharedColors = brayns::DataHandler::shareBuffer(_colors, OSPDataType::OSP_VEC4F);
    brayns::GeometricModelHandler::setColors(_model, sharedColors);

    brayns::GeometricModelHandler::commitModel(_model);
}

bool BlockVolumeComponent::commit()
{
    auto &material = brayns::ExtractModelObject::extractMaterial(getModel());
    if (material.isModified())
    {
        material.commit();
        brayns::GeometricModelHandler::setMaterial(_model, material);
        brayns::GeometricModelHandler::commitModel(_model);
        return true;
    }

    return false;
}

void BlockVolumeComponent::onDestroy()
{
    brayns::GeometricModelHandler::removeFromGeometryGroup(_model, getModel());
    brayns::GeometricModelHandler::destroy(_model);
}
