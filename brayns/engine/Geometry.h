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

#include <numeric>
#include <vector>

namespace brayns
{
template<typename T>
struct GeometryBoundsUpdater
{
    static void updateBounds(const T& geometry, const Matrix4f& transform, Bounds& bounds)
    {
        (void) geometry;
        (void) transform;
        (void) bounds;
        const std::string typeName (typeid(T).name());
        throw std::runtime_error("No implementation for " + typeName);
    }
};

template<typename T>
class Geometry : public EngineObject
{
public:
    Geometry()
    {
        throw std::runtime_error("Constructor must be specialized to instantiate the appropiate OSPRay geometry");
    }

    uint32_t add(T geometry)
    {
        const auto idx = _geometries.size();
        constexpr auto limit = std::numeric_limits<uint32_t>::max();
        if(idx >= limit)
            throw std::runtime_error("OSPRay is limited to 2^32 geometries per model");

        markModified(false);

        _geometries.push_back(std::move(geometry));
        return static_cast<uint32_t>(idx);
    }

    std::vector<uint32_t> add(const std::vector<T>& geometries)
    {
        const auto idx = _geometries.size();
        const auto endIdx = idx + geometries.size();
        constexpr auto limit = std::numeric_limits<uint32_t>::max();
        if(idx >= limit || endIdx >= limit)
            throw std::runtime_error("OSPRay is limited to 2^32 geometries per model");

        _geometries.insert(_geometries.end(), geometries.begin(), geometries.end());

        markModified(false);

        std::vector<uint32_t> result (geometries.size());
        std::iota(result.begin(), result.end(), static_cast<uint32_t>(idx));
        return result;
    }

    T& get(const uint32_t index)
    {
        const auto size = static_cast<uint32_t>(_geometries.size());
        if(index >= size)
            throw std::invalid_argument("Geometry index out of range");

        markModified(false);

        return _geometries[index];
    }

    uint32_t getNumGeometries() const noexcept
    {
        return _geometries.size();
    }

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
                GeometryBoundsUpdater<T>::updateBounds(geometry, transform, local);
            }

            #pragma omp critical (local_bounds_merge_section)
            result.expand(local);
        }

        return result;
    }

    void commitGeometrySpecificParams()
    {
        const std::string typeName (typeid(T).name());
        Log::warn("No commitGeometrySpecificParams() implementatino for {}", typeName);
    }

    void commit() final
    {
        if(!_geometries.empty())
            commitGeometrySpecificParams();

        ospCommit(_handle);
    }

    OSPGeometry handle() const noexcept
    {
        return _handle;
    }

protected:
    OSPGeometry _handle {nullptr};

    std::vector<T> _geometries;
};
}
