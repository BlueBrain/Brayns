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

#include <brayns/engine/common/DataWrapper.h>

#include "GeometryData.h"
#include "GeometryTraits.h"

#include <ospray/ospray_cpp/Geometry.h>

#include <memory>
#include <vector>

namespace brayns
{
class Geometry
{
private:
public:
    template<typename Type>
    Geometry(std::vector<Type> primitives)
        : _osprayHandleName(GeometryTraits<Type>::osprayValue)
        , _geometryName(GeometryTraits<Type>::value)
        , _handle(_osprayHandleName)
        , _data(std::make_unique<GeometryData<Type>>(std::move(primitives)))
    {
    }

    Geometry(std::vector<T> primitives)
        : _osprayGeometry(OsprayGeometryName<T>::get())
        , _primitives(std::move(primitives))
    {
        assert(!_primitives.empty());
        InputGeometryChecker<T>::check(_primitives);
    }

    /**
     * @brief Retrieves all geometry primitives in this Geometry object
     */
    const std::vector<T> &getPrimitives() const noexcept
    {
        return _primitives;
    }

    /**
     * @brief Allows to pass a callback to mainipulate all the geometries on thie Geometry buffer.
     * The callback must have the signature void(const uint32_t index, GeometryType&).
     */
    template<typename Type, typename Callable>
    void forEach(const Callable &callback)
    {
        _dirty = true;
        const auto end = static_cast<uint32_t>(_primitives.size());
        for (uint32_t i = 0; i < end; ++i)
        {
            callback(i, _primitives[i]);
        }
    }

    Bounds computeBounds(const Matrix4f &transform) const noexcept;

    bool commit();

    const ospray::cpp::Geometry &getHandle() const noexcept;

private:
    std::string _osprayHandleName;
    std::string _geometryName;
    ospray::cpp::Geometry _handle;
    std::unique_ptr<IGeometryData> _data;
};
}
