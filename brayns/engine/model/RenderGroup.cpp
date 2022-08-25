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

#include "RenderGroup.h"

#include <ospray/ospray_cpp/Data.h>

#include <algorithm>

namespace
{
struct GroupParameters
{
    inline static const std::string geometry = "geometry";
    inline static const std::string volume = "volume";
    inline static const std::string clipping = "clippingGeometry";
};

class HandleListCompiler
{
public:
    template<typename HandleType, typename ObjectType>
    static std::vector<HandleType> compile(const std::vector<ObjectType> &types)
    {
        std::vector<HandleType> result;
        result.reserve(types.size());
        for (auto &type : types)
        {
            result.push_back(type.getHandle());
        }
        return result;
    }
};
}

namespace brayns
{
void RenderGroup::fromGeometry(const GeometryView &geometry)
{
    auto handle = std::vector<ospray::cpp::GeometricModel>{geometry.getHandle()};
    _handle.setParam(GroupParameters::geometry, ospray::cpp::CopiedData(handle));
    _flag = true;
}

void RenderGroup::fromGeometry(const std::vector<GeometryView> &geometries)
{
    auto handles = HandleListCompiler::compile<ospray::cpp::GeometricModel>(geometries);
    _handle.setParam(GroupParameters::geometry, ospray::cpp::CopiedData(handles));
    _flag = true;
}

void RenderGroup::fromVolume(const VolumeView &volume)
{
    auto handle = std::vector<ospray::cpp::VolumetricModel>{volume.getHandle()};
    _handle.setParam(GroupParameters::volume, ospray::cpp::CopiedData(handle));
    _flag = true;
}

void RenderGroup::fromVolume(const std::vector<VolumeView> &volumes)
{
    auto handles = HandleListCompiler::compile<ospray::cpp::VolumetricModel>(volumes);
    _handle.setParam(GroupParameters::volume, ospray::cpp::CopiedData(handles));
    _flag = true;
}

void RenderGroup::fromClipper(const GeometryView &clipper)
{
    auto handle = std::vector<ospray::cpp::GeometricModel>{clipper.getHandle()};
    _handle.setParam(GroupParameters::clipping, ospray::cpp::CopiedData(handle));
    _flag = true;
}

void RenderGroup::fromClipper(const std::vector<GeometryView> &clippers)
{
    auto handles = HandleListCompiler::compile<ospray::cpp::GeometricModel>(clippers);
    _handle.setParam(GroupParameters::clipping, ospray::cpp::CopiedData(handles));
    _flag = true;
}

void RenderGroup::commit()
{
    if (!_flag)
    {
        return;
    }
    _flag = false;
    _handle.commit();
}

const ospray::cpp::Group &RenderGroup::getHandle() const noexcept
{
    return _handle;
}
}
