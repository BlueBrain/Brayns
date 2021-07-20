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

#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/Scene.h>

#include <brayns/parameters/AnimationParameters.h>
#include <brayns/parameters/ApplicationParameters.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/parameters/RenderingParameters.h>
#include <brayns/parameters/VolumeParameters.h>

#include <brayns/pluginapi/PluginAPI.h>

namespace brayns
{
template <typename T>
struct ObjectExtractor
{
};

template <>
struct ObjectExtractor<AnimationParameters>
{
    static AnimationParameters& extract(PluginAPI& api)
    {
        auto& parametersManager = api.getParametersManager();
        return parametersManager.getAnimationParameters();
    }
};

template <>
struct ObjectExtractor<ApplicationParameters>
{
    static ApplicationParameters& extract(PluginAPI& api)
    {
        auto& parametersManager = api.getParametersManager();
        return parametersManager.getApplicationParameters();
    }
};

template <>
struct ObjectExtractor<Camera>
{
    static Camera& extract(PluginAPI& api)
    {
        auto& engine = api.getEngine();
        return engine.getCamera();
    }
};

template <>
struct ObjectExtractor<RenderingParameters>
{
    static RenderingParameters& extract(PluginAPI& api)
    {
        auto& parametersManager = api.getParametersManager();
        return parametersManager.getRenderingParameters();
    }
};

template <>
struct ObjectExtractor<Scene>
{
    static Scene& extract(PluginAPI& api)
    {
        auto& engine = api.getEngine();
        return engine.getScene();
    }
};

template <>
struct ObjectExtractor<VolumeParameters>
{
    static VolumeParameters& extract(PluginAPI& api)
    {
        auto& parametersManager = api.getParametersManager();
        return parametersManager.getVolumeParameters();
    }
};
} // namespace brayns