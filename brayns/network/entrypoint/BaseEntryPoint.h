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

#include <brayns/network/interface/ActionInterface.h>
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
     * @brief Load the title in entrypoint schema.
     *
     * @param name Name of the entrypoint.
     */
    void setName(const std::string& name) { _schema.title = name; }

    /**
     * @brief Load the description in the entrypoint schema.
     *
     * @param description Description of the entrypoint.
     */
    void setDescription(const std::string& description)
    {
        _schema.description = description;
    }

    /**
     * @brief Load the schema of the entrypoint (override old one).
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
     * @brief Return the schema of the entrypoint stored in the instance.
     *
     * @return const EntryPointSchema& The entrypoint schema.
     */
    virtual const EntryPointSchema& getSchema() const override
    {
        return _schema;
    }

private:
    PluginAPI* _api = nullptr;
    EntryPointSchema _schema;
};
} // namespace brayns