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

#include <ospray/ospray_cpp/Volume.h>

#include <numeric>
#include <string>
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
class OsprayVolumeName
{
public:
    /**
     * @brief Returns the OSPRay ID of the type
     */
    static const std::string &get()
    {
        throw std::runtime_error("Unhandled volume type");
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
    static void commit(const ospray::cpp::Volume &osprayVolume, const T &volumeData)
    {
        (void)osprayVolume;
        (void)volumeData;
        throw std::runtime_error("Unhandled volume type");
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
    Volume(T data)
        : _osprayVolume(OsprayVolumeName<T>::get())
        , _volumeData(std::move(data))
    {
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
    template<typename Callable>
    void manipulate(const Callable &callback)
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

        VolumeCommitter<T>::commit(_osprayVolume, _volumeData);
        _osprayVolume.commit();

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
    const ospray::cpp::Volume &getOsprayVolume() const noexcept
    {
        return _osprayVolume;
    }

private:
    ospray::cpp::Volume _osprayVolume;
    bool _dirty = true;
    T _volumeData;
};
}
