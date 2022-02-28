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
#include <brayns/common/MathTypes.h>

#include <functional>
#include <stdexcept>
#include <string>

#include <ospray/ospray.h>

namespace brayns
{
/**
 * @brief The VolumeBoundsUpdater is a templated class that can be specialized to update bounds
 * based on a trasnformation and a given specialized volume type.
 */
template<typename T>
struct VolumeBoundsUpdater
{
    /**
     * @brief Updates a Bounds object with the given volume trasnformed by the given matrix
     * @param volume The volumetric data used to update the bounds
     * @param transform The matrix by which to trasnform the volume data
     * @param bounds The bounds to update
     */
    static void update(const T& volumeType, const Matrix4f& transform, Bounds& bounds)
    {
        (void) volumeType;
        (void) transform;
        (void) bounds;
        const std::string typeName (typeid(T).name());
        throw std::runtime_error("No implementation for " + typeName);
    }
};

/**
 * @brief The Volume class is a specializable wrapper to implement different types of volumes which shares the same
 * usage (as all volumes in OSPRay)
 */
template<typename T>
class Volume
{
public:
    /**
     * @brief Calls initializeHandle() to create the OSPRay object
     */
    Volume()
    {
        initializeHandle();
    }

    ~Volume()
    {
        if(_handle)
            ospRelease(_handle);
    }

    Volume(const Volume&) = delete;
    Volume& operator=(const Volume&) = delete;

    Volume(Volume&&) = default;
    Volume& operator=(Volume&&) = default;

    /**
     * @brief Must be specialized for the supported volume types to call ospNewVolume with the appropiate ID
     */
    void initializeHandle()
    {
        const std::string name (typeid(T).name());
        throw std::runtime_error("No initializeHandle() specialization for " + name);
    }

    /**
     * @brief Sets the data of this volume, which will trigger a commit of the data to OSPRay
     */
    void setData(T&& volumeData) noexcept
    {
        _volumeData = std::move(volumeData);
        _dirty = true;
    }

    /**
     * @brief Returns the current volume data
     */
    const T& getData() const noexcept
    {
        return _volumeData;
    }

    /**
     * @brief Allows to pass a callback to modify the volume data, which will trigger a commit of the data to OSPRay
     */
    void manipulate(const std::function<void(T& volumeData)>& callback)
    {
        callback(_volumeData);
        _dirty = true;
    }

    /**
     * @brief Commits the volume data to OSPRay. Will only have effect if the volume has been modified since the last
     * commit. If so, it will call commitVolumeSpecificParams() and clear the dirty flag.
     */
    void commit()
    {
        if(!_dirty)
            return;

        commitVolumeSpecificParams();

        ospCommit(_handle);
        _dirty = false;
    }

    /**
     * @brief Computes the bounds of this volume with the given trasnformation. It will call the VolumeBoundsUpdater
     * with the type this Volume has been instantiated.
     */
    Bounds computeBounds(const Matrix4f& transform) const noexcept
    {
        Bounds result;
        VolumeBoundsUpdater<T>::update(_volumeData, transform, result);
        return result;
    }

    /**
     * @brief Returns the size in bytes of this object. Can be specialized to provide a more accurate measure
     */
    uint64_t getSizeInBytes() const noexcept
    {
        return sizeof(*this);
    }

private:
    /**
     * @brief Must be specialized for the supported volume types to set the appropiate data on the OSPRay object
     */
    void commitVolumeSpecificParams()
    {
        const std::string name (typeid(T).name());
        throw std::runtime_error("No commitVolumeSpecificParams() specialization for " + name);
    }

private:
    T _volumeData;

    OSPVolume _handle {nullptr};
    bool _dirty {true};
};
}
