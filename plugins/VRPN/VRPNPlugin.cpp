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

#include <brayns/common/light/Light.h>
#include <brayns/common/log.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>
#include <brayns/pluginapi/PluginAPI.h>

namespace brayns
{
namespace
{
constexpr vrpn_int32 HEAD_SENSOR_ID = 0;
constexpr vrpn_int32 FLYSTICK_SENSOR_ID = 1;
constexpr double MOVING_SPEED = 8.0f;
const std::string DEFAULT_VRPN_NAME = "DTrack@cave1";
#ifdef BRAYNSVRPN_USE_LIBUV
constexpr int VRPN_IDLE_TIMEOUT_MS = 5000;
constexpr int VRPN_REPEAT_TIMEOUT_MS = 16;
#endif

const std::string HEAD_POSITION_PROP = "headPosition";
const std::string HEAD_ROTATION_PROP = "headRotation";
const std::string FLYSTICK_ROTATION_PROP = "flystickRotation";

constexpr vrpn_int32 BUTTON_TRIGGER = 0;
constexpr vrpn_int32 BUTTON_JOYSTICK = 5;
constexpr vrpn_int32 BUTTON_1 = 4;
constexpr vrpn_int32 BUTTON_2 = 3;
constexpr vrpn_int32 BUTTON_3 = 2;
constexpr vrpn_int32 BUTTON_4 = 1;

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
    states->api->getCamera().updateProperty(FLYSTICK_ROTATION_PROP,
                                            to_array_4d(tracker.quat), false);

    auto newPos = Vector3f(tracker.pos[0], tracker.pos[1], tracker.pos[2]);

    auto model = states->api->getScene().getModel(0);
    model->setVisible(states->triggerPressed);
    if (states->triggerPressed)
    {
        auto delta = newPos - states->prevPos;
        delta *= 2;
        auto sunLight = states->api->getScene().getLightManager().getLight(0);
        auto sun =
            std::dynamic_pointer_cast<brayns::DirectionalLight>(sunLight);
        if (sun)
        {
            sun->_direction += delta;
            states->api->getScene().getLightManager().addLight(sun);
        }
    }
    states->prevPos = newPos;
}

void joystickCallback(void* userData, const vrpn_ANALOGCB joystick)
{
    VrpnStates* states = static_cast<VrpnStates*>(userData);
    states->axisX = joystick.channel[0];
    states->axisZ = joystick.channel[1];
}

void buttonCallback(void* userData, const vrpn_BUTTONCB button)
{
    auto& functable = *static_cast<FuncTable*>(userData);
    functable[button.button](button.state == 1);
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
    _vrpnButton->unregister_change_handler(&_buttonFuncs, buttonCallback);
}

void VRPNPlugin::init()
{
    _vrpnTracker = std::make_unique<vrpn_Tracker_Remote>(_vrpnName.c_str());
    if (!_vrpnTracker->connectionPtr()->doing_okay())
        return;

    _vrpnAnalog = std::make_unique<vrpn_Analog_Remote>(_vrpnName.c_str());
    if (!_vrpnAnalog->connectionPtr()->doing_okay())
        return;

    _vrpnButton = std::make_unique<vrpn_Button_Remote>(_vrpnName.c_str());
    if (!_vrpnButton->connectionPtr()->doing_okay())
        return;

    BRAYNS_INFO << "VRPN successfully connected to " << _vrpnName << std::endl;

#ifdef BRAYNSVRPN_USE_LIBUV
    _setupIdleTimer();
#endif

    _vrpnTracker->register_change_handler(&(_api->getCamera()), trackerCallback,
                                          HEAD_SENSOR_ID);
    _states.api = _api;
    _vrpnTracker->register_change_handler(&_states, flyStickCallback,
                                          FLYSTICK_SENSOR_ID);
    _vrpnAnalog->register_change_handler(&_states, joystickCallback);
    _vrpnButton->register_change_handler(&_buttonFuncs, buttonCallback);

    _buttonFuncs[BUTTON_JOYSTICK] =
        [& camera = _api->getCamera(), this ](bool pressed)
    {
        if (pressed)
        {
            camera.reset();
            camera.updateProperty("interpupillaryDistance", 0.065);
            camera.updateProperty("nearClip", 0.5);
            auto sunLight = _api->getScene().getLightManager().getLight(0);
            auto sun =
                std::dynamic_pointer_cast<brayns::DirectionalLight>(sunLight);
            if (sun)
            {
                sun->_direction = {0., 0., 0.};
                _api->getScene().getLightManager().addLight(sun);
            }
        }
    };
    _buttonFuncs[BUTTON_TRIGGER] = [& triggerPressed =
                                        _states.triggerPressed](bool pressed)
    {
        triggerPressed = pressed;
    };

    auto idpChange = [& camera = _api->getCamera()](const double delta)
    {
        camera.updateProperty("interpupillaryDistance",
                              std::max(0.0, camera.getProperty<double>(
                                                "interpupillaryDistance") +
                                                delta));
    };
    _buttonFuncs[BUTTON_1] = [idpChange](bool pressed) {
        if (pressed)
            idpChange(-0.002);
    };

    _buttonFuncs[BUTTON_2] = [idpChange](bool pressed) {
        if (pressed)
            idpChange(0.002);
    };

    auto nearChange = [& camera = _api->getCamera()](const double delta)
    {
        camera.updateProperty(
            "nearClip",
            std::max(0.0, camera.getProperty<double>("nearClip") + delta));
    };
    _buttonFuncs[BUTTON_3] = [nearChange](bool pressed) {
        if (pressed)
            nearChange(-0.1);
    };
    _buttonFuncs[BUTTON_4] = [nearChange](bool pressed) {
        if (pressed)
            nearChange(0.1);
    };
}

void VRPNPlugin::preRender()
{
    if (!_vrpnTracker->connectionPtr()->doing_okay())
        return;

    _timer.stop();
    _vrpnTracker->mainloop();
    _vrpnAnalog->mainloop();
    _vrpnButton->mainloop();

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
