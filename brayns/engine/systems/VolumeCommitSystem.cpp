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

#include "VolumeCommitSystem.h"

#include <brayns/common/ColorRamp.h>
#include <brayns/engine/components/VolumeViews.h>
#include <brayns/engine/components/Volumes.h>
#include <brayns/engine/model/RenderGroup.h>

namespace brayns
{
CommitResult VolumeCommitSystem::execute(Components &components)
{
    auto &volumes = components.get<Volumes>();
    auto &views = components.get<VolumeViews>();
    auto &colorRamp = components.get<ColorRamp>();
    auto &group = components.get<RenderGroup>();

    auto volumeModified = static_cast<bool>(volumes.modified);
    auto viewsModified = static_cast<bool>(views.modified);

    volumes.modified = false;
    views.modified = false;

    if (colorRamp.isModified())
    {
        colorRamp.resetModified();
        viewsModified = true;
        for (auto &view : views.elements)
        {
            view.setColorRamp(colorRamp);
        }
    }

    if (volumeModified)
    {
        for (auto &volume : volumes.elements)
        {
            volume.commit();
        }
    }
    if (viewsModified)
    {
        for (auto &view : views.elements)
        {
            view.commit();
        }
    }
    auto groupModified = group.commit();

    return {volumeModified || groupModified, viewsModified};
}
}
