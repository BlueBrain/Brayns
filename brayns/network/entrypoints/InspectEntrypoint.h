/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#pragma once

#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/network/messages/InspectMessage.h>

#include <brayns/network/adapters/RendererPickResultAdapter.h>

namespace brayns
{
class InspectEntrypoint : public Entrypoint<InspectMessage, Renderer::PickResult>
{
public:
    virtual std::string getName() const override
    {
        return "inspect";
    }

    virtual std::string getDescription() const override
    {
        return "Inspect the scene at x-y position";
    }

    virtual void onRequest(const Request &request) override
    {
        auto params = request.getParams();
        auto position = Vector2f(params.position);
        auto &engine = getApi().getEngine();
        auto &renderer = engine.getRenderer();
        auto result = renderer.pick(position);
        request.reply(result);
    }
};
} // namespace brayns