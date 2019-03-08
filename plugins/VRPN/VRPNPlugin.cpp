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

#include <brayns/common/log.h>
#include <brayns/engine/Camera.h>
#include <brayns/pluginapi/PluginAPI.h>

namespace brayns
{
namespace
{
constexpr vrpn_int32 HEAD_SENSOR_ID = 0;
constexpr vrpn_int32 FLYSTICK_SENSOR_ID = 1;
constexpr double MOVING_SPEED = 1.0f;
const std::string DEFAULT_VRPN_NAME = "DTrack@cave1";
#ifdef BRAYNSVRPN_USE_LIBUV
constexpr int VRPN_IDLE_TIMEOUT_MS = 5000;
constexpr int VRPN_REPEAT_TIMEOUT_MS = 16;
#endif

const std::string HEAD_POSITION_PROP = "headPosition";
const std::string HEAD_ROTATION_PROP = "headRotation";

constexpr std::array<double, 3> to_array_3d(const vrpn_float64* pos)
{
    return {{pos[0], pos[1], pos[2]}};
}
constexpr std::array<double, 4> to_array_4d(const vrpn_float64* quat)
{
    return {{quat[0], quat[1], quat[2], quat[3]}};
}

void trackerCallback(void* userData, const vrpn_TRACKERCB tracker)
{
    auto camera = static_cast<Camera*>(userData);
    camera->updateProperty(HEAD_POSITION_PROP, to_array_3d(tracker.pos), false);
    camera->updateProperty(HEAD_ROTATION_PROP, to_array_4d(tracker.quat),
                           false);
}

void flyStickCallback(void* userData, const vrpn_TRACKERCB tracker)
{
    VrpnStates* states = static_cast<VrpnStates*>(userData);
    states->flyStickOrientation = glm::quat(tracker.quat[3], tracker.quat[0],
                                            tracker.quat[1], tracker.quat[2]);
}

void joystickCallback(void* userData, const vrpn_ANALOGCB joystick)
{
    VrpnStates* states = static_cast<VrpnStates*>(userData);
    states->axisX = joystick.channel[0];
    states->axisZ = joystick.channel[1];
}
}

VRPNPlugin::VRPNPlugin(const std::string& vrpnName)
    : _vrpnName(vrpnName)
{
}

VRPNPlugin::~VRPNPlugin()
{
    _vrpnTracker->unregister_change_handler(&(_api->getCamera()),
                                            trackerCallback, HEAD_SENSOR_ID);
}

void VRPNPlugin::init()
{
    _vrpnTracker = std::make_unique<vrpn_Tracker_Remote>(_vrpnName.c_str());
    if (!_vrpnTracker->connectionPtr()->doing_okay())
        return;

    _vrpnAnalog = std::make_unique<vrpn_Analog_Remote>(_vrpnName.c_str());
    if (!_vrpnAnalog->connectionPtr()->doing_okay())
        return;

    BRAYNS_INFO << "VRPN successfully connected to " << _vrpnName << std::endl;

#ifdef BRAYNSVRPN_USE_LIBUV
    _setupIdleTimer();
#endif

    _vrpnTracker->register_change_handler(&(_api->getCamera()), trackerCallback,
                                          HEAD_SENSOR_ID);
    _vrpnTracker->register_change_handler(&_states, flyStickCallback,
                                          FLYSTICK_SENSOR_ID);
    _vrpnAnalog->register_change_handler(&_states, joystickCallback);
}

void VRPNPlugin::preRender()
{
    if (!_vrpnTracker->connectionPtr()->doing_okay())
        return;

    _timer.stop();
    _vrpnTracker->mainloop();

    double frameTime = _timer.seconds();

    Camera& camera = _api->getCamera();
    Vector3d pos = camera.getPosition();
    pos += _states.axisX * MOVING_SPEED *
           glm::rotate(_states.flyStickOrientation, Vector3f(1.0, 0.0, 0.0)) *
           frameTime;
    pos += _states.axisZ * MOVING_SPEED *
           glm::rotate(_states.flyStickOrientation, Vector3f(0.0, 0.0, -1.0)) *
           frameTime;
    camera.setPosition(pos);

    _timer.start();
}

#ifdef BRAYNSVRPN_USE_LIBUV
void VRPNPlugin::resumeRenderingIfTrackerIsActive()
{
    _vrpnTracker->mainloop();
    if (_api->getCamera().isModified())
        _api->triggerRender();
}

void VRPNPlugin::_setupIdleTimer()
{
    if (auto uvLoop = uv_default_loop())
    {
        _idleTimer.reset(new uv_timer_t);
        uv_timer_init(uvLoop, _idleTimer.get());
        _idleTimer->data = this;

        uv_timer_start(_idleTimer.get(),
                       [](uv_timer_t* timer) {
                           auto plugin = static_cast<VRPNPlugin*>(timer->data);
                           plugin->resumeRenderingIfTrackerIsActive();
                       },
                       VRPN_IDLE_TIMEOUT_MS, VRPN_REPEAT_TIMEOUT_MS);
    }
}
#endif
}

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(const int argc,
                                                         const char** argv)
{
    if (argc > 2)
    {
        throw std::runtime_error(
            "VRPN plugin expects at most one argument, the name of the VRPN "
            "device to connect to (eg: Tracker0@localhost)");
    }

    const auto vrpnName = (argc >= 2) ? argv[1] : brayns::DEFAULT_VRPN_NAME;
    return new brayns::VRPNPlugin(vrpnName);
}
