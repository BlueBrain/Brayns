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

#include "GeometryDataSystem.h"

#include <brayns/engine/components/ColorSolid.h>
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
        _initColor(components);
    }

private:
    static void _initGeometryViews(brayns::Components &components)
    {
        if (components.has<brayns::GeometryViews>())
        {
            return;
        }

        auto &geometries = components.get<brayns::Geometries>();
        components.add<brayns::GeometryViews>(geometries.elements);
    }

    static void _initMaterial(brayns::Components &components)
    {
        auto &material = components.getOrAdd<brayns::Material>();
        auto &views = components.get<brayns::GeometryViews>();
        for (auto &view : views.elements)
        {
            view.setMaterial(material);
        }
    }

    static void _initColor(brayns::Components &components)
    {
        auto solidColor = components.find<brayns::ColorSolid>();
        if (!solidColor)
        {
            return;
        }

        auto &views = components.get<brayns::GeometryViews>();
        for (auto &view : views.elements)
        {
            view.setColor(solidColor->color);
        }
    }
};

class GeometryCommitter
{
public:
    static bool commitMaterial(brayns::Material &material, brayns::GeometryViews &views)
    {
        if (!material.commit())
        {
            return false;
        }
        for (auto &view : views.elements)
        {
            view.setMaterial(material);
        }
        return true;
    }

    static bool commitGeometries(brayns::Geometries &geometries)
    {
        if (!geometries.modified)
        {
            return false;
        }

        geometries.modified.setModified(false);
        for (auto &geometry : geometries.elements)
        {
            geometry.commit();
        }
        return true;
    }

    static bool commitGeometryViews(brayns::GeometryViews &views, bool force)
    {
        if (!views.modified && !force)
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
void GeometryDataSystem::init(Components &components)
{
    GeometryInitializer::init(components);
}

CommitResult GeometryDataSystem::commit(Components &components)
{
    auto &geometries = components.get<Geometries>();
    auto &views = components.get<GeometryViews>();
    auto &material = components.get<Material>();

    bool rebuildBVH = false;
    rebuildBVH |= GeometryCommitter::commitGeometries(geometries);

    auto matModified = GeometryCommitter::commitMaterial(material, views);
    auto viewModified = GeometryCommitter::commitGeometryViews(views, matModified);
    auto renderFrame = matModified || viewModified;

    return {rebuildBVH, renderFrame};
}
}
