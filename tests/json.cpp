/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jonas Karlsson jonas.karlsson@epfl.ch
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

#include <plugins/Rockets/jsonSerialization.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

TEST_CASE("loaderProperties")
{
    brayns::ModelParams paramsOrig;

    {
        brayns::PropertyMap properties;
        properties.add({"string", std::string("string")});
        properties.add({"int", 42});
        properties.add(
            {"enum", {"b", {"a", "b", "c", "d"}}});
        properties.add({"array", brayns::Vector3i{1, 2, 3}});
        paramsOrig.setLoaderProperties(properties);
    }

    const auto jsonStr = to_json(paramsOrig);

    brayns::ModelParams paramsParse;
    from_json(paramsParse, jsonStr);
    CHECK_EQ(paramsOrig.getLoaderProperties()["string"].as<std::string>(),
             paramsParse.getLoaderProperties()["string"].as<std::string>());
    CHECK_EQ(paramsOrig.getLoaderProperties()["int"].as<int32_t>(),
             paramsParse.getLoaderProperties()["int"].as<int32_t>());
    CHECK_EQ(paramsOrig.getLoaderProperties()["enum"]
                 .as<brayns::EnumProperty>()
                 .toString(),
             paramsParse.getLoaderProperties()["enum"]
                 .as<std::string>());
    CHECK(paramsOrig.getLoaderProperties()["array"].as<brayns::Vector3i>() ==
          paramsParse.getLoaderProperties()["array"].as<brayns::Vector3i>());
}
