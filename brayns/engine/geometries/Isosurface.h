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
#include <brayns/engine/common/DataHandler.h>

namespace brayns
{
template<typename T>
struct Isosurface
{
    std::vector<float> isovalues;
    Volume<T> volume;
};

template<typename T>
class GeometryOSPRayID<Isosurface<T>>
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
class GeometryAddChecker<Isosurface<T>>
{
public:
    static void check(const std::vector<Isosurface<T>> &dstGeometry, const Isosurface<T> &inputGeometry)
    {
        (void)inputGeometry;
        if (!dstGeometry.empty())
        {
            throw std::runtime_error("Geometry<Isosurface<T>> only accepts 1 geometry");
        }
    }

    static void check(const std::vector<Isosurface<T>> &dstGeometry, const std::vector<Isosurface<T>> &inputGeometries)
    {
        if (!dstGeometry.empty() || inputGeometries.size() > 1)
        {
            throw std::runtime_error("Geometry<Isosurface<T>> only accepts 1 geometry");
        }
    }
};

template<typename T>
class GeometryCommitter<Isosurface<T>>
{
public:
    static void commit(OSPGeometry handle, const std::vector<Isosurface<T>> &geometries)
    {
        const auto &geometry = geometries.front();
        const auto &volume = geometry.volume;

        const auto volumeHandle = volume.handle();
        ospSetParam(handle, "volume", OSPDataType::OSP_VOLUME, &volumeHandle);

        auto sharedIsoValues = DataHandler::shareBuffer(geometry.isovalues, OSPDataType::OSP_FLOAT);
        ospSetParam(handle, "isovalue", OSPDataType::OSP_DATA, &sharedIsoValues.handle);
    }
};

}
