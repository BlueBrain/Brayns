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

#include "ClearClippingGeometriesEntrypoint.h"

#include <brayns/core/engine/scene/ModelsOperations.h>

namespace brayns
{
ClearClipPlanesEntrypoint::ClearClipPlanesEntrypoint(ModelManager &models):
    _models(models)
{
}

std::string ClearClipPlanesEntrypoint::getMethod() const
{
    return "clear-clip-planes";
}

std::string ClearClipPlanesEntrypoint::getDescription() const
{
    return "Old clear for clipping geometries, use 'clear-clipping-geometries' instead";
}

bool ClearClipPlanesEntrypoint::isDeprecated() const
{
    return true;
}

void ClearClipPlanesEntrypoint::onRequest(const Request &request)
{
    ModelsOperations::removeClippers(_models);
    request.reply(EmptyJson());
}

ClearClippingGeometriesEntrypoint::ClearClippingGeometriesEntrypoint(ModelManager &models):
    _models(models)
{
}

std::string ClearClippingGeometriesEntrypoint::getMethod() const
{
    return "clear-clipping-geometries";
}

std::string ClearClippingGeometriesEntrypoint::getDescription() const
{
    return "Clear all clipping geometries in the scene";
}

void ClearClippingGeometriesEntrypoint::onRequest(const Request &request)
{
    ModelsOperations::removeClippers(_models);
    request.reply(EmptyJson());
}
} // namespace brayns
