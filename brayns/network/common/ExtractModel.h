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
    static ModelDescriptor& fromId(PluginAPI& api, size_t id)
    {
        return fromId(api.getEngine(), id);
    }

    static ModelDescriptor& fromId(Engine& engine, size_t id)
    {
        return fromId(engine.getScene(), id);
    }

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

    template <typename T, typename U>
    static ModelDescriptor& fromParams(T& source, const U& params)
    {
        return fromId(source, params.id);
    }

    template <typename T, typename U>
    static ModelDescriptor& fromRequest(T& source, const U& request)
    {
        return fromParams(source, request.getParams());
    }
};
} // namespace brayns