/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3.0 as
 * published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "LoaderHelper.h"

#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace
{
class LoaderFinder
{
public:
    static brayns::LoaderRef &find(brayns::LoaderRegistry &loaders, const std::string &name, const std::string &format)
    {
        if (!name.empty())
        {
            return findByName(loaders, name);
        }
        return findByFormat(loaders, format);
    }

    static brayns::LoaderRef &findByName(brayns::LoaderRegistry &loaders, const std::string &name)
    {
        auto *loader = loaders.findByName(name);
        if (loader)
        {
            return *loader;
        }
        throw brayns::InvalidParamsException("Invalid loader name: '" + name + "'");
    }

    static brayns::LoaderRef &findByFormat(brayns::LoaderRegistry &loaders, const std::string &format)
    {
        auto *loader = loaders.findByFormat(format);
        if (loader)
        {
            return *loader;
        }
        throw brayns::InvalidParamsException("Unsupported file format: '" + format + "'");
    }
};

class LoaderParamsValidator
{
public:
    static void validate(const brayns::JsonValue &params, const brayns::LoaderRef &loader)
    {
        auto &schema = loader.getSchema();
        auto errors = brayns::Json::validate(params, schema);
        if (!errors.empty())
        {
            throw brayns::InvalidParamsException("Invalid loader params schema", errors);
        }
    }
};
} // namespace

namespace brayns
{
LoaderRef &LoaderHelper::findAndValidate(
    LoaderRegistry &loaders,
    const std::string &name,
    const std::string &format,
    const JsonValue &params)
{
    auto &loader = LoaderFinder::find(loaders, name, format);
    LoaderParamsValidator::validate(params, loader);
    return loader;
}
} // namespace brayns
