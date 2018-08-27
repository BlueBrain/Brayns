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

#include "VRPNPlugin.h"

#include <brayns/common/camera/Camera.h>
#include <brayns/pluginapi/PluginAPI.h>

#include <ospray/SDK/camera/PerspectiveCamera.h>

namespace brayns
{
namespace
{
const vrpn_int32 trackedSensorId = 0;

const std::array<double, 3> openDeckInitPos{0.0, 0.0, 0.0};
const Vector3d openDeckRightDirection{1.0f, 0.0f, 0.0f};

const std::string cameraType = "cylindricStereoTracked";
const std::string openDeckPositionProp = "openDeckPosition";
const std::string openDeckCamDUProp = "openDeckCamDU";

constexpr std::array<double, 3> to_array_3d(const vrpn_float64* pos)
{
    return {pos[0], pos[1], pos[2]};
}
inline std::array<double, 3> to_array_3d(const Vector3d& v)
{
    return {v.x(), v.y(), v.z()};
}

Vector3d rotateVectorByQuat(const Vector3d& v, const vrpn_float64* q)
{
    const auto u = Vector3d{q[0], q[1], q[2]}; // vector part of the quaternion
    const auto s = q[3];                       // scalar part of the quaternion

    return 2.0 * vmml::dot(u, v) * u + (s * s - vmml::dot(u, u)) * v +
           2.0 * s * vmml::cross(u, v);
}

PropertyMap::Property getOpenDeckPositionProperty()
{
    PropertyMap::Property openDeckPosition{openDeckPositionProp,
                                           openDeckPositionProp,
                                           openDeckInitPos};
    openDeckPosition.markReadOnly();
    return openDeckPosition;
}

PropertyMap::Property getOpenDeckCameraDUProperty()
{
    PropertyMap::Property openDeckCamDU{openDeckCamDUProp, openDeckCamDUProp,
                                        to_array_3d(openDeckRightDirection)};
    openDeckCamDU.markReadOnly();
    return openDeckCamDU;
}

PropertyMap::Property getStereoModeProperty()
{
    return {"stereoMode",
            "Stereo mode",
            (int)ospray::PerspectiveCamera::StereoMode::OSP_STEREO_SIDE_BY_SIDE,
            {"None", "Left eye", "Right eye", "Side by side"}};
}

PropertyMap::Property getInterpupillaryDistanceProperty()
{
    return {"interpupillaryDistance", "Eye separation", 0.0635, {0.0, 10.0}};
}

PropertyMap getDefaultCameraProperties()
{
    PropertyMap properties;
    properties.setProperty(getOpenDeckPositionProperty());
    properties.setProperty(getOpenDeckCameraDUProperty());
    properties.setProperty(getStereoModeProperty());
    properties.setProperty(getInterpupillaryDistanceProperty());
    return properties;
}

void trackerCallback(void* userData, const vrpn_TRACKERCB t)
{
    auto camera = static_cast<Camera*>(userData);

    const auto cameraDU = rotateVectorByQuat(openDeckRightDirection, t.quat);

    camera->updateProperty(openDeckPositionProp, to_array_3d(t.pos));
    camera->updateProperty(openDeckCamDUProp, to_array_3d(cameraDU));
}
}

VRPNPlugin::VRPNPlugin(PluginAPI* api, const std::string& vrpnName)
    : _camera{api->getCamera()}
    , _vrpnTracker{vrpnName.c_str()}
{
    if (_vrpnTracker.register_change_handler(&_camera, trackerCallback,
                                             trackedSensorId) == -1)
    {
        throw std::runtime_error("VRPN couldn't connect to: " + vrpnName);
    }
    _camera.setProperties(cameraType, getDefaultCameraProperties());
}

VRPNPlugin::~VRPNPlugin()
{
    _vrpnTracker.unregister_change_handler(&_camera, trackerCallback,
                                           trackedSensorId);
}

void VRPNPlugin::preRender()
{
    _vrpnTracker.mainloop();
}
}

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(
    brayns::PluginAPI* api, int argc BRAYNS_UNUSED, char** argv BRAYNS_UNUSED)
{
    return new brayns::VRPNPlugin(api, "DTrack@cave1");
}
