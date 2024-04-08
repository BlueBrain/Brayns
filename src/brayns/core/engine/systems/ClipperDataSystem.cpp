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

#include "ClipperDataSystem.h"

#include <brayns/core/engine/components/ClipperViews.h>
#include <brayns/core/engine/components/Geometries.h>

namespace
{
class ClippingInitializer
{
public:
    static void init(brayns::Components &components)
    {
        if (components.has<brayns::ClipperViews>())
        {
            _commit(components);
            return;
        }

        _createAndCommit(components);
    }

private:
    static void _createAndCommit(brayns::Components &components)
    {
        auto &geometries = components.get<brayns::Geometries>();
        auto &views = components.add<brayns::ClipperViews>();
        views.elements.reserve(geometries.elements.size());
        for (auto &geometry : geometries.elements)
        {
            geometry.commit();
            auto &view = views.elements.emplace_back(geometry);
            view.commit();
        }
    }

    static void _commit(brayns::Components &components)
    {
        auto &geometries = components.get<brayns::Geometries>();
        auto &views = components.get<brayns::ClipperViews>();

        assert(geometries.elements.size() == views.elements.size());
        for (size_t i = 0; i < geometries.elements.size(); ++i)
        {
            geometries.elements[i].commit();
            views.elements[i].commit();
        }
    }
};
}

namespace brayns
{
void ClipperDataSystem::init(Components &components)
{
    ClippingInitializer::init(components);
}
}
