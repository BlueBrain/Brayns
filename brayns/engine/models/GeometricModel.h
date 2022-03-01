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

#include <brayns/engine/Model.h>
#include <brayns/engine/Material.h>

namespace brayns
{
/**
 * @brief The GeometricModel class is a candidate base class for geometry based models with a material
 */
class GeometricModel : public Model
{
public:
    /**
     * @brief Initializes the OSPRay handle and the material to a default one
     */
    GeometricModel();

    virtual ~GeometricModel();

    /**
     * @brief Sets the material applied to the surface of the geometry handled by this model
     */
    void setMaterial(Material::Ptr&& material);

    /**
     * @brief Returns the material of this model
     */
    const Material& getMaterial() const noexcept;

    /**
     * @brief Commit implementation
     */
    void commit() final;

    /**
     * @brief Returns the geometric model OSPRay handle
     */
    OSPGeometricModel handle() const noexcept;

protected:
    uint64_t getSizeInBytes() const noexcept final;

    /**
     * @brief Subclasses must implement this returning their size in bytes
     */
    virtual uint64_t getGeometryModelSizeInBytes() const noexcept = 0;

    /**
     * @brief Sublclasses must implement this to commit their geometry
     */
    virtual void commitGeometryModel() = 0;

private:
    OSPGeometricModel _handle {nullptr};
    Material::Ptr _material {nullptr};
};
}
