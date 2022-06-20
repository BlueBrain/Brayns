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

#include <ospray/ospray_cpp/Geometry.h>

#include <numeric>
#include <string_view>
#include <vector>

namespace brayns
{
/**
 * @brief The RenderableBoundsUpdater is a templated class that can be specialized to update bounds
 * based on a trasnformation and a given specialized geometry
 */
template<typename T>
class GeometryBoundsUpdater
{
public:
    /**
     * @brief Updates a Bounds object with the given geometry trasnformed by the given matrix
     * @param renderableType The source renderable used to update the bounds
     * @param transform The matrix by which to trasnform the geometry
     * @param bounds The bounds to update
     */
    static void update(const T &renderableType, const Matrix4f &transform, Bounds &bounds)
    {
        (void)renderableType;
        (void)transform;
        (void)bounds;
    }
};

/**
 * @brief Utility class to deduce the Ospray geometry name for a given geometry type
 */
template<typename T>
class OsprayGeometryName
{
public:
    /**
     * @brief Returns the Ospray name of the type
     */
    static std::string_view get()
    {
        throw std::runtime_error("Unhandled geometry type");
    }
};

template<typename T>
class InputGeometryChecker
{
public:
    static void check(const std::vector<T> &primitives)
    {
        if (primitives.size() >= std::numeric_limits<uint32_t>::max())
        {
            throw std::invalid_argument("Maximum number of primitives is 2^32 - 1");
        }
    }
};

/**
 * @brief Utility class to commit geometry-specific parameters
 */
template<typename T>
class GeometryCommitter
{
public:
    /**
     * @brief Commits the type specific parameters
     */
    static void commit(const ospray::cpp::Geometry &osprayGeometry, const std::vector<T> &primitives)
    {
        (void)osprayGeometry;
        (void)primitives;
        throw std::runtime_error("Unhandled geometry type");
    }
};

/**
 * @brief The Geometry class is a templated class to easily manage Ospray geometry at a high level.
 * It allows to add, retrieve and manipulate Geometry.
 */
template<typename T>
class Geometry
{
public:
    Geometry(T primitive)
        : _osprayGeometry(OsprayGeometryName<T>::get().data())
    {
        _primitives.push_back(std::move(primitive));
        InputGeometryChecker<T>::check(_primitives);
    }

    Geometry(std::vector<T> primitives)
        : _osprayGeometry(OsprayGeometryName<T>::get().data())
        , _primitives(std::move(primitives))
    {
        InputGeometryChecker<T>::check(_primitives);
    }

    /**
     * @brief Retrieves all geometry primitives in this Geometry object
     */
    const std::vector<T> &getPrimitives() const noexcept
    {
        return _primitives;
    }

    /**
     * @brief Allows to pass a callback to mainipulate all the geometries on thie Geometry buffer.
     * The callback must have the signature void(const uint32_t index, GeometryType&).
     */
    template<typename Callable>
    void forEach(const Callable &callback)
    {
        _dirty = true;
        const auto end = static_cast<uint32_t>(_primitives.size());
        for (uint32_t i = 0; i < end; ++i)
        {
            callback(i, _primitives[i]);
        }
    }

    /**
     * @brief Compute the spatial bounds of the geometry on the buffer transformed by the given
     * matrix.
     * There must be a specialization of GeometryBoundsUpdater::updateBounds for the gometry type
     * handled.
     */
    Bounds computeBounds(const Matrix4f &transform) const noexcept
    {
        Bounds result;
#pragma omp parallel
        {
            Bounds local;

#pragma omp for
            for (size_t i = 0; i < _primitives.size(); ++i)
            {
                const auto &geometry = _primitives[i];
                GeometryBoundsUpdater<T>::update(geometry, transform, local);
            }

#pragma omp critical(local_bounds_merge_section)
            result.expand(local);
        }

        return result;
    }

    /**
     * @brief Will attempt to synchronize the geometry data with Ospray, if any changes since the previous
     * synchronization has happen. It will call the commitGeometrySpecificParams(), which must have been
     * specialized for the geometry type being handled.
     */
    bool commit()
    {
        if (!_dirty)
        {
            return false;
        }

        GeometryCommitter<T>::commit(_osprayGeometry, _primitives);
        _osprayGeometry.commit();

        _dirty = false;

        return true;
    }

    /**
     * @brief Returns the Ospray geometry object handle
     */
    const ospray::cpp::Geometry &getOsprayGeometry() const noexcept
    {
        return _osprayGeometry;
    }

private:
    ospray::cpp::Geometry _osprayGeometry;
    bool _dirty{false};
    std::vector<T> _primitives;
};
}
