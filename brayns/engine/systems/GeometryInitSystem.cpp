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
#include <brayns/engine/components/Renderable.h>
#include <brayns/engine/material/Material.h>
#include <brayns/engine/material/types/Phong.h>

namespace
{
class GeometryInitializer
{
public:
    GeometryInitializer(brayns::Components &components)
        : _components(components)
    {
    }

    void init()
    {
        _initGeometryViews();
        _initMaterial();
    }

private:
    bool _checkGeometryViewIntegrity()
    {
        auto &geometries = _components.get<brayns::Geometries>();
        auto &views = _components.get<brayns::GeometryViews>();
        return views.elements.size() == geometries.elements.size();
    }

    void _initGeometryViews()
    {
        if (_components.has<brayns::GeometryViews>())
        {
            assert(_checkGeometryViewIntegrity());
            return;
        }

        auto &geometries = _components.get<brayns::Geometries>();
        auto &views = _components.add<brayns::GeometryViews>();
        views.elements.reserve(geometries.elements.size());
        for (auto &geometry : geometries.elements)
        {
            views.elements.emplace_back(geometry);
        }
    }

    void _initMaterial()
    {
        auto material = _components.find<brayns::Material>();
        if (!material)
        {
            material = &_components.add<brayns::Material>(brayns::Phong());
        }

        auto &views = _components.get<brayns::GeometryViews>();
        for (auto &view : views.elements)
        {
            view.setMaterial(*material);
        }
    }

private:
    brayns::Components &_components;
};
}

namespace brayns
{
void GeometryInitSystem::execute(Components &components)
{
    GeometryInitializer initializer(components);
    initializer.init();
}
}
