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

#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/network/entrypoint/EntrypointTask.h>
#include <brayns/network/messages/BinaryParamsAdapter.h>
#include <brayns/network/messages/ModelDescriptorAdapter.h>
#include <brayns/network/tasks/NetworkTaskManager.h>

#include <brayns/tasks/SnapshotTask.h>

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
        return "Request upload of blob to trigger adding of model after blob "
               "has been received and return model descriptor on success";
    }

    virtual JsonSchema getParamsSchema() const override
    {
        auto schema = Json::getSchema<BinaryParam>();
        schema.name = "param";
        schema.description = "size, type, name, transformation, etc";
        return schema;
    }

    virtual bool isAsync() const override { return true; }

    virtual void onRequest(const Request& request) override
    {
        request.reply({});
    }
};
} // namespace brayns