/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#ifndef OSPRAYMATERIAL_H
#define OSPRAYMATERIAL_H

#include <brayns/common/material/Material.h>
#include <ospray.h>

namespace brayns
{
class OSPRayMaterial : public Material
{
public:
    OSPRayMaterial() = default;
    ~OSPRayMaterial();

    /** Noop until commit(renderer) is called. */
    void commit() final;

    /** Instance the actual renderer specific object for this material.
        This operation always creates a new ISPC side material.
     */
    void commit(const std::string& renderer);

    OSPMaterial getOSPMaterial() { return _ospMaterial; }
private:
    OSPTexture _createOSPTexture2D(Texture2DPtr texture);
    OSPMaterial _ospMaterial{nullptr};
};
}

#endif // OSPRAYMATERIAL_H
