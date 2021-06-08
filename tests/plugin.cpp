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

#include <jsonPropertyMap.h>

#include "ClientServer.h"

const brayns::Vector2i vecVal{1, 1};

TEST_CASE("plugin_actions")
{
    ClientServer clientServer({"--plugin", "myPlugin"});

    makeNotification("notify");
    brayns::PropertyMap input;
    input.add({"value", 42});
    makeNotification("notify-param", input);

    // wrong input, cannot test though
    makeNotification("notify-param", vecVal);

    brayns::PropertyMap output;
    output.add({"result", false});
    auto result = makeRequestUpdate("request", output);
    CHECK(result["result"].as<bool>());

    result = makeRequestUpdate("request-param", input, output);
    CHECK(result["result"].as<bool>());

    // wrong input
    result = makeRequestUpdate("request-param", vecVal, output);
    CHECK(result["error"].as<int>() == -1);

    makeNotification("hello");
    brayns::PropertyMap vecValInput;
    vecValInput.add({"param", vecVal, {""}});
    makeNotification("foo", vecValInput);

    brayns::PropertyMap whoOutput;
    whoOutput.add({"param", std::string(), {""}});
    makeRequestUpdate("who", whoOutput);
    CHECK(whoOutput["param"].as<std::string>() == "me");

    brayns::PropertyMap echoInput;
    echoInput.add({"param", vecVal, {""}});
    brayns::PropertyMap echoOutput;
    echoOutput.add({"param", brayns::Vector2i(), {""}});
    echoOutput = makeRequestUpdate("echo", echoInput, echoOutput);
    CHECK(vecVal == echoOutput["param"].as<brayns::Vector2i>());
    clientServer.getBrayns()
        .getParametersManager()
        .getRenderingParameters()
        .setCurrentRenderer("myrenderer");
    clientServer.getBrayns().commitAndRender();

    auto props =
        clientServer.getBrayns().getEngine().getRenderer().getPropertyMap();
    CHECK(props.find("awesome"));
    CHECK_EQ(props["awesome"].as<int>(), 42);

    props.update("awesome", 10);

    CHECK(
        (makeRequest<brayns::PropertyMap, bool>("set-renderer-params", props)));

    const auto& newProps =
        clientServer.getBrayns().getEngine().getRenderer().getPropertyMap();
    CHECK_EQ(newProps["awesome"].as<int>(), 10);
}
