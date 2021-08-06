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

namespace brayns
{
class LoadersSchemaEntrypoint : public Entrypoint<EmptyMessage, JsonValue>
{
public:
    virtual std::string getName() const override { return "loaders-schema"; }

    virtual std::string getDescription() const override
    {
        return "Get the schema of all loaders";
    }

    virtual void onRequest(const Request& request) override
    {
        auto& engine = getApi().getEngine();
        auto& scene = engine.getScene();
        auto& registry = scene.getLoaderRegistry();
        auto& loaders = registry.getLoaderInfos();
        auto schema = _getSchema(loaders);
        auto json = Json::serialize(schema);
        request.reply(json);
    }

private:
    JsonSchema _getSchema(const std::vector<LoaderInfo>& loaders)
    {
        JsonSchema schema;
        schema.title = "loaders";
        auto& oneOf = schema.oneOf;
        oneOf.reserve(loaders.size());
        for (const auto& loader : loaders)
        {
            oneOf.push_back(_getSchema(loader));
        }
        return schema;
    }

    JsonSchema _getSchema(const LoaderInfo& loader)
    {
        auto& properties = loader.properties;
        auto schema = Json::getSchema(properties);
        auto& name = loader.name;
        schema.title = name;
        return schema;
    }
};
} // namespace brayns