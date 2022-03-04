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
#include <brayns/engine/ModelComponents.h>
#include <brayns/engine/geometries/Box.h>
#include <brayns/engine/geometries/Plane.h>
#include <brayns/engine/geometries/Primitive.h>
#include <brayns/engine/geometries/Sphere.h>
#include <brayns/engine/geometries/TriangleMesh.h>

namespace brayns
{
template<typename T>
class GeometryComponent final: public Component
{
public:
    GeometryComponent() = default;

    GeometryComponent(const T& geometry)
    {
        _geometry.add(geometry);
    }

    GeometryComponent(const std::vector<T>& geometries)
    {
        _geometry.add(geometries);
    }

    uint64_t getSizeInBytes() const noexcept
    {
        return _geometry.getNumGeometries() * sizeof(T);
    }

    Bounds computeBounds(const Matrix4f& transform) const noexcept override
    {
        return _geometry.computeBounds(transform);
    }

    Geometry<T> &getGeometry() noexcept
    {
        return _geometry;
    }

private:
    Geometry<T> _geometry;
};

using BoxGeometryComponent = GeometryComponent<Box>;
using PlaneGeometryComponent = GeometryComponent<Plane>;
using PrimitiveGeometryComponent = GeometryComponent<Primitive>;
using SphereGeometryComponent = GeometryComponent<Sphere>;
using MeshGeometryComponent = GeometryComponent<TriangleMesh>;
}
