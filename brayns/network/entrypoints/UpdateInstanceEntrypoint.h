/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/network/adapters/ModelInstanceAdapter.h>
#include <brayns/network/common/ExtractModel.h>
#include <brayns/network/entrypoint/Entrypoint.h>

namespace brayns
{
class UpdateInstanceEntrypoint : public Entrypoint<ModelInstance, EmptyMessage>
{
public:
    virtual std::string getName() const override { return "update-instance"; }

    virtual std::string getDescription() const override
    {
        return "Update the model instance with the given values";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto modelId = params.getModelID();
        auto instanceId = params.getInstanceID();
        auto& engine = getApi().getEngine();
        auto& scene = engine.getScene();
        auto& model = ExtractModel::fromId(scene, modelId);
        auto instance = model.getInstance(instanceId);
        if (!instance)
        {
            throw EntrypointException(
                "Model with ID " + std::to_string(modelId) +
                " has no instance with ID " + std::to_string(instanceId));
        }
        auto& source = model.getModel();
        source.markInstancesDirty();
        scene.markModified(false);
        engine.triggerRender();
        request.getParams(*instance);
        request.notify(*instance);
        request.reply(EmptyMessage());
    }
};
} // namespace brayns