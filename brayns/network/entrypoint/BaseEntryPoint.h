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

#include <brayns/network/messages/Json.h>
#include <brayns/network/messages/JsonSchema.h>
#include <brayns/network/socket/NetworkRequest.h>

#include <brayns/pluginapi/PluginAPI.h>

#include "EntryPoint.h"
#include "EntryPointException.h"
#include "EntryPointSchema.h"

namespace brayns
{
/**
 * @brief Entrypoint common implementation used as base class.
 *
 */
class BaseEntryPoint : public EntryPoint
{
public:
    /**
     * @brief Get a reference to Brayns API.
     *
     * @return PluginAPI& Stored reference to Brayns API.
     */
    PluginAPI& getApi() const { return *_api; }

    /**
     * @brief Setup the name of the entrypoint.
     *
     * @param name Name of the entrypoint.
     */
    void setName(const std::string& name) { _name = name; }

    /**
     * @brief Setup the description of the entrypoint.
     *
     * @param description Description of the entrypoint.
     */
    void setDescription(const std::string& description)
    {
        _description = description;
    }

    /**
     * @brief Setup the schema of the entrypoint.
     *
     * @param schema Schema of the entrypoint.
     */
    void setSchema(const EntryPointSchema& schema) { _schema = schema; }

    /**
     * @brief Store the API reference inside instance for child reuse.
     *
     * @param api A reference to Brayns API context.
     */
    virtual void setApi(PluginAPI& api) override { _api = &api; }

    /**
     * @brief Must return the name of the entrypoint.
     *
     * @return const std::string& The name (path) of the entrypoint.
     */
    virtual const std::string& getName() const override { return _name; }

    /**
     * @brief Must return a description of the entrypoint.
     *
     * @return const std::string& A user-defined description.
     */
    virtual const std::string& getDescription() const override
    {
        return _description;
    }

    /**
     * @brief Must return the JSON schema of the entrypoint.
     *
     * @return const EntryPointSchema& The entrypoint JSON schema.
     */
    virtual const EntryPointSchema& getSchema() const override
    {
        return _schema;
    }

private:
    PluginAPI* _api = nullptr;
    std::string _name;
    std::string _description;
    EntryPointSchema _schema;
};
} // namespace brayns