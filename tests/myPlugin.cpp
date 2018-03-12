/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

#include "jsonSerialization.h"

#include <brayns/PluginAPI.h>
#include <brayns/common/ActionInterface.h>
#include <plugins/extensions/plugins/ExtensionPlugin.h>

#include <boost/test/unit_test.hpp>

using Vec2 = std::array<unsigned, 2>;
const Vec2 vecVal{{1, 1}};

class MyPlugin : public brayns::ExtensionPlugin
{
public:
    MyPlugin(brayns::PluginAPI* api)
    {
        auto actions = api->getActionInterface();
        BOOST_REQUIRE(actions);

        actions->registerNotification("hello", [&] { ++numCalls; });
        actions->registerNotification<Vec2>("foo", [&](const Vec2& vec) {
            ++numCalls;
            BOOST_CHECK(vec == vecVal);
        });

        actions->registerRequest<std::string>("who", [&] {
            ++numCalls;
            return "me";
        });
        actions->registerRequest<Vec2, Vec2>("echo", [&](const Vec2& vec) {
            ++numCalls;
            return vec;
        });
    }

    ~MyPlugin() { BOOST_REQUIRE_EQUAL(numCalls, 4); }
    size_t numCalls{0};
};

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(brayns::PluginAPI* api)
{
    return new MyPlugin(api);
}
