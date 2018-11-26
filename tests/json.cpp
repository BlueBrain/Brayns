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

#define BOOST_TEST_MODULE braynsJson

#include <plugins/Rockets/jsonSerialization.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_ModelParams)
{
    brayns::ModelParams paramsOrig;

    {
        brayns::PropertyMap properties;
        properties.setProperty({"string", "string", std::string("string")});
        properties.setProperty({"int", "int", 42});
        properties.setProperty(
            {"enum", "enum", std::string("b"), {"a", "b", "c", "d"}});
        paramsOrig.setLoaderProperties(properties);
    }

    const auto jsonStr = to_json(paramsOrig);

    brayns::ModelParams paramsParse;
    from_json(paramsParse, jsonStr);
    BOOST_CHECK_EQUAL(
        paramsOrig.getLoaderProperties().getProperty<std::string>("string"),
        paramsParse.getLoaderProperties().getProperty<std::string>("string"));
    BOOST_CHECK_EQUAL(
        paramsOrig.getLoaderProperties().getProperty<int32_t>("int"),
        paramsParse.getLoaderProperties().getProperty<int32_t>("int"));
    BOOST_CHECK_EQUAL(
        paramsOrig.getLoaderProperties().getProperty<std::string>("enum"),
        paramsParse.getLoaderProperties().getProperty<std::string>("enum"));
}
