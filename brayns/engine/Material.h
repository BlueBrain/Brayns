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

#include <brayns/common/MathTypes.h>
#include <brayns/common/ModifiedFlag.h>

#include <ospray/ospray_cpp/Material.h>

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
class Material
{
public:
    Material(const std::string &handleID);

    Material(const Material &) = delete;
    Material &operator=(const Material &) = delete;

    Material(Material &&) = delete;
    Material &operator=(Material &&) = delete;

    virtual ~Material() = default;

    /**
     * @brief returns the handle to the Ospray object of the material
     */
    const ospray::cpp::Material &getOsprayMaterial() const noexcept;

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
     * @brief synchronizes the material data with the Ospray backend
     * @returns true if there was anything to commit
     */
    bool commit();

protected:
    /**
     * @brief Returns the modified flag status object
     */
    ModifiedFlag &getModifiedFlag() noexcept;

    /**
     * @brief Subclasses must implement this method to commit material specific parameters to the Ospray material
     * counterpart. The base class will make sure to call ospCommit(handle) on the material handle, so subclasses
     * should avoid it.
     */
    virtual void commitMaterialSpecificParams() = 0;

private:
    ospray::cpp::Material _osprayMaterial;
    Vector3f _color{1.f};
    ModifiedFlag _flag;
};
} // namespace brayns
