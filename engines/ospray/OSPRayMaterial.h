/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <brayns/engine/Material.h>
#include <ospray.h>

namespace brayns
{
class OSPRayMaterial : public Material
{
public:
    OSPRayMaterial(const PropertyMap& properties = {},
                   const bool backgroundMaterial = false)
        : Material(properties)
        , _isBackGroundMaterial(backgroundMaterial)
    {
    }
    ~OSPRayMaterial();

    /** Noop until commit(renderer) is called. */
    void commit() final;

    /** Instance the actual renderer specific object for this material.
        This operation always creates a new ISPC side material.
     */
    void commit(const std::string& renderer);

    OSPMaterial getOSPMaterial() { return _ospMaterial; }

private:
    OSPMaterial _ospMaterial{nullptr};
    bool _isBackGroundMaterial{false};
    std::string _renderer;
};
} // namespace brayns
