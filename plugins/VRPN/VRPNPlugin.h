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

#pragma once

#include <braynsvrpn/defines.h>

#include <brayns/common/Timer.h>
#include <brayns/common/types.h>
#include <brayns/pluginapi/ExtensionPlugin.h>

#include <vrpn_Analog.h>
#include <vrpn_Button.h>
#include <vrpn_Tracker.h>

#ifdef BRAYNSVRPN_USE_LIBUV
#include <uv.h>
#endif

#include <functional>
#include <unordered_map>

namespace brayns
{
struct VrpnStates
{
    float axisX = 0.0f;
    float axisZ = 0.0f;
    glm::quat flyStickOrientation;
    PluginAPI* api = nullptr;
    bool triggerPressed = false;
    Vector3f prevPos;
};

using FuncTable = std::unordered_map<vrpn_int32, std::function<void(bool)>>;

class VRPNPlugin : public ExtensionPlugin
{
public:
    VRPNPlugin(const std::string& vrpnName);
    ~VRPNPlugin();

    void init() final;

    void preRender() final;

#ifdef BRAYNSVRPN_USE_LIBUV
    void resumeRenderingIfTrackerIsActive();
#endif

private:
    std::unique_ptr<vrpn_Tracker_Remote> _vrpnTracker;
    std::unique_ptr<vrpn_Analog_Remote> _vrpnAnalog;
    std::unique_ptr<vrpn_Button_Remote> _vrpnButton;
    const std::string _vrpnName;
    Timer _timer;
    VrpnStates _states;

    FuncTable _buttonFuncs;

#ifdef BRAYNSVRPN_USE_LIBUV
    struct LibuvDeleter
    {
        void operator()(uv_timer_t* timer)
        {
            uv_timer_stop(timer);
            uv_close(reinterpret_cast<uv_handle_t*>(timer),
                     [](uv_handle_t* handle) { delete handle; });
        }
    };
    std::unique_ptr<uv_timer_t, LibuvDeleter> _idleTimer;

    void _setupIdleTimer();
#endif
};
}
