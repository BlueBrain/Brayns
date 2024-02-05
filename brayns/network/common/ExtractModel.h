/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#pragma once

#include <brayns/engine/Engine.h>
#include <brayns/engine/Scene.h>

#include <brayns/network/entrypoint/EntrypointException.h>

#include <brayns/pluginapi/PluginAPI.h>

namespace brayns
{
/**
 * @brief Helper class to extract a model from a given source.
 *
 */
class ExtractModel
{
public:
    /**
     * @brief Extract a model from the API using its ID.
     *
     * @param api Brayns API.
     * @param id Model ID.
     * @return ModelDescriptor& Model instance.
     * @throw EntrypointException Model not found.
     */
    static ModelDescriptor& fromId(PluginAPI& api, size_t id)
    {
        return fromId(api.getEngine(), id);
    }

    /**
     * @brief Extract a model from an engine using its ID.
     *
     * @param engine Brayns engine.
     * @param id Model ID.
     * @return ModelDescriptor& Model instance.
     * @throw EntrypointException Model not found.
     */
    static ModelDescriptor& fromId(Engine& engine, size_t id)
    {
        return fromId(engine.getScene(), id);
    }

    /**
     * @brief Extract a model from a scene using its ID.
     *
     * @param engine Brayns scene.
     * @param id Model ID.
     * @return ModelDescriptor& Model instance.
     * @throw EntrypointException Model not found.
     */
    static ModelDescriptor& fromId(Scene& scene, size_t id)
    {
        auto model = scene.getModel(id);
        if (!model)
        {
            throw EntrypointException("No model found with ID " +
                                      std::to_string(id));
        }
        return *model;
    }

    /**
     * @brief Extract a model from a given source and JSON-RPC params.
     *
     * Params must have a public "id" member.
     *
     * @tparam T Source type (API, engine, scene).
     * @tparam U JSON-RPC params type.
     * @param source Source instance.
     * @param params Params instance.
     * @return ModelDescriptor& Model instance.
     * @throw EntrypointException Model not found.
     */
    template <typename T, typename U>
    static ModelDescriptor& fromParams(T& source, const U& params)
    {
        return fromId(source, params.id);
    }

    /**
     * @brief Extract a model from a given source and JSON-RPC request.
     *
     * Request must have a public getParams() method.
     *
     * @tparam T Source type (API, engine, scene).
     * @tparam U JSON-RPC request type.
     * @param source Source instance.
     * @param request Request instance.
     * @return ModelDescriptor& Model instance.
     * @throw EntrypointException Model not found.
     */
    template <typename T, typename U>
    static ModelDescriptor& fromRequest(T& source, const U& request)
    {
        return fromParams(source, request.getParams());
    }
};
} // namespace brayns