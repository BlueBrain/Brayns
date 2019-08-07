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

using Vec2 = std::array<unsigned, 2>;
const Vec2 vecVal{{1, 1}};

TEST_CASE("plugin_actions")
{
    ClientServer clientServer({"--plugin", "myPlugin"});

    makeNotification("notify");
    brayns::PropertyMap input;
    input.setProperty({"value", 42});
    makeNotification("notify-param", input);

    // wrong input, cannot test though
    makeNotification("notify-param", vecVal);

    brayns::PropertyMap output;
    output.setProperty({"result", false});
    auto result = makeRequestUpdate("request", output);
    CHECK(result.getProperty<bool>("result"));

    result = makeRequestUpdate("request-param", input, output);
    CHECK(result.getProperty<bool>("result"));

    // wrong input
    CHECK_THROWS_AS(makeRequestUpdate("request-param", vecVal, output),
                    std::runtime_error);

    makeNotification("hello");
    makeNotification("foo", vecVal);
    CHECK_EQ(makeRequest<std::string>("who"), "me");
    CHECK((makeRequest<Vec2, Vec2>("echo", vecVal) == vecVal));

    clientServer.getBrayns()
        .getParametersManager()
        .getRenderingParameters()
        .setCurrentRenderer("myrenderer");
    clientServer.getBrayns().commitAndRender();

    auto props =
        clientServer.getBrayns().getEngine().getRenderer().getPropertyMap();
    CHECK(props.hasProperty("awesome"));
    CHECK_EQ(props.getProperty<int>("awesome"), 42);

    props.updateProperty("awesome", 10);

    CHECK(
        (makeRequest<brayns::PropertyMap, bool>("set-renderer-params", props)));

    const auto& newProps =
        clientServer.getBrayns().getEngine().getRenderer().getPropertyMap();
    CHECK_EQ(newProps.getProperty<int>("awesome"), 10);
}
