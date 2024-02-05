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

#include <brayns/engine/Scene.h>

#include <brayns/network/binary/ModelUploadTask.h>
#include <brayns/network/entrypoint/Entrypoint.h>

namespace brayns
{
class RequestModelUploadEntrypoint
    : public Entrypoint<BinaryParam, std::vector<ModelDescriptorPtr>>
{
public:
    virtual std::string getName() const override
    {
        return "request-model-upload";
    }

    virtual std::string getDescription() const override
    {
        return "Request model upload from further received blobs and return "
               "model descriptor on success";
    }

    virtual bool isAsync() const override { return true; }

    virtual void onRequest(const Request& request) override
    {
        auto& engine = getApi().getEngine();
        auto& registry = getApi().getLoaderRegistry();
        auto task = std::make_shared<ModelUploadTask>(engine, registry);
        launchTask(task, request);
        auto& binary = getBinary();
        auto& handle = request.getConnectionHandle();
        binary.addTask(handle, task);
    }
};
} // namespace brayns
