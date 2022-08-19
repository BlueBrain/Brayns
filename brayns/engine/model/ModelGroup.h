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

#include <brayns/engine/geometry/GeometryObject.h>
#include <brayns/engine/volume/VolumeObject.h>

#include <ospray/ospray_cpp/Group.h>

#include <algorithm>
#include <vector>

namespace brayns
{
/**
 * @brief The ModelGroup class encapsulates the handling of an OSPGroup object
 */
class ModelGroup
{
public:
    ModelGroup() = default;

    ModelGroup(const ModelGroup &) = delete;
    ModelGroup &operator=(const ModelGroup &) = delete;

    ModelGroup(ModelGroup &&) = delete;
    ModelGroup &operator=(ModelGroup &&) = delete;

    /**
     * @brief Adds a new geometric model and marks the group as dirty so it gets committed
     */
    template<typename T>
    void addGeometry(const GeometryObject<T> &geometry)
    {
        _addObject(_geometries, geometry.getOsprayObject());
    }

    /**
     * @brief Removes an existing geometric model and mark the group as dirty so it gets committed
     */
    template<typename T>
    void removeGeometry(const GeometryObject<T> &geometry)
    {
        _removeObject(_geometries, geometry.getOsprayObject());
    }

    /**
     * @brief Adds a new volumetric model and marks the group as dirty so it gets committed
     */
    template<typename T>
    void addVolume(const VolumeObject<T> &volume)
    {
        _addObject(_volumes, volume.getOsprayObject());
    }

    /**
     * @brief Removes an existing volumetric model and mark the group as dirty so it gets committed
     */
    template<typename T>
    void removeVolume(const VolumeObject<T> &volume)
    {
        _removeObject(_volumes, volume.getOsprayObject());
    }

    /**
     * @brief Adds a new geometric clipping model and marks the group as dirty so it gets commited
     */
    template<typename T>
    void addClipper(const GeometryObject<T> &clipper)
    {
        _addObject(_clippers, clipper.getOsprayObject());
    }

    /**
     * @brief Removes an existing clipping model and mark the group as dirty so it gets committed
     */
    template<typename T>
    void removeClipper(const GeometryObject<T> &clipper)
    {
        _removeObject(_clippers, clipper.getOsprayObject());
    }

private:
    friend class Model;
    friend class ModelInstance;

    template<typename T>
    void _addObject(std::vector<T> &list, const T &object)
    {
        list.push_back(object);
        _modified = true;
    }

    template<typename T>
    void _removeObject(std::vector<T> &list, const T &object)
    {
        auto handle = object.handle();
        auto it = std::find_if(list.begin(), list.end(), [&](const T &item) { return item.handle() == handle; });

        if (it != list.end())
        {
            list.erase(it);
            _modified = true;
        }
    }

    /**
     * @brief Returns the OSPGroup handle
     */
    ospray::cpp::Group &getOsprayGroup() noexcept;

    /**
     * @brief Synchronizes the group data with Ospray
     */
    bool commit();

private:
    ospray::cpp::Group _osprayGroup;
    std::vector<ospray::cpp::GeometricModel> _geometries;
    std::vector<ospray::cpp::VolumetricModel> _volumes;
    std::vector<ospray::cpp::GeometricModel> _clippers;
    bool _modified{false};
};
}
