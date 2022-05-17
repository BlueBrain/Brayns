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

#pragma once

#include <brayns/engine/ModelComponents.h>
#include <brayns/engine/geometries/Box.h>

/**
 * @brief Represents a geometry-made volume, in which each voxel is represented by a cube
 */
class BlockVolumeComponent final : public brayns::Component
{
public:
    /**
     * @brief Construct a new Block Volume Component object
     *
     * @param sizes 3D grid size
     * @param dimensions spatial size for each axis
     * @param colors Color data for each voxel
     * @throws std::invalid_argment if colors.size() does not match sizes.x * sizes.y * sizes.z
     */
    BlockVolumeComponent(
        const brayns::Vector3ui &sizes,
        const brayns::Vector3f &dimensions,
        std::vector<brayns::Vector4f> colors);

    brayns::Bounds computeBounds(const brayns::Matrix4f &transform) const noexcept override;

    void onCreate() override;

    bool commit() override;

    void onDestroy() override;

private:
    OSPGeometricModel _model = nullptr;
    brayns::Geometry<brayns::Box> _geometry;
    std::vector<brayns::Vector4f> _colors;
};
