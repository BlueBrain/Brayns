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

#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>

#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/network/messages/UpdateModelMessage.h>

namespace brayns
{
class UpdateModelEntrypoint
    : public Entrypoint<UpdateModelMessage, EmptyMessage>
{
public:
    virtual std::string getName() const override { return "update-model"; }

    virtual std::string getDescription() const override
    {
        return "Update the model with the given values";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto& engine = getApi().getEngine();
        auto& scene = engine.getScene();
        auto id = params.id;
        auto model = scene.getModel(id);
        if (!model)
        {
            throw EntrypointException("Unknown model ID: '" +
                                      std::to_string(id) + "'");
        }
        // TODO
        model->computeBounds();
        scene.markModified();
        engine.triggerRender();
        request.reply(EmptyMessage());
    }
};
} // namespace brayns