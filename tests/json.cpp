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
        properties.setProperty({"string", std::string("string")});
        properties.setProperty({"int", 42});
        properties.setProperty(
            {"enum", std::string("b"), {"a", "b", "c", "d"}, {}});
        properties.setProperty({"array", std::array<int, 3>{{1, 2, 3}}});
        paramsOrig.setLoaderProperties(properties);
    }

    const auto jsonStr = to_json(paramsOrig);

    brayns::ModelParams paramsParse;
    from_json(paramsParse, jsonStr);
    CHECK_EQ(
        paramsOrig.getLoaderProperties().getProperty<std::string>("string"),
        paramsParse.getLoaderProperties().getProperty<std::string>("string"));
    CHECK_EQ(paramsOrig.getLoaderProperties().getProperty<int32_t>("int"),
             paramsParse.getLoaderProperties().getProperty<int32_t>("int"));
    CHECK_EQ(paramsOrig.getLoaderProperties().getProperty<std::string>("enum"),
             paramsParse.getLoaderProperties().getProperty<std::string>(
                 "enum"));

    const auto& origArray =
        paramsOrig.getLoaderProperties().getProperty<std::array<int, 3>>(
            "array");
    const auto& parseArray =
        paramsParse.getLoaderProperties().getProperty<std::array<int, 3>>(
            "array");
    CHECK(origArray == parseArray);
}
