/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#pragma once

#include <brayns/network/message/Message.h>

#include <brayns/engine/Scene.h>

#include "BoxAdapter.h"
#include "ModelDescriptorMessage.h"

namespace brayns
{
BRAYNS_MESSAGE_BEGIN(SceneMessage)
BRAYNS_MESSAGE_ENTRY(Boxd, bounds, "Scene boundary")
BRAYNS_MESSAGE_ENTRY(std::vector<ModelDescriptorMessage>, models, "All models")

static Scene& extract(PluginAPI& api)
{
    auto& engine = api.getEngine();
    return engine.getScene();
}

void dump(Scene& scene) const
{
    scene.setBounds(bounds);
}

void load(const Scene& scene)
{
    bounds = scene.getBounds();
}

BRAYNS_MESSAGE_END()
} // namespace brayns