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

#include "ModelPropertiesEntrypoint.h"

#include <brayns/network/common/ExtractModel.h>

namespace brayns
{
GetModelPropertiesEntrypoint::GetModelPropertiesEntrypoint(Scene &scene)
    : _scene(scene)
{
}

std::string GetModelPropertiesEntrypoint::getName() const
{
    return "get-model-properties";
}

std::string GetModelPropertiesEntrypoint::getDescription() const
{
    return "Get the properties of the given model";
}

void GetModelPropertiesEntrypoint::onRequest(const Request &request)
{
    auto &model = ExtractModel::fromRequest(_scene, request);
    request.reply(model.getProperties());
}

SetModelPropertiesEntrypoint::SetModelPropertiesEntrypoint(Scene &scene)
    : _scene(scene)
{
}

std::string SetModelPropertiesEntrypoint::getName() const
{
    return "set-model-properties";
}

std::string SetModelPropertiesEntrypoint::getDescription() const
{
    return "Set the properties of the given model";
}

void SetModelPropertiesEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto &newProperties = params.properties;
    auto &model = ExtractModel::fromParams(_scene, params);
    auto oldProperties = model.getProperties();
    oldProperties.merge(newProperties);
    model.setProperties(oldProperties);
    request.reply(EmptyMessage());
}

ModelPropertiesSchemaEntrypoint::ModelPropertiesSchemaEntrypoint(Scene &scene)
    : _scene(scene)
{
}

std::string ModelPropertiesSchemaEntrypoint::getName() const
{
    return "model-properties-schema";
}

std::string ModelPropertiesSchemaEntrypoint::getDescription() const
{
    return "Get the property schema of the model";
}

void ModelPropertiesSchemaEntrypoint::onRequest(const Request &request)
{
    auto &model = ExtractModel::fromRequest(_scene, request);
    auto &properties = model.getProperties();
    auto schema = Json::getSchema(properties);
    auto result = Json::serialize(schema);
    request.reply(result);
}
} // namespace brayns
