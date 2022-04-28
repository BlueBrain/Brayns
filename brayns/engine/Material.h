/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/common/BaseObject.h>
#include <brayns/common/MathTypes.h>

#include <ospray/ospray.h>

#include <memory>
#include <string_view>

namespace brayns
{
/**
 * @brief The Material class defines which material is applied to the geometry is associated with.
 * Materials define the physical surface properties of the geometry, BUT NOT THE COLOR. Color is
 * managed by the GeometricModels implementation, because this allows for a more optimal representation
 * of the geometry color for Brayns needs.
 */
class Material : public BaseObject
{
public:
    Material(std::string_view handleID);

    Material(const Material &) = delete;
    Material &operator=(const Material &) = delete;

    Material(Material &&) = delete;
    Material &operator=(Material &&) = delete;

    virtual ~Material();

    /**
     * @brief returns the handle to the OSPRay object of the material
     */
    OSPMaterial handle() const noexcept;

    /**
     * @brief Sets the base color
     *
     * @param color
     */
    void setColor(const Vector3f &color) noexcept;

    /**
     * @brief Returns the base color
     *
     * @return const Vector3f&
     */
    const Vector3f &getColor() const noexcept;

    /**
     * @brief Returns the material type as a string
     */
    virtual std::string getName() const noexcept = 0;

    /**
     * @brief synchronizes the material data with the OSPRay backend
     * @returns true if there was anything to commit
     */
    bool commit();

protected:
    /**
     * @brief Subclasses must implement this method to commit material specific parameters to the OSPRay material
     * counterpart. The base class will make sure to call ospCommit(handle) on the material handle, so subclasses
     * should avoid it.
     */
    virtual void commitMaterialSpecificParams() = 0;

private:
    OSPMaterial _handle{nullptr};

    Vector3f _color{1.f};
};
} // namespace brayns
