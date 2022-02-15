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

#include "LoadersSchemaEntrypoint.h"

namespace
{
class LoadersSchemaBuilder
{
public:
    static brayns::JsonSchema getSchemas(const std::vector<brayns::LoaderInfo> &loaders)
    {
        brayns::JsonSchema schema;
        schema.title = "loaders";
        auto &oneOf = schema.oneOf;
        oneOf.reserve(loaders.size());
        for (const auto &loader : loaders)
        {
            oneOf.push_back(_getSchema(loader));
        }
        return schema;
    }

private:
    static brayns::JsonSchema _getSchema(const brayns::LoaderInfo &loader)
    {
        auto schema = loader.inputParametersSchema;
        auto &name = loader.name;
        schema.title = name;
        return schema;
    }
};
}

namespace brayns
{
LoadersSchemaEntrypoint::LoadersSchemaEntrypoint(const LoaderRegistry &loaders)
    : _loaders(loaders)
{
}

std::string LoadersSchemaEntrypoint::getName() const
{
    return "loaders-schema";
}

std::string LoadersSchemaEntrypoint::getDescription() const
{
    return "Get the schema of all loaders";
}

void LoadersSchemaEntrypoint::onRequest(const Request &request)
{
    auto &loaders = _loaders.getLoaderInfos();
    auto schema = LoadersSchemaBuilder::getSchemas(loaders);
    auto json = Json::serialize(schema);
    request.reply(json);
}
} // namespace brayns
