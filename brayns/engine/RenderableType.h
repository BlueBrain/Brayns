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
#include <brayns/common/Log.h>

#include <ospray/ospray.h>

#include <memory>
#include <numeric>
#include <string_view>
#include <vector>

namespace brayns
{
/**
 * @brief The RenderableBoundsUpdater is a templated class that can be specialized to update bounds
 * based on a trasnformation and a given specialized renderable
 */
template<typename T>
struct RenderableBoundsUpdater
{
    /**
     * @brief Updates a Bounds object with the given renderable trasnformed by the given matrix
     * @param renderableType The source renderable used to update the bounds
     * @param transform The matrix by which to trasnform the geometry
     * @param bounds The bounds to update
     */
    static void update(const T& renderableType, const Matrix4f& transform, Bounds& bounds)
    {
        (void) renderableType;
        (void) transform;
        (void) bounds;
        const std::string typeName (typeid(T).name());
        throw std::runtime_error("No implementation for " + typeName);
    }
};

/**
 * @brief Utility class to deduce the OSPRay ID for a given Renderable type
 */
template<typename T>
struct RenderableOSPRayID
{
    /**
     * @brief Returns the OSPRay ID of the type
     */
    static std::string_view get()
    {
        const std::string typeName (typeid(T).name());
        throw std::runtime_error("No implementation for " + typeName);
    }
};

/**
 * @brief The Geometry class is a templated class to easily manage OSPRay geometry at a high level.
 * It allows to add, retrieve and manipulate Geometry.
 */
template<typename T>
class Geometry
{
public:
    Geometry()
    {
        _handle = ospNewGeometry(RenderableOSPRayID<T>::get().data());
    }

    Geometry(const Geometry&) = delete;
    Geometry &operator=(const Geometry& o) = delete;

    Geometry(Geometry&&) = default;
    Geometry &operator=(Geometry&&) = default;

    ~Geometry()
    {
        ospRelease(_handle);
    }

    /**
     * @brief Adds a new gometry primitive to the list. OSPRay is limited to 2^32 geometries per model.
     * @throws std::runtime_error if the Geometry buffer is already at full capacity.
     * @returns the index at which the given geometry was added to the buffer. This index is the same that
     * OSPRay will use as 'primID' within itself.
     */
    uint32_t add(T geometry)
    {
        const auto idx = _geometries.size();
        constexpr auto limit = std::numeric_limits<uint32_t>::max();
        if(idx >= limit)
            throw std::runtime_error("OSPRay is limited to 2^32 geometries per model");

        _geometries.emplace_back(std::move(geometry));
        _dirty = true;
        return static_cast<uint32_t>(idx);
    }

    /**
     * @brief Adds a list of geometries to the geometry buffer. OSPRay is limited to 2^32 geometries per model.
     * @throws std::runtime_error if the contents of the Geometry buffer plus the new geometries surpass the
     * maximum capacity.
     * @returns A list of indices at which the given list of geometries were added to the buffer. These indices,
     * returned in the same order as the input geometries, are the same that OSPRay will use as 'primID' within itself.
     */
    std::vector<uint32_t> add(const std::vector<T>& geometries)
    {
        const auto idx = _geometries.size();
        const auto endIdx = idx + geometries.size();
        constexpr auto limit = std::numeric_limits<uint32_t>::max();
        if(idx >= limit || endIdx >= limit)
        {
            throw std::runtime_error("OSPRay is limited to 2^32 geometries per model");
        }

        _geometries.insert(_geometries.end(), geometries.begin(), geometries.end());

        std::vector<uint32_t> result (geometries.size());
        std::iota(result.begin(), result.end(), static_cast<uint32_t>(idx));
        _dirty = true;
        return result;
    }

    /**
     * @brief Sets the primitives of this geometry object. Returns the list of indices that correspond
     * to the added geometries
     * 
     * @param geometries 
     * @return std::vector<uint32_t> 
     */
    std::vector<uint32_t> set(std::vector<T> geometries)
    {
        constexpr auto limit = std::numeric_limits<uint32_t>::max();
        if(geometries.size() >= limit)
        {
            throw std::runtime_error("OSPRay is limited to 2^32 geometries per model");
        }
        _geometries = std::move(geometries);
        std::vector<uint32_t> result (_geometries.size());
        std::iota(result.begin(), result.end(), 0u);
        _dirty = true;
        return result;
    }


    /**
     * @brief Retrieves a constant reference to a geometry given its index in the Geometry buffer.
     * @throws std::invalid_argument if the given index goes beyond the Geometry buffer size.
     */
    const T& get(const uint32_t index) const
    {
        _checkIndex(index);
        return _geometries[index];
    }

