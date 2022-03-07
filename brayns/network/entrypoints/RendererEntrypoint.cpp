/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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
SetRendererInteractiveEntrypoint::SetRendererInteractiveEntrypoint(Engine &engine)
 : SetRendererEntrypoint<InteractiveRenderer>(engine)
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

SetRendererProductionEntrypoint::SetRendererProductionEntrypoint(Engine &engine)
 : SetRendererEntrypoint<ProductionRenderer>(engine)
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

GetRendererEntrypoint::GetRendererEntrypoint(Engine &engine, RendererFactory::Ptr factory)
    : _engine(engine)
    , _rendererFactory(factory)
{
}

std::string GetRendererEntrypoint::getMethod() const
{
    return "get-renderer";
}

std::string GetRendererEntrypoint::getDescription() const
{
    return "Get the current system renderer";
}

void GetRendererEntrypoint::onRequest(const Request &request)
{
    auto& renderer = _engine.getRenderer();

    GenericRenderer result;
    result.serialize(*_rendererFactory, renderer);

    request.reply(result);
}
} // namespace brayns
