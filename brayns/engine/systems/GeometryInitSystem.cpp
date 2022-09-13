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
#include <brayns/engine/material/types/Phong.h>
#include <brayns/engine/model/RenderGroup.h>

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
        auto &views = _initViews();
        _initMaterial(views);
        _initGroup(views);
    }

private:
    brayns::GeometryViews &_initViews()
    {
        if (auto views = _components.find<brayns::GeometryViews>())
        {
            return *views;
        }
        auto &views = _components.add<brayns::GeometryViews>();
        _constructViews(views);
        return views;
    }

    void _constructViews(brayns::GeometryViews &views)
    {
        auto &geometries = _components.get<brayns::Geometries>();
        auto &geometryList = geometries.elements;
        auto &viewList = views.elements;
        viewList.reserve(geometryList.size());
        for (auto &geometry : geometryList)
        {
            viewList.emplace_back(geometry);
        }
    }

    void _initMaterial(brayns::GeometryViews &views)
    {
        auto material = _components.find<brayns::Material>();
        if (!material)
        {
            material = &_components.add<brayns::Material>(brayns::Phong());
        }

        for (auto &view : views.elements)
        {
            view.setMaterial(*material);
        }
    }

    void _initGroup(brayns::GeometryViews &views)
    {
        if (auto group = _components.find<brayns::RenderGroup>())
        {
            return;
        }
        auto group = brayns::RenderGroupFactory::fromGeometry(views.elements);
        _components.add<brayns::RenderGroup>(std::move(group));
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
