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

#include <brayns/engine/Geometry.h>
#include <brayns/engine/geometries/Sphere.h>
#include <brayns/engine/models/GeometricModel.h>

namespace brayns
{
class XYZBModel : public GeometricModel
{
public:
    XYZBModel(const std::vector<Sphere>& spheres);

    Bounds computeBounds(const Matrix4f& transform) const noexcept final;

    void setColor(const Vector4f& color) noexcept;

    const Vector4f& getColor() const noexcept;

    void setRadius(const float newRadius) noexcept;

private:
    void commitGeometryModel() final;

private:
    Geometry<Sphere> _geometry;
    Vector4f _color;
};
}
