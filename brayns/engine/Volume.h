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

#include <ospray/ospray.h>

#include <functional>
#include <memory>
#include <numeric>
#include <string_view>
#include <vector>

namespace brayns
{
/**
 * @brief The RenderableBoundsUpdater is a templated class that can be specialized to update bounds
 * based on a trasnformation and a given specialized volume
 */
template<typename T>
class VolumeBoundsUpdater
{
public:
    /**
     * @brief Updates a Bounds object with the given volume trasnformed by the given matrix
     * @param volume The source volume used to update the bounds
     * @param transform The matrix by which to trasnform the geometry
     * @param bounds The bounds to update
     */
    static void update(const T &volume, const Matrix4f &transform, Bounds &bounds)
    {
        (void)volume;
        (void)transform;
        (void)bounds;
    }
};

/**
 * @brief Utility class to deduce the OSPRay ID for a given volume type
 */
template<typename T>
class VolumeOSPRayID
{
public:
    /**
     * @brief Returns the OSPRay ID of the type
     */
    static std::string_view get()
    {
        return "";
    }
};

/**
 * @brief Utility class to commit volume-specific parameters
 */
template<typename T>
class VolumeCommitter
{
public:
    /**
     * @brief Commits the type specific parameters
     */
    static void commit(OSPVolume handle, const T &volumeData)
    {
        (void)handle;
        (void)volumeData;
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
    Volume()
        : _handle(ospNewVolume(VolumeOSPRayID<T>::get().data()))
    {
    }

    Volume(const Volume &) = delete;
    Volume &operator=(const Volume &) = delete;

    Volume(Volume &&o) noexcept
    {
        *this = std::move(o);
    }

    Volume &operator=(Volume &&o) noexcept
    {
        std::swap(_handle, o._handle);
        _dirty = o._dirty;
        _volumeData = std::move(o._volumeData);
        return *this;
    }

    ~Volume()
    {
        ospRelease(_handle);
    }

    /**
     * @brief Sets the data of this volume, which will trigger a commit of the data to OSPRay
     */
    void setData(T volumeData) noexcept
    {
        _volumeData = std::move(volumeData);
        _dirty = true;
    }

    /**
     * @brief Returns the current volume data
     */
    const T &getData() const noexcept
    {
        return _volumeData;
    }

    /**
     * @brief Allows to pass a callback to modify the volume data, which will trigger a commit of the data to OSPRay
     */
    void manipulate(const std::function<void(T &volumeData)> &callback)
    {
        callback(_volumeData);
        _dirty = true;
    }

    /**
     * @brief Commits the volume data to OSPRay. Will only have effect if the volume has been modified since the last
     * commit. If so, it will call commitVolumeSpecificParams() and clear the dirty flag.
     */
    bool commit()
    {
        if (!_dirty)
        {
            return false;
        }

        VolumeCommitter<T>::commit(_handle, _volumeData);

        ospCommit(_handle);

        _dirty = false;
        return true;
    }

    /**
     * @brief Computes the bounds of this volume with the given trasnformation. It will call the VolumeBoundsUpdater
     * with the type this Volume has been instantiated.
     */
    Bounds computeBounds(const Matrix4f &transform) const noexcept
    {
        Bounds result;
        VolumeBoundsUpdater<T>::update(_volumeData, transform, result);
        return result;
    }

    /**
     * @brief Returns the volume OSPRay handle
     *
     * @return OSPVolume
     */
    OSPVolume handle() const noexcept
    {
        return _handle;
    }

private:
    OSPVolume _handle{nullptr};
    bool _dirty{false};
    T _volumeData;
};
}
