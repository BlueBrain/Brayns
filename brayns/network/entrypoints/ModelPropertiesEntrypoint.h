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

#include <brayns/network/common/ExtractModel.h>
#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/network/messages/GetModelMessage.h>
#include <brayns/network/messages/ModelPropertiesMessage.h>

namespace brayns
{
class GetModelPropertiesEntrypoint
    : public Entrypoint<GetModelMessage, PropertyMap>
{
public:
    virtual std::string getName() const override
    {
        return "get-model-properties";
    }

    virtual std::string getDescription() const override
    {
        return "Get the properties of the given model";
    }

    virtual void onRequest(const Request& request) override
    {
        auto& model = ExtractModel::fromRequest(getApi(), request);
        request.reply(model.getProperties());
    }
};

class SetModelPropertiesEntrypoint
    : public Entrypoint<ModelPropertiesMessage, EmptyMessage>
{
public:
    virtual std::string getName() const override
    {
        return "set-model-properties";
    }

    virtual std::string getDescription() const override
    {
        return "Set the properties of the given model";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto& newProperties = params.properties;
        auto& model = ExtractModel::fromParams(getApi(), params);
        auto oldProperties = model.getProperties();
        oldProperties.merge(newProperties);
        model.setProperties(oldProperties);
        request.reply(EmptyMessage());
    }
};

class ModelPropertiesSchemaEntrypoint
    : public Entrypoint<GetModelMessage, JsonValue>
{
public:
    virtual std::string getName() const override
    {
        return "model-properties-schema";
    }

    virtual std::string getDescription() const override
    {
        return "Get the property schema of the model";
    }

    virtual void onRequest(const Request& request) override
    {
        auto& model = ExtractModel::fromRequest(getApi(), request);
        auto& properties = model.getProperties();
        auto schema = Json::getSchema(properties);
        auto result = Json::serialize(schema);
        request.reply(result);
    }
};
} // namespace brayns