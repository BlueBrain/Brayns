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

#include "VolumeDataSystem.h"

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

class VolumeCommitter
{
public:
    static bool commitColorRamp(brayns::ColorRamp &colorRamp, brayns::VolumeViews &views)
    {
        if (!colorRamp.isModified())
        {
            return false;
        }
        colorRamp.resetModified();
        for (auto &view : views.elements)
        {
            view.setColorRamp(colorRamp);
        }
        return true;
    }

    static bool commitVolumes(brayns::Volumes &volumes)
    {
        if (!volumes.modified)
        {
            return false;
        }

        volumes.modified.setModified(false);
        for (auto &volume : volumes.elements)
        {
            volume.commit();
        }
        return true;
    }

    static bool commitVolumeViews(brayns::VolumeViews &views)
    {
        if (!views.modified)
        {
            return false;
        }

        views.modified.setModified(false);
        for (auto &view : views.elements)
        {
            view.commit();
        }
        return true;
    }
};
}

namespace brayns
{
void VolumeDataSystem::init(Components &components)
{
    VolumeInitializer::init(components);
}

CommitResult VolumeDataSystem::commit(Components &components)
{
    auto &volumes = components.get<Volumes>();
    auto &views = components.get<VolumeViews>();
    auto &colorRamp = components.get<ColorRamp>();

    bool rebuildBVH = false;
    rebuildBVH |= VolumeCommitter::commitVolumes(volumes);

    bool renderFrame = false;
    renderFrame |= VolumeCommitter::commitColorRamp(colorRamp, views);
    renderFrame |= VolumeCommitter::commitVolumeViews(views);

    return {rebuildBVH, renderFrame};
}
}
