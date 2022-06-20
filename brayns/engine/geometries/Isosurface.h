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

#include <brayns/common/MathTypes.h>
#include <brayns/engine/Geometry.h>
#include <brayns/engine/Volume.h>

#include <ospray/ospray_cpp/Data.h>

namespace brayns
{
template<typename T>
struct Isosurface
{
    Volume<T> volume;
    std::vector<float> isovalues;
};

template<typename T>
class OsprayGeometryName<Isosurface<T>>
{
public:
    static std::string_view get()
    {
        return "isosurface";
    }
};

template<typename T>
class GeometryBoundsUpdater<Isosurface<T>>
{
public:
    static void update(const Isosurface<T> &geometry, const Matrix4f &transform, Bounds &bounds)
    {
        const auto &volume = geometry.volume;
        auto volumeBounds = volume.computeBounds(transform);
        bounds.expand(volumeBounds);
    }
};

template<typename T>
class InputGeometryChecker<Isosurface<T>>
{
public:
    static void check(const std::vector<Isosurface<T>> &primitives)
    {
        if (primitives.size() > 1)
        {
            throw std::runtime_error("Geometry<Isosurface<T>> only accepts 1 geometry");
        }
    }
};

template<typename T>
class GeometryCommitter<Isosurface<T>>
{
public:
    static void commit(const ospray::cpp::Geometry &osprayGeometry, const std::vector<Isosurface<T>> &primitives)
    {
        static const std::string volumeParameter = "volume";
        static const std::string isoValueParameter = "isovalue";

        const auto &primitive = primitives.front();
        auto &isoValues = primitive.isovalues;
        auto &volume = primitive.volume;

        osprayGeometry.setParam(volumeParameter, volume.getOsprayVolume());
        osprayGeometry.setParam(isoValueParameter, ospray::cpp::SharedData(isoValues));
    }
};

}
