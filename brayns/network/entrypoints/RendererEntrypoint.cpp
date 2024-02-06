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

#include "RendererEntrypoint.h"

namespace brayns
{
GetRendererTypeEntrypoint::GetRendererTypeEntrypoint(Engine &engine):
    _engine(engine)
{
}

std::string GetRendererTypeEntrypoint::getMethod() const
{
    return "get-renderer-type";
}

std::string GetRendererTypeEntrypoint::getDescription() const
{
    return "Returns the type of the renderer currently being used";
}

void GetRendererTypeEntrypoint::onRequest(const Request &request)
{
    auto &renderer = _engine.getRenderer();
    auto name = renderer.getName();
    request.reply(name);
}

SetRendererInteractiveEntrypoint::SetRendererInteractiveEntrypoint(Engine &engine):
    SetRendererEntrypoint<Interactive>(engine)
{
}

std::string SetRendererInteractiveEntrypoint::getMethod() const
{
    return "set-renderer-interactive";
}

std::string SetRendererInteractiveEntrypoint::getDescription() const
{
    return "Sets the system renderer to the interactive one";
}

SetRendererProductionEntrypoint::SetRendererProductionEntrypoint(Engine &engine):
    SetRendererEntrypoint<Production>(engine)
{
}

std::string SetRendererProductionEntrypoint::getMethod() const
{
    return "set-renderer-production";
}

std::string SetRendererProductionEntrypoint::getDescription() const
{
    return "Sets the system renderer to the production one";
}

GetRendererInteractiveEntrypoint::GetRendererInteractiveEntrypoint(Engine &engine):
    GetRendererEntrypoint<Interactive>(engine)
{
}

std::string GetRendererInteractiveEntrypoint::getMethod() const
{
    return "get-renderer-interactive";
}

std::string GetRendererInteractiveEntrypoint::getDescription() const
{
    return "Returns the current renderer as interactive renderer, if possible";
}

GetRendererProductionEntrypoint::GetRendererProductionEntrypoint(Engine &engine):
    GetRendererEntrypoint<Production>(engine)
{
}

std::string GetRendererProductionEntrypoint::getMethod() const
{
    return "get-renderer-production";
}

std::string GetRendererProductionEntrypoint::getDescription() const
{
    return "Returns the current renderer as production renderer, if possible";
}
} // namespace brayns
