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

#include <brayns/engine/Geometry.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Material.h>

#include <brayns/json/JsonType.h>

namespace brayns
{
/**
 * @brief The ClippingModel class is an utility class to add clipping geometry seamlessly to the scene
 */
template<typename GeometryType>
class ClippingModel : public Model
{
public:
    /**
     * @brief Initializes the OSPRay handle
     */
    ClippingModel()
    {
        _handle = ospNewGeometricModel();
    }

    ~ClippingModel()
    {
        if(_handle)
            ospRelease(_handle);
    }

    /**
     * @brief Adds a new gometry of the type supported by this ClippingModel instance to be used as clipping
     * proxy in the scene
     */
    void addClipGeometry(const GeometryType &geometry)
    {
        _geometry.add(geometry);
        markModified(false);
    }

    /**
     * @brief Commit implementation
     */
    void commit() final
    {
        _geometry.commit();

        auto geomHandle = _geometry.handle();
        ospSetParam(_handle, "geometry", OSPDataType::OSP_GEOMETRY, &geomHandle);

        std::vector<OSPGeometricModel> modelList = {_handle};

        auto sharedModelList = ospNewSharedData(modelList.data(), OSPDataType::OSP_GEOMETRIC_MODEL, 1);
        auto copyModelList = ospNewData(OSPDataType::OSP_GEOMETRIC_MODEL, 1);
        ospCopyData(sharedModelList, copyModelList);

        auto ospGroup = groupHandle();
        ospSetParam(ospGroup, "clippingGeometry", OSPDataType::OSP_DATA, &copyModelList);

        ospRelease(sharedModelList);
        ospRelease(copyModelList);

        ospCommit(ospGroup);
    }

    /**
     * @brief Returns the geometric model OSPRay handle
     */
    OSPGeometricModel handle() const noexcept
    {
        return _handle;
    }

    /**
     * @brief Returns a JsonValue with the geometry of this clipping plane serialized
     */
    JsonValue serializeGeometry() const
    {
        return _geometry.serialize();
    }

protected:
    uint64_t getSizeInBytes() const noexcept final
    {
        return sizeof(ClippingModel) + _geometry.getNumGeometries() * sizeof(GeometryType);
    }

private:
    OSPGeometricModel _handle {nullptr};
    Geometry<GeometryType> _geometry;
};
}
