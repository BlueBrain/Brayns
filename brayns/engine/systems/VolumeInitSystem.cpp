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

#include "VolumeInitSystem.h"

#include <brayns/common/ColorRamp.h>
#include <brayns/engine/components/Renderable.h>
#include <brayns/engine/components/VolumeViews.h>
#include <brayns/engine/components/Volumes.h>

#include <ospray/ospray_cpp/Data.h>

namespace
{
struct GroupParameters
{
    inline static const std::string volume = "volume";
};
class VolumeInitializer
{
public:
    VolumeInitializer(brayns::Components &components)
        : _components(components)
    {
    }

    void init()
    {
        auto &views = _initViews();
        assert(_checkViews(views));
        _initColorRamp(views);
        _initGroup(views);
    }

private:
    brayns::VolumeViews &_initViews()
    {
        if (auto views = _components.find<brayns::VolumeViews>())
        {
            return *views;
        }

        auto &views = _components.add<brayns::VolumeViews>();
        _constructViews(views);
        return views;
    }

    void _constructViews(brayns::VolumeViews &views)
    {
        auto &volumes = _components.get<brayns::Volumes>();
        auto &volumeList = volumes.elements;

        auto &viewList = views.elements;
        viewList.reserve(volumeList.size());
        for (auto &volume : volumeList)
        {
            volume.commit();
            viewList.emplace_back(volume);
        }
    }

    bool _checkViews(brayns::VolumeViews &views)
    {
        auto &volumes = _components.get<brayns::Volumes>();
        return volumes.elements.size() == views.elements.size();
    }

    void _initColorRamp(brayns::VolumeViews &views)
    {
        auto &colorRamp = _components.getOrAdd<brayns::ColorRamp>();
        for (auto &view : views.elements)
        {
            view.setColorRamp(colorRamp);
        }
    }

    void _initGroup(brayns::VolumeViews &views)
    {
        if (_components.has<brayns::Renderable>())
        {
            return;
        }

        auto &renderable = _components.add<brayns::Renderable>();
        renderable.group = brayns::RenderGroupFactory::fromVolumes(views.elements);
    }

private:
    brayns::Components &_components;
};
}

namespace brayns
{
void VolumeInitSystem::execute(Components &components)
{
    VolumeInitializer initializer(components);
    initializer.init();
}
}
