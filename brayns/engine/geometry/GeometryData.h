/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <brayns/engine/components/Bounds.h>

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
    virtual Bounds computeBounds(const TransformMatrix &matrix) const noexcept = 0;
};

template<typename DataType>
class GeometryData final : public IGeometryData
{
public:
    using Type = std::decay_t<DataType>;

    explicit GeometryData(std::vector<Type> primitives):
        primitives(std::move(primitives))
    {
    }

    void pushTo(ospray::cpp::Geometry &handle) override
    {
        GeometryTraits<Type>::updateData(handle, primitives);
    }

    std::unique_ptr<IGeometryData> clone() const noexcept override
    {
        return std::make_unique<GeometryData<Type>>(primitives);
    }

    size_t numPrimitives() const noexcept override
    {
        return primitives.size();
    }

    Bounds computeBounds(const TransformMatrix &matrix) const noexcept override
    {
        Bounds result;
        for (auto &primitive : primitives)
        {
            result.expand(GeometryTraits<Type>::computeBounds(matrix, primitive));
        }
        return result;
    }

    std::vector<Type> primitives;
};
}
