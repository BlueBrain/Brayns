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

#include "ClipperInitSystem.h"

#include <brayns/engine/components/Clippers.h>
#include <brayns/engine/components/GeometryViews.h>
#include <brayns/engine/components/Renderable.h>

namespace brayns
{
void ClipperInitSystem::execute(Components &components)
{
    auto &clippers = components.get<Clippers>();

    auto views = components.find<GeometryViews>();
    if (!views)
    {
        views = &components.add<GeometryViews>();

        views->elements.reserve(clippers.elements.size());
        for (auto &geometry : clippers.elements)
        {
            geometry.commit();
            auto &view = views->elements.emplace_back(geometry);
            view.commit();
        }
    }

    if (!components.has<Renderable>())
    {
        auto &renderable = components.add<Renderable>();
        renderable.group = RenderGroupFactory::fromClippers(views->elements);
    }
}
}
