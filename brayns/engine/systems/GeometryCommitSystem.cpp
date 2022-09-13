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

#include "GeometryCommitSystem.h"

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>
#include <brayns/engine/material/Material.h>
#include <brayns/engine/model/RenderGroup.h>

namespace brayns
{
CommitResult GeometryCommitSystem::execute(Components &components)
{
    auto &geometries = components.get<Geometries>();
    auto &views = components.get<GeometryViews>();
    auto &material = components.get<Material>();
    auto &group = components.get<RenderGroup>();

    auto geometryModified = static_cast<bool>(geometries.modified);
    auto viewModified = static_cast<bool>(views.modified);

    geometries.modified = false;
    views.modified = false;

    if (material.commit())
    {
        for (auto &view : views.elements)
        {
            view.setMaterial(material);
        }
        viewModified = true;
    }
    if (viewModified)
    {
        for (auto &view : views.elements)
        {
            view.commit();
        }
    }
    if (geometryModified)
    {
        for (auto &geometry : geometries.elements)
        {
            geometry.commit();
        }
    }
    auto groupModified = group.commit();

    return {geometryModified || groupModified, viewModified};
}
}
