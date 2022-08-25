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

#include <brayns/common/ModifiedFlag.h>
#include <brayns/engine/geometry/GeometryView.h>
#include <brayns/engine/volume/VolumeView.h>

#include <ospray/ospray_cpp/Group.h>

#include <vector>

namespace brayns
{
class RenderGroup
{
public:
    void setGeometry(const GeometryView &geometry);
    void setGeometry(const std::vector<GeometryView> &geometries);
    void setGeometry(const std::vector<ospray::cpp::GeometricModel> &geometries);
    void setVolume(const VolumeView &volume);
    void setVolume(const std::vector<VolumeView> &volumes);
    void setVolume(const std::vector<ospray::cpp::VolumetricModel> &volumes);
    void setClipper(const GeometryView &clipper);
    void setClipper(const std::vector<GeometryView> &clippers);
    void setClipper(const std::vector<ospray::cpp::GeometricModel> &clippers);

    void commit();
    const ospray::cpp::Group &getHandle() const noexcept;

private:
    ospray::cpp::Group _handle;
    ModifiedFlag _flag;
};
}