    /**
     * @brief Retrieves all geometry primitives in this Geometry object
     */
    const std::vector<T> &getAll() const noexcept
    {
        return _geometries;
    }

    /**
     * @brief Allows to pass a callback to manipulate a Geometry identified by its index in the Geometry buffer.
     * The callback must have the signature void(GeometryType&).
     * @throws std::invalid_argument if the given index goes beyond the Geometry buffer size.
     */
    void manipulate(const uint32_t index, const std::function<void(T&)>& manipulationCallback)
    {
        _checkIndex(index);
        auto& geometry = _geometries[index];
        _dirty = true;
        manipulationCallback(geometry);
    }

    /**
     * @brief Allows to pass a callback to mainipulate all the geometries on thie Geometry buffer.
     * The callback must have the signature void(const uint32_t index, GeometryType&).
     */
    void mainpulateAll(const std::function<void(const uint32_t, T&)>& mainpulationCallback)
    {
        _dirty = true;
        const auto end = static_cast<uint32_t>(_geometries.size());
        for(uint32_t i = 0; i < end; ++i)
            mainpulationCallback(i, _geometries[i]);
    }

    /**
     * @brief Returns the number of geometries on the Geometry buffer
     */
    uint32_t getNumGeometries() const noexcept
    {
        return _geometries.size();
    }

    /**
     * @brief Returns the size in bytes of this Geometry object
     */
    size_t getSizeInBytes() const noexcept
    {
        return sizeof(*this) + _geometries.size() * sizeof(T);
    }

    /**
     * @brief Compute the spatial bounds of the geometry on the buffer transformed by the given
     * matrix.
     * There must be a specialization of GeometryBoundsUpdater::updateBounds for the gometry type
     * handled.
     */
    Bounds computeBounds(const Matrix4f& transform) const noexcept
    {
        Bounds result;
        #pragma omp parallel
        {
            Bounds local;

            #pragma omp for
            for(size_t i = 0; i < _geometries.size(); ++i)
            {
                const auto& geometry = _geometries[i];
                RenderableBoundsUpdater<T>::update(geometry, transform, local);
            }

            #pragma omp critical (local_bounds_merge_section)
            result.expand(local);
        }

        return result;
    }

    /**
     * @brief Will attempt to synchronize the geometry data with OSPRay, if any changes since the previous
     * synchronization has happen. It will call the commitGeometrySpecificParams(), which must have been
     * specialized for the geometry type being handled.
     */
    bool commit()
    {
        if(!_dirty)
        {
            return false;
        }

        if(!_geometries.empty())
        {
            commitGeometrySpecificParams();
        }

        ospCommit(_handle);
        _dirty = false;
        return true;
    }

    /**
     * @brief Returns the OSPRay geometry object handle
     */
    OSPGeometry handle() const noexcept
    {
        return _handle;
    }

private:
    /**
     * @brief This method must be specialized by the supported geometries to commit the geometry data
     * to OSPRay
     */
    void commitGeometrySpecificParams()
    {
        const std::string typeName (typeid(T).name());
        Log::warn("No commitGeometrySpecificParams() implementation for {}", typeName);
    }

    /**
     * @brief Retrieves a geometry from the buffer given its index.
     * @throws std::invalid_argument if the index is beyond the buffer size.
     */
    void _checkIndex(const uint32_t index) const
    {
        const auto size = static_cast<uint32_t>(_geometries.size());
        if(index >= size)
            throw std::invalid_argument("Geometry index out of range");
    }

private:
    OSPGeometry _handle {nullptr};
    bool _dirty {false};

    // Shared, to allow instances to share the geometry memory
    std::vector<T> _geometries;
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
        _handle = ospNewVolume(RenderableOSPRayID<T>::get().data());
    }

    Volume(const Volume&) = delete;
    Volume& operator=(const Volume&) = delete;

    Volume(Volume&&) = default;
    Volume& operator=(Volume&&) = default;

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
    bool commit()
    {
        if(!_dirty)
        {
            return false;
        }

        commitVolumeSpecificParams();

        ospCommit(_handle);
        _dirty = false;
        return true;
    }

    /**
     * @brief Computes the bounds of this volume with the given trasnformation. It will call the VolumeBoundsUpdater
     * with the type this Volume has been instantiated.
     */
    Bounds computeBounds(const Matrix4f& transform) const noexcept
    {
        Bounds result;
        RenderableBoundsUpdater<T>::update(_volumeData, transform, result);
        return result;
    }

    /**
     * @brief Returns the size in bytes of this object. Can be specialized to provide a more accurate measure
     */
    size_t getSizeInBytes() const noexcept
    {
        return sizeof(*this);
    }

    OSPVolume handle() const noexcept
    {
        return _handle;
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
    bool _dirty {false};
};
}
