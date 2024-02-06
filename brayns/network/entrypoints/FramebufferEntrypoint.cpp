/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include "FramebufferEntrypoint.h"

#include <brayns/engine/framebuffer/types/ProgressiveFrameHandler.h>
#include <brayns/engine/framebuffer/types/StaticFrameHandler.h>

namespace brayns
{
SetStaticFramebufferEntrypoint::SetStaticFramebufferEntrypoint(Engine &engine):
    _engine(engine)
{
}

std::string SetStaticFramebufferEntrypoint::getMethod() const
{
    return "set-framebuffer-static";
}

std::string SetStaticFramebufferEntrypoint::getDescription() const
{
    return "Stablishes a static frame rendering on the engine";
}

void SetStaticFramebufferEntrypoint::onRequest(const Request &request)
{
    (void)request;

    auto &framebuffer = _engine.getFramebuffer();
    framebuffer.setFrameHandler(std::make_unique<StaticFrameHandler>());

    request.reply(EmptyJson());
}

SetProgressiveFramebufferEntrypoint::SetProgressiveFramebufferEntrypoint(Engine &engine):
    _engine(engine)
{
}

std::string SetProgressiveFramebufferEntrypoint::getMethod() const
{
    return "set-framebuffer-progressive";
}

std::string SetProgressiveFramebufferEntrypoint::getDescription() const
{
    return "Stablishes a progressive-resolution frame rendering on the engine";
}

void SetProgressiveFramebufferEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto scale = params.scale;

    auto &framebuffer = _engine.getFramebuffer();
    framebuffer.setFrameHandler(std::make_unique<ProgressiveFrameHandler>(scale));

    request.reply(EmptyJson());
}
}
