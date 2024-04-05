/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/core/utils/FunctorInfo.h>
#include <brayns/core/utils/ModifiedFlag.h>

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
    template<typename T>
    using Traits = GeometryTraits<T>;

    template<typename Type>
    explicit Geometry(std::vector<Type> primitives):
        _handleName(GeometryTraits<Type>::handleName),
        _geometryName(GeometryTraits<Type>::name),
        _handle(ospray::cpp::Geometry(_handleName)),
        _data(std::make_unique<GeometryData<Type>>(std::move(primitives)))
    {
    }

    template<typename Type>
    explicit Geometry(Type primitive):
        Geometry(std::vector<Type>{std::move(primitive)})
    {
    }

    Geometry(Geometry &&) noexcept;
    Geometry &operator=(Geometry &&) noexcept;

    Geometry(const Geometry &other);
    Geometry &operator=(const Geometry &other);

    /**
     * @brief Tries to cast the primitive data to the given type.
     * @tparam Type primitive type to cast the data to
     * @return const std::vector<Type>*, if the cast is successful, null otherwise
     */
    template<typename Type>
    const std::vector<Type> *as() const noexcept
    {
        if (auto cast = dynamic_cast<const GeometryData<Type> *>(_data.get()))
        {
            return &cast->primitives;
        }
        return nullptr;
    }

    /**
     * @brief Iterates over the primitives applying the provided callback. The primitives will be casted to the same
     * type as the callback parameter. Providing a wrong primitive type in the callback will result in undefined
     * behaviour.
     *
     * @tparam Callable The callback type
     * @param callback Callback to apply to each primitive, with the signature void(<primitive type>&)
     */
    template<typename Callable>
    void forEach(Callable &&callback) noexcept
    {
        using ArgType = DecayFirstArgType<Callable>;
        assert(dynamic_cast<GeometryData<ArgType> *>(_data.get()));
        auto &cast = static_cast<GeometryData<ArgType> &>(*_data);
        for (auto &element : cast.primitives)
        {
            callback(element);
        }
        _flag.setModified(true);
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
    Bounds computeBounds(const TransformMatrix &matrix) const noexcept;

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
    std::string _handleName;
    std::string _geometryName;
    ospray::cpp::Geometry _handle;
    std::unique_ptr<IGeometryData> _data;
    ModifiedFlag _flag;
};
}
