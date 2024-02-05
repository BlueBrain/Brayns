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

#include "OpenDeckPlugin.h"

#include <brayns/common/Log.h>
#include <brayns/common/PixelFormat.h>
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

const std::string HEAD_POSITION_PROP = "headPosition";
const std::string HEAD_ROTATION_PROP = "headRotation";

Vector3d HEAD_INIT_POS = {0.0, 2.0, 0.0};
Vector4d HEAD_INIT_ROT = {0.0, 0.0, 0.0, 1.0};

Property getHeadPositionProperty()
{
    Property headPosition{HEAD_POSITION_PROP, HEAD_INIT_POS};
    headPosition.setReadOnly(true);
    return headPosition;
}

Property getHeadRotationProperty()
{
    Property headRotation{HEAD_ROTATION_PROP, HEAD_INIT_ROT};
    headRotation.setReadOnly(true);
    return headRotation;
}

Property getStereoModeProperty()
{
    return {"stereoMode",
            {3, // side-by-side
             {"None", "Left eye", "Right eye", "Side by side"}},
            {"Stereo mode"}};
}

Property getInterpupillaryDistanceProperty()
{
    return {"interpupillaryDistance", 0.0635, {"Eye separation"}};
}

Property getCameraScalingProperty(const double scaling)
{
    return {PARAM_CAMERA_SCALING, scaling, {"Camera scaling"}};
}

PropertyMap getCylindricStereoProperties()
{
    PropertyMap properties;
    properties.add(getStereoModeProperty());
    properties.add(getInterpupillaryDistanceProperty());
    return properties;
}

PropertyMap getCylindricStereoTrackedProperties(
    const OpenDeckParameters& params)
{
    PropertyMap properties;
    properties.add(getHeadPositionProperty());
    properties.add(getHeadRotationProperty());
    properties.add(getStereoModeProperty());
    properties.add(getInterpupillaryDistanceProperty());
    properties.add(getCameraScalingProperty(params.getCameraScaling()));
    return properties;
}
} // namespace

OpenDeckPlugin::OpenDeckPlugin(OpenDeckParameters&& params)
    : _params(std::move(params))
{
    if (_params.getResolutionScaling() > 1.0f ||
        _params.getResolutionScaling() <= 0.0f)
    {
        throw std::runtime_error(
            "The scale of the native OpenDeck resolution cannot be bigger "
            "than 1.0, zero or negative.");
    }
    if (_params.getCameraScaling() <= 0.0)
        throw std::runtime_error("The camera scale cannot be zero or negative");

    _wallRes = Vector2ui(openDeckWallResX * _params.getResolutionScaling(),
                         openDeckWallResY * _params.getResolutionScaling());
    _floorRes = Vector2ui(openDeckFloorResX * _params.getResolutionScaling(),
                          openDeckFloorResY * _params.getResolutionScaling());
}

void OpenDeckPlugin::init()
{
    auto& engine = _api->getEngine();

    engine.addCameraType("cylindric");
    engine.addCameraType("cylindricStereo", getCylindricStereoProperties());
    engine.addCameraType("cylindricStereoTracked",
                         getCylindricStereoTrackedProperties(_params));

    FrameBufferPtr frameBuffer =
        engine.createFrameBuffer(leftWallBufferName, _wallRes,
                                 PixelFormat::RGBA_I8);
    engine.addFrameBuffer(frameBuffer);
    frameBuffer = engine.createFrameBuffer(rightWallBufferName, _wallRes,
                                           PixelFormat::RGBA_I8);
    engine.addFrameBuffer(frameBuffer);
    frameBuffer = engine.createFrameBuffer(leftFloorBufferName, _floorRes,
                                           PixelFormat::RGBA_I8);
    engine.addFrameBuffer(frameBuffer);
    frameBuffer = engine.createFrameBuffer(rightFloorBufferName, _floorRes,
                                           PixelFormat::RGBA_I8);
    engine.addFrameBuffer(frameBuffer);
}

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(const int argc,
                                                         const char** argv)
{
    brayns::OpenDeckParameters params;
    if (!params.parse(argc, argv))
        return nullptr;
    try
    {
        return new brayns::OpenDeckPlugin(std::move(params));
    }
    catch (const std::runtime_error& exc)
    {
        std::cerr << exc.what() << std::endl;
        return nullptr;
    }
}
} // namespace brayns
