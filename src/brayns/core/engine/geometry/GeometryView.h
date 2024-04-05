/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/core/engine/material/Material.h>
#include <brayns/core/utils/ModifiedFlag.h>

#include "Geometry.h"

#include <ospray/ospray_cpp/Data.h>
#include <ospray/ospray_cpp/GeometricModel.h>
#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace brayns
{
class GeometryView
{
public:
    explicit GeometryView(const Geometry &geometry);

    void setMaterial(const Material &material);

    void setNormalsInverted(bool inverted);

    void setColor(const Vector3f &color);
    void setColor(const Vector4f &color);

    template<bool Shared>
    void setColorPerPrimitive(const ospray::cpp::Data<Shared> &colorList)
    {
        _setColorPerPrimitive(colorList.handle());
    }

    template<bool SharedIndices, bool SharedColors>
    void setColorMap(const ospray::cpp::Data<SharedIndices> &indices, const ospray::cpp::Data<SharedColors> &colors)
    {
        _setColorMap(indices.handle(), colors.handle());
    }

    bool commit();

    const ospray::cpp::GeometricModel &getHandle() const noexcept;

private:
    void _setColorPerPrimitive(const OSPData handle);
    void _setColorMap(const OSPData indexHandle, const OSPData colorHandle);

private:
    ospray::cpp::GeometricModel _handle;
    ModifiedFlag _flag;
};
}
