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
#include <brayns/engine/geometry/Geometry.h>
#include <brayns/engine/volume/Volume.h>

#include <ospray/ospray_cpp/Data.h>

namespace brayns
{
struct IsosurfaceParameters
{
    inline static const std::string osprayName = "isosurface";
    inline static const std::string volume = "volume";
    inline static const std::string isovalue = "isovalue";
};

template<typename T>
struct Isosurface
{
    // As owner of the volume, the isosurface needs to commit it during its own commit(), where
    // the data is const, thus volume must be mutable. Temporary until commit is changed
    mutable Volume<T> volume;
    std::vector<float> isovalues;
};

template<typename T>
class OsprayGeometryName<Isosurface<T>>
{
public:
    static const std::string &get()
    {
        return IsosurfaceParameters::osprayName;
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
        const auto &primitive = primitives.front();
        auto &isoValues = primitive.isovalues;
        auto &volume = primitive.volume;

        volume.commit();
        osprayGeometry.setParam(IsosurfaceParameters::volume, volume.getOsprayVolume());
        osprayGeometry.setParam(IsosurfaceParameters::isovalue, ospray::cpp::SharedData(isoValues));
    }
};

}
