/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "RemoveModelEntrypoint.h"

#include <brayns/engine/common/SimulationScanner.h>

namespace brayns
{
RemoveModelEntrypoint::RemoveModelEntrypoint(ModelManager &models, SimulationParameters &simulation):
    _models(models),
    _simulation(simulation)
{
}

std::string RemoveModelEntrypoint::getMethod() const
{
    return "remove-model";
}

std::string RemoveModelEntrypoint::getDescription() const
{
    return "Remove the model(s) from the ID list from the scene";
}

void RemoveModelEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    _models.removeModelInstancesById(params.ids);
    SimulationScanner::scanAndUpdate(_models, _simulation);
    request.reply(EmptyJson());
}
} // namespace brayns
