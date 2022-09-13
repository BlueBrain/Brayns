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
struct RenderGroupParameters
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

class RenderGroupBuilder
{
public:
    template<typename T>
    static brayns::RenderGroup build(const std::string &param, const std::vector<T> &handles)
    {
        brayns::RenderGroup group;
        auto &handle = group.getHandle();
        handle.setParam(param, ospray::cpp::CopiedData(handles));
        return group;
    }
};
}

namespace brayns
{
RenderGroup::RenderGroup(const RenderGroup &other)
{
    *this = other;
}

RenderGroup &RenderGroup::operator=(const RenderGroup &other)
{
    (void)other;
    _handle = ospray::cpp::Group();
    _flag.setModified(true);
    return *this;
}

RenderGroup::RenderGroup(RenderGroup &&other) noexcept
{
    *this = other;
}

RenderGroup &RenderGroup::operator=(RenderGroup &&other) noexcept
{
    _handle = std::move(other._handle);
    _flag = std::move(other._flag);
    return *this;
}

bool RenderGroup::commit()
{
    if (!_flag)
    {
        return false;
    }
    _flag = false;
    _handle.commit();
    return true;
}

const ospray::cpp::Group &RenderGroup::getHandle() const noexcept
{
    return _handle;
}

RenderGroup RenderGroupFactory::fromGeometry(const GeometryView &geometry)
{
    auto handle = std::vector<ospray::cpp::GeometricModel>{geometry.getHandle()};
    return RenderGroupBuilder::build(RenderGroupParameters::geometry, handle);
}

RenderGroup RenderGroupFactory::fromGeometry(const std::vector<GeometryView> &geometries)
{
    auto handles = HandleListCompiler::compile<ospray::cpp::GeometricModel>(geometries);
    return RenderGroupBuilder::build(RenderGroupParameters::geometry, handles);
}

RenderGroup RenderGroupFactory::fromVolume(const VolumeView &volume)
{
    auto handle = std::vector<ospray::cpp::VolumetricModel>{volume.getHandle()};
    return RenderGroupBuilder::build(RenderGroupParameters::volume, handle);
}

RenderGroup RenderGroupFactory::fromVolume(const std::vector<VolumeView> &volumes)
{
    auto handles = HandleListCompiler::compile<ospray::cpp::VolumetricModel>(volumes);
    return RenderGroupBuilder::build(RenderGroupParameters::volume, handles);
}

RenderGroup RenderGroupFactory::fromClipper(const GeometryView &clipper)
{
    auto handle = std::vector<ospray::cpp::GeometricModel>{clipper.getHandle()};
    return RenderGroupBuilder::build(RenderGroupParameters::clipping, handle);
}

RenderGroup RenderGroupFactory::fromClipper(const std::vector<GeometryView> &clippers)
{
    auto handles = HandleListCompiler::compile<ospray::cpp::GeometricModel>(clippers);
    return RenderGroupBuilder::build(RenderGroupParameters::clipping, handles);
}
}
