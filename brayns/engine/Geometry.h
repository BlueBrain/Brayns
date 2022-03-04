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
#include <brayns/engine/EngineObject.h>

#include <ospray/ospray.h>

#include <memory>
#include <numeric>
#include <vector>

namespace brayns
{
/**
 * @brief The GometryBoundsUpdate is a templated class that can be specialized to update bounds
 * based on a trasnformation and a given specialized geometry
 */
template<typename T>
struct GeometryBoundsUpdater
{
    /**
     * @brief Updates a Bounds object with the given geometry trasnformed by the given matrix
     * @param geometry The source geometry used to update the bounds
     * @param transform The matrix by which to trasnform the geometry
     * @param bounds The bounds to update
     */
    static void update(const T& geometry, const Matrix4f& transform, Bounds& bounds)
    {
        (void) geometry;
        (void) transform;
        (void) bounds;
        const std::string typeName (typeid(T).name());
        throw std::runtime_error("No implementation for " + typeName);
    }
};

/**
 * @brief The Geometry class is a templated class to easily manage OSPRay geometry at a high level.
 * It allows to add, retrieve and manipulate Geometry. For each supported geometry:
 * - The constructor must be specialized to instantiate the appropiate OSPRay geometry
 * - The commitGeometrySpecificParams() must be specialized to feed the geometry data to the OSPRay geometry
 * - There must a specialization of GeometryBoundsUpdater for the given geometry.
 */
template<typename T>
class Geometry
{
public:
    Geometry()
    {
        initializeHandle();
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
     * @brief initializeHandle must be specialized for the supported geometries in order to instantiate the
     * OSPRay geometry counterpart
     */
    void initializeHandle()
    {
        throw std::runtime_error("initializeHandle() must be specialized to instantiate the OSPRay geometry");
    }

    /**
     * @brief Adds a new gometry to the buffer of geometries. OSPRay is limited to 2^32 geometries per model.
     * @throws std::runtime_error if the Geometry buffer is already at full capacity.
     * @returns the index at which the given geometry was added to the buffer. This index is the same that
     * OSPRay will use as 'primID' within itself.
     */
    uint32_t add(const T& geometry)
    {
        const auto idx = _geometries.size();
        constexpr auto limit = std::numeric_limits<uint32_t>::max();
        if(idx >= limit)
            throw std::runtime_error("OSPRay is limited to 2^32 geometries per model");

        _geometries.push_back(geometry);
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
            throw std::runtime_error("OSPRay is limited to 2^32 geometries per model");

        _geometries.insert(_geometries.end(), geometries.begin(), geometries.end());

        std::vector<uint32_t> result (geometries.size());
        std::iota(result.begin(), result.end(), static_cast<uint32_t>(idx));
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

    bool isModified() const noexcept
    {
        return _dirty;
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
                GeometryBoundsUpdater<T>::update(geometry, transform, local);
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
    void doCommit()
    {
        if(!_dirty)
            return;

        if(!_geometries.empty())
            commitGeometrySpecificParams();

        ospCommit(_handle);
        _dirty = false;
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
        Log::warn("No commitGeometrySpecificParams() implementatino for {}", typeName);
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

protected:
    OSPGeometry _handle {nullptr};
    bool _dirty {true};

    // Shared, to allow instances to share the geometry memory
    std::vector<T> _geometries;
};
}
