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

#include "RenderableGroup.h"

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

class RawDataUpdater
{
public:
    template<typename HandleType>
    static void update(ospray::cpp::Group &handle, const std::string &param, const std::vector<HandleType> &handles)
    {
        handle.setParam(param, ospray::cpp::SharedData(handles));
        handle.commit();
    }
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
void RenderableGroup::setGeometry(const GeometryView &geometry)
{
    RawDataUpdater::update(
        _handle,
        GroupParameters::geometry,
        std::vector<ospray::cpp::GeometricModel>{geometry.getHandle()});
}

void RenderableGroup::setGeometry(const std::vector<GeometryView> &geometries)
{
    auto handleList = HandleListCompiler::compile<ospray::cpp::GeometricModel>(geometries);
    RawDataUpdater::update(_handle, GroupParameters::geometry, handleList);
}

void RenderableGroup::setVolume(const VolumeView &volume)
{
    RawDataUpdater::update(
        _handle,
        GroupParameters::volume,
        std::vector<ospray::cpp::VolumetricModel>{volume.getHandle()});
}

void RenderableGroup::setVolume(const std::vector<VolumeView> &volumes)
{
    auto handleList = HandleListCompiler::compile<ospray::cpp::VolumetricModel>(volumes);
    RawDataUpdater::update(_handle, GroupParameters::volume, handleList);
}

void RenderableGroup::setClipper(const GeometryView &clipper)
{
    RawDataUpdater::update(
        _handle,
        GroupParameters::clipping,
        std::vector<ospray::cpp::GeometricModel>{clipper.getHandle()});
}

void RenderableGroup::setClipper(const std::vector<GeometryView> &clippers)
{
    auto handleList = HandleListCompiler::compile<ospray::cpp::GeometricModel>(clippers);
    RawDataUpdater::update(_handle, GroupParameters::clipping, handleList);
}

const ospray::cpp::Group &RenderableGroup::getHandle() const noexcept
{
    return _handle;
}
}
