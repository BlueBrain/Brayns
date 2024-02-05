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

#include <brayns/network/adapters/ModelTransferFunctionAdapter.h>
#include <brayns/network/adapters/TransferFunctionAdapter.h>
#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/network/messages/GetModelMessage.h>

namespace brayns
{
class GetModelTransferFunctionEntrypoint
    : public Entrypoint<GetModelMessage, TransferFunction>
{
public:
    virtual std::string getName() const override
    {
        return "get-model-transfer-function";
    }

    virtual std::string getDescription() const override
    {
        return "Get the transfer function of the given model";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto& engine = getApi().getEngine();
        auto& scene = engine.getScene();
        ExtractModel::fromParams(scene, params);
        auto& transferFunction = scene.getTransferFunction();
        request.reply(transferFunction);
    }
};

class SetModelTransferFunctionEntrypoint
    : public Entrypoint<ModelTransferFunction, EmptyMessage>
{
public:
    virtual std::string getName() const override
    {
        return "set-model-transfer-function";
    }

    virtual std::string getDescription() const override
    {
        return "Set the transfer function of the given model";
    }

    virtual void onRequest(const Request& request) override
    {
        auto& engine = getApi().getEngine();
        auto& scene = engine.getScene();
        ModelTransferFunction transferFunction(scene);
        request.getParams(transferFunction);
        request.reply(EmptyMessage());
    }
};
} // namespace brayns