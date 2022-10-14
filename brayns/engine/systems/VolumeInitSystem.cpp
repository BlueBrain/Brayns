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

#include <brayns/engine/components/ColorRamp.h>
#include <brayns/engine/components/VolumeViews.h>
#include <brayns/engine/components/Volumes.h>

namespace
{
class VolumeInitializer
{
public:
    static void init(brayns::Components &components)
    {
        _initVolumeViews(components);
        _initColorRamp(components);
    }

private:
    static void _initVolumeViews(brayns::Components &components)
    {
        if (components.has<brayns::VolumeViews>())
        {
            return;
        }

        auto &views = components.add<brayns::VolumeViews>();
        auto &volumes = components.get<brayns::Volumes>();
        views.elements.reserve(volumes.elements.size());
        for (auto &volume : volumes.elements)
        {
            views.elements.emplace_back(volume);
        }
    }

    static void _initColorRamp(brayns::Components &components)
    {
        auto &colorRamp = components.getOrAdd<brayns::ColorRamp>();
        auto &views = components.get<brayns::VolumeViews>();
        for (auto &view : views.elements)
        {
            view.setColorRamp(colorRamp);
        }
    }
};
}

namespace brayns
{
void VolumeInitSystem::execute(Components &components)
{
    VolumeInitializer::init(components);
}
}
