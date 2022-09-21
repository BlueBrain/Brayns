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

#include "GeometryInitSystem.h"

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>
#include <brayns/engine/material/Material.h>

namespace
{
class GeometryInitializer
{
public:
    static void init(brayns::Components &components)
    {
        _initGeometryViews(components);
        _initMaterial(components);
    }

private:
    static void _initGeometryViews(brayns::Components &components)
    {
        if (components.has<brayns::GeometryViews>())
        {
            return;
        }

        auto &geometries = components.get<brayns::Geometries>();
        auto &views = components.add<brayns::GeometryViews>();
        views.elements.reserve(geometries.elements.size());
        for (auto &geometry : geometries.elements)
        {
            views.elements.emplace_back(geometry);
        }
    }

    static void _initMaterial(brayns::Components &components)
    {
        auto material = components.getOrAdd<brayns::Material>();
        auto &views = components.get<brayns::GeometryViews>();
        for (auto &view : views.elements)
        {
            view.setMaterial(material);
        }
    }
};
}

namespace brayns
{
void GeometryInitSystem::execute(Components &components)
{
    GeometryInitializer::init(components);
}
}
