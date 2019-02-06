/* Copyright (c) 2018, EPFL/Blue Brain Project
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

#include "OpenDeckPlugin.h"

#include <brayns/common/log.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/pluginapi/PluginAPI.h>

namespace brayns
{
namespace
{
constexpr uint32_t openDeckWallResX = 11940u;
constexpr uint32_t openDeckWallResY = 3424u;
constexpr uint32_t openDeckFloorResX = 4096u;
constexpr uint32_t openDeckFloorResY = 2125u;
constexpr char leftWallBufferName[] = "0L";
constexpr char rightWallBufferName[] = "0R";
constexpr char leftFloorBufferName[] = "1L";
constexpr char rightFloorBufferName[] = "1R";
}

OpenDeckPlugin::OpenDeckPlugin(const Vector2ui& wallRes,
                               const Vector2ui& floorRes)
    : _wallRes(wallRes)
    , _floorRes(floorRes)
{
}

void OpenDeckPlugin::init()
{
#ifdef BRAYNS_USE_OSPRAY
    _api->getEngine().addCameraType("cylindric");
    _api->getEngine().addCameraType("cylindricStereo");
    _api->getEngine().addCameraType("cylindricStereoTracked");
#endif
    FrameBufferPtr frameBuffer =
        _api->getEngine().createFrameBuffer(leftWallBufferName, _wallRes,
                                            FrameBufferFormat::rgba_i8);
    _api->getEngine().addFrameBuffer(frameBuffer);
    frameBuffer =
        _api->getEngine().createFrameBuffer(rightWallBufferName, _wallRes,
                                            FrameBufferFormat::rgba_i8);
    _api->getEngine().addFrameBuffer(frameBuffer);
    frameBuffer =
        _api->getEngine().createFrameBuffer(leftFloorBufferName, _floorRes,
                                            FrameBufferFormat::rgba_i8);
    _api->getEngine().addFrameBuffer(frameBuffer);
    frameBuffer =
        _api->getEngine().createFrameBuffer(rightFloorBufferName, _floorRes,
                                            FrameBufferFormat::rgba_i8);
    _api->getEngine().addFrameBuffer(frameBuffer);
}

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(const int argc,
                                                         const char** argv)
{
    if (argc > 2)
    {
        throw std::runtime_error(
            "OpenDeck plugin expects at most one argument, the scale of the "
            "native OpenDeck resolution.");
    }

    const float scaling = (argc == 2) ? std::stof(argv[1]) : 1.0f;

    if (scaling > 1.0f || scaling <= 0.0f)
    {
        throw std::runtime_error(
            "The scale of the native OpenDeck resolution cannot be bigger "
            "than 1.0 or negative.");
    }

    const Vector2ui floorRes(openDeckFloorResX * scaling,
                             openDeckFloorResY * scaling);
    const Vector2ui wallRes(openDeckWallResX * scaling,
                            openDeckWallResY * scaling);
    return new brayns::OpenDeckPlugin(wallRes, floorRes);
}
}
