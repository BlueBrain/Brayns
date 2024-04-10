/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/core/engine/common/DataWrapper.h>
#include <brayns/core/utils/ModifiedFlag.h>

#include "MaterialTraits.h"
#include "types/Phong.h"

#include <ospray/ospray_cpp/Material.h>

#include <memory>

namespace brayns
{
class Material
{
private:
    template<typename MaterialType>
    using Data = DataWrapper<MaterialType, ospray::cpp::Material, MaterialTraits>;

public:
    template<typename T>
    using Traits = MaterialTraits<T>;

    template<typename MaterialType = Phong>
    Material(MaterialType data = MaterialType()):
        _handleName(MaterialTraits<MaterialType>::handleName),
        _materialName(MaterialTraits<MaterialType>::name),
        _handle(_handleName),
        _data(std::make_unique<Data<MaterialType>>(std::move(data)))
    {
        _data->pushTo(_handle);
    }

    Material(Material &&) noexcept;
    Material &operator=(Material &&) noexcept;

    Material(const Material &other);
    Material &operator=(const Material &other);

    /**
     * @brief Tries to cast the material data to the given type.
     * @tparam MaterialType material type to cast the data to
     * @return const MaterialType*, will be nullptr if the cast fails
     */
    template<typename MaterialType>
    const MaterialType *as() const noexcept
    {
        if (auto cast = dynamic_cast<const Data<MaterialType> *>(_data.get()))
        {
            return &cast->data;
        }
        return nullptr;
    }

    /**
     * @brief Calls the underlying OSPRay commit function, if any parameter has been modified, and resets the modified
     * state.
     * @return true If any parameter was modified and thus the commit function was called.
     * @return false If no parameter was modified.
     */
    bool commit();

    /**
     * @brief Returns the material name.
     * @return const std::string&.
     */
    std::string getName() const noexcept;

    /**
     * @brief Returns the OSPRay material handle.
     * @return const ospray::cpp::Material&.
     */
    const ospray::cpp::Material &getHandle() const noexcept;

private:
    std::string _handleName;
    std::string _materialName;
    ospray::cpp::Material _handle;
    std::unique_ptr<IDataWrapper<ospray::cpp::Material>> _data;
    ModifiedFlag _flag;
};
}
