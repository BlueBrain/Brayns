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

#include <brayns/common/ModifiedFlag.h>
#include <brayns/engine/common/ArgumentInferer.h>

#include "GeometryData.h"
#include "GeometryTraits.h"

#include <ospray/ospray_cpp/Geometry.h>

#include <cassert>
#include <memory>
#include <vector>

namespace brayns
{
class Geometry
{
public:
    template<typename Type>
    Geometry(std::vector<Type> primitives)
        : _osprayHandleName(GeometryTraits<Type>::osprayValue)
        , _geometryName(GeometryTraits<Type>::value)
        , _handle(_osprayHandleName)
        , _data(std::make_unique<GeometryData<Type>>(std::move(primitives)))
    {
    }

    Geometry(Geometry &&) noexcept = default;
    Geometry &operator=(Geometry &&) noexcept = default;

    Geometry(const Geometry &other);
    Geometry &operator=(const Geometry &other);

    /**
     * @brief Allows to pass a callback of the underlying primitive type to iterate over all the primitives.
     * @tparam Callable callback signature whose argument must be a single reference to the underlying geometry type.
     * @param callback the Callable object.
     */
    template<typename Callable>
    void forEach(Callable &&callback) const noexcept
    {
        using ArgType = typename ArgumentInferer<Callable>::argType;
        auto cast = dynamic_cast<const GeometryData<ArgType> *>(data.get());
        assert(cast);
        for (auto &element : cast->elements)
        {
            callable(element);
        }
    }

    /**
     * @copydoc Geometry::forEach<Callable>(Callable&&) const
     */
    template<typename Callable>
    void forEach(Callable &&callback) noexcept
    {
        using ArgType = typename ArgumentInferer<Callable>::argType;
        auto cast = dynamic_cast<GeometryData<ArgType> *>(data.get());
        assert(cast);
        for (auto &element : cast->elements)
        {
            callable(element);
        }
        data->pushTo(_handle);
        _flag = true;
    }

    /**
     * @brief Return the number of primitives that make up this geometry.
     * @return size_t number of primitives.
     */
    size_t numPrimitives() const noexcept;

    /**
     * @brief Compute the spatial bounds of the transformed primitives.
     * @param matrix transformation to apply to the geometry spatial data.
     * @return Bounds axis-aligned bounding box of the geometry
     */
    Bounds computeBounds(const Matrix4f &matrix) const noexcept;

    /**
     * @brief Calls the underlying OSPRay commit function, if any parameter has been modified, and resets the modified
     * state.
     * @return true If any parameter was modified and thus the commit function was called.
     * @return false If no parameter was modified.
     */
    bool commit();

    /**
     * @brief Returns the OSPRay geometry handle.
     * @return const ospray::cpp::Geometry&.
     */
    const ospray::cpp::Geometry &getHandle() const noexcept;

private:
    std::string _osprayHandleName;
    std::string _geometryName;
    ospray::cpp::Geometry _handle;
    std::unique_ptr<IGeometryData> _data;
    ModifiedFlag _flag;
};
}
