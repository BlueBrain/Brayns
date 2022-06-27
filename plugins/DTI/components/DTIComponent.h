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
#include <brayns/engine/geometry/GeometryObject.h>
#include <brayns/engine/geometry/types/Primitive.h>

namespace dti
{
class DTIComponent final : public brayns::Component
{
public:
    struct Streamline
    {
        brayns::GeometryObject<brayns::Primitive> geometry;
        std::vector<brayns::Vector4f> colors;
    };

public:
    DTIComponent(std::vector<std::vector<brayns::Primitive>> streamlineGeometries);

    brayns::Bounds computeBounds(const brayns::Matrix4f &transform) const noexcept override;

    void onCreate() override;

    bool commit() override;

    void onDestroy() override;

    size_t getNumStreamlines() const noexcept;

    void setDefaultColors() noexcept;

    void updateSimulation(const std::vector<std::vector<float>> &data);

private:
    void _commitColors() noexcept;

private:
    std::vector<Streamline> _streamlines;
    bool _colorsDirty{false};
};
}
