/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/engine/common/DataWrapper.h>
#include <brayns/engine/components/Bounds.h>

#include "LightTraits.h"

#include <ospray/ospray_cpp/Light.h>

#include <memory>

namespace brayns
{
class Light
{
private:
    template<typename LightType>
    using Data = SpatialDataWrapper<LightType, ospray::cpp::Light, LightTraits>;

public:
    template<typename T>
    using Traits = LightTraits<T>;

    template<typename Type>
    explicit Light(Type data):
        _handleName(LightTraits<Type>::handleName),
        _lightName(LightTraits<Type>::name),
        _handle(_handleName),
        _data(std::make_unique<Data<Type>>(std::move(data)))
    {
        _data->pushTo(_handle);
        _handle.commit();
    }

    Light(Light &&) noexcept;
    Light &operator=(Light &&) noexcept;

    Light(const Light &other);
    Light &operator=(const Light &other);

    /**
     * @brief Tries to cast the light data to the given type.
     * @tparam Type light type to cast the data to
     * @return const Type* if the cast is successful, null otherwise
     */
    template<typename Type>
    const Type *as() const noexcept
    {
        if (auto cast = dynamic_cast<const Data<Type> *>(_data.get()))
        {
            return &cast->data;
        }
        return nullptr;
    }

    /**
     * @brief Returns the OSPRay light handle.
     * @return const ospray::cpp::Light&.
     */
    const ospray::cpp::Light &getHandle() const noexcept;

    /**
     * @brief Compute the spatial bounds of the transformed light.
     * @param matrix transformation to apply to the light spatial data.
     * @return Bounds axis-aligned bounding box of the light
     */
    Bounds computeBounds(const TransformMatrix &matrix) const noexcept;

private:
    std::string _handleName;
    std::string _lightName;
    ospray::cpp::Light _handle;
    std::unique_ptr<ISpatialDataWrapper<ospray::cpp::Light>> _data;
};
} // namespace brayns
