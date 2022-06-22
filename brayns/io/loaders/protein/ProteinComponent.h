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
#include <brayns/engine/geometry/types/Sphere.h>

namespace brayns
{
class ProteinComponent final : public Component
{
public:
    ProteinComponent(std::vector<Sphere> spheres, std::vector<Vector4f> colors, std::vector<uint8_t> colorIndices);

    Bounds computeBounds(const Matrix4f &transform) const noexcept override;

    void onCreate() override;

    bool commit() override;

    void onDestroy() override;

private:
    GeometryObject<Sphere> _object;
    std::vector<Vector4f> _colors;
    std::vector<uint8_t> _colorIndices;
};
}
