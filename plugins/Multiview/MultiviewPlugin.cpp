/* Copyright 2018-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
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

#include "MultiviewPlugin.h"

#include <brayns/common/Log.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/pluginapi/PluginAPI.h>

constexpr auto PARAM_ARM_LENGTH = "armLength";
constexpr auto PARAM_HEIGHT = "height";

namespace brayns
{
MultiviewPlugin::MultiviewPlugin(PropertyMap&& properties)
    : _properties(std::move(properties))
{
    const double armLength = _properties[PARAM_ARM_LENGTH].as<double>();
    if (armLength <= 0.0f)
    {
        throw std::runtime_error(
            "The multiview camera arm length must be stricly positive");
    }
}

void MultiviewPlugin::init() {}
} // namespace brayns

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(const int argc,
                                                         const char** argv)
{
    brayns::PropertyMap properties;
    properties.add({PARAM_ARM_LENGTH,
                    5.0,
                    {"Arm length",
                     "The distance between the cameras and the view center"}});
    properties.add(
        {PARAM_HEIGHT,
         10.0,
         {"View height", "The height of the viewport in world space"}});

    if (!properties.parse(argc, argv))
        return nullptr;
    try
    {
        return new brayns::MultiviewPlugin(std::move(properties));
    }
    catch (const std::runtime_error& exc)
    {
        std::cerr << exc.what() << std::endl;
        return nullptr;
    }
}
