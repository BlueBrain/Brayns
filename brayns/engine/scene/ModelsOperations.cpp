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

#include "ModelsOperations.h"

#include <brayns/engine/components/ClipperViews.h>
#include <brayns/engine/components/Lights.h>

namespace brayns
{
void ModelsOperations::removeLights(ModelManager &models)
{
    removeModelsWithComponent<Lights>(models);
}

void ModelsOperations::removeClippers(ModelManager &models)
{
    removeModelsWithComponent<ClipperViews>(models);
}

void ModelsOperations::removeObjects(ModelManager &models)
{
    auto &instances = models.getAllModelInstances();

    std::vector<uint32_t> ids;
    ids.reserve(instances.size());

    for (auto instance : instances)
    {
        auto &model = instance->getModel();
        auto &components = model.getComponents();
        if (components.has<Lights>() || components.has<ClipperViews>())
        {
            continue;
        }
        ids.push_back(instance->getID());
    }

    models.removeModelInstances(ids);
}
}
