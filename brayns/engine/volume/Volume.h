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

#include <brayns/common/Bounds.h>
#include <brayns/common/ModifiedFlag.h>
#include <brayns/engine/common/ArgumentInferer.h>
#include <brayns/engine/common/DataWrapper.h>

#include "VolumeTraits.h"

#include <ospray/ospray_cpp/Volume.h>

#include <cassert>
#include <memory>
#include <string>

namespace brayns
{
class Volume
{
private:
    template<typename T>
    using Data = SpatialDataWrapper<T, ospray::cpp::Volume, VolumeTraits>;

public:
    template<typename T>
    Volume(T data)
        : _handleName(VolumeTraits<T>::handleName)
        , _volumeName(VolumeTraits<T>::volumeName)
        , _handle(_handleName)
        , _data(std::move(data))
    {
    }

    Volume(Volume &&) noexcept = default;
    Volume &operator=(Volume &&) noexcept = default;

    Volume(const Volume &other);
    Volume &operator=(const Volume &other);

    /**
     * @brief Returns a pointer to the underlying volume data casted to the given type, if possible.
     * @tparam Type type of data to cast the volume to.
     * @returns a const pointer to the underlying data, or null if the cast was not possible
     */
    template<typename Type>
    const Type *as() const noexcept
    {
        if (auto cast = dynamic_cast<SpatialDataWrapper<ArgType> *>(_data.get()))
        {
            return &cast->data;
        }
        return nullptr;
    }

    /**
     * @brief Allows to pass a callback to manipulate the underlying volume data. The type of
     * data is inferer from the callback argument
     * @tparam Callable
     * @param callback callback to manipulate the volume data. The argument must be a reference to the volume type.
     */
    template<typename Callable>
    void manipulate(Callable &&callback) const noexcept
    {
        using ArgType = typename ArgumentInferer<Callable>::argType;
        auto cast = dynamic_cast<const SpatialDataWrapper<ArgType> *>(_data.get());
        assert(cast);
        callback(cast->data);
        _flag = true;
    }

    /**
     * @brief Calls the underlying OSPRay commit function, if any parameter has been modified, and resets the modified
     * state.
     * @return true If any parameter was modified and thus the commit function was called.
     * @return false If no parameter was modified.
     */
    bool commit();

    /**
     * @brief Compute the spatial bounds of the transformed voxels.
     * @param matrix transformation to apply to the volume spatial data.
     * @return Bounds axis-aligned bounding box of the volume
     */
    Bounds computeBounds(const Matrix4f &matrix) const noexcept;

    /**
     * @brief Returns the volume Ospray handle
     * @return const ospray::cpp::Volume &
     */
    const ospray::cpp::Volume &getHandle() const noexcept;

private:
    std::string _handleName;
    std::string _volumeName;
    ospray::cpp::Volume _handle;
    std::unique_ptr<ISpatialDataWrapper<ospray::cpp::Volume>> _data;
    ModifiedFlag _flag;
};
}
