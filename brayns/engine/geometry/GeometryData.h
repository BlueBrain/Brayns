/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/common/Bounds.h>

#include "GeometryTraits.h"

#include <ospray/ospray_cpp/Geometry.h>

#include <cassert>
#include <memory>

namespace brayns
{
class IGeometryData
{
public:
    virtual ~IGeometryData() = default;
    virtual void pushTo(ospray::cpp::Geometry &handle) = 0;
    virtual std::unique_ptr<IGeometryData> clone() const noexcept = 0;
    virtual size_t numPrimitives() const noexcept = 0;
    virtual Bounds computeBounds(const Matrix4f &matrix) const noexcept = 0;
};

template<typename Type>
class GeometryData final : public IGeometryData
{
public:
    GeometryData(std::vector<Type> primitives)
        : primitives(std::move(primitives))
    {
    }

    void pushTo(ospray::cpp::Geometry &handle) override
    {
        GeometryTraits<Type>::update(handle, primitives);
    }

    std::unique_ptr<IGeometryData> clone() const noexcept override
    {
        return std::make_unique<GeometryData<Type>>(primitives);
    }

    size_t numPrimitives() const noexcept override
    {
        return primitives.size();
    }

    Bounds computeBounds(const Matrix4f &matrix) const noexcept override
    {
        Bounds result;
#pragma omp parallel
        {
            Bounds local;

#pragma omp for
            for (size_t i = 0; i < primitives.size(); ++i)
            {
                local.expand(GeometryTraits<Type>::computeBounds(matrix, primitives[i]));
            }

#pragma omp critical(local_bounds_merge_section)
            result.expand(local);
        }

        return result;
    }

    std::vector<Type> primitives;
};
}
