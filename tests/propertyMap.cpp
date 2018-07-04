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

#define BOOST_TEST_MODULE braynsPropertyMap

#include <brayns/common/PropertyMap.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(set_property)
{
    brayns::PropertyMap properties;
    properties.setProperty("foo", 1);
    BOOST_REQUIRE_EQUAL(properties.getProperties().size(), 1);
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->get<int32_t>(), 1);

    brayns::PropertyMap otherProperties;
    otherProperties.setProperty("bar", 42.f);
    properties.setProperty(*otherProperties.getProperties()[0]);
    BOOST_CHECK_EQUAL(properties.getProperties()[1]->get<float>(), 42.f);
}

BOOST_AUTO_TEST_CASE(get_property)
{
    brayns::PropertyMap properties;
    properties.setProperty("foo", 1);

    BOOST_CHECK_EQUAL(properties.getProperty<int32_t>("foo"), 1);
    BOOST_CHECK_EQUAL(properties.getProperty("bla", 5), 5);
}

BOOST_AUTO_TEST_CASE(set_and_get_all_supported_types)
{
    brayns::PropertyMap properties;
    properties.setProperty("int", 42);
    properties.setProperty("float", 1.2f);
    properties.setProperty("string", std::string("foo"));
    properties.setProperty("vec2i", std::array<int32_t, 2>{{1, 2}});
    properties.setProperty("vec2f", std::array<float, 2>{{1, 2}});
    properties.setProperty("vec3i", std::array<int32_t, 3>{{1, 2, 3}});
    properties.setProperty("vec3f", std::array<float, 3>{{1, 2, 3}});
    properties.setProperty("vec4f", std::array<float, 4>{{1, 2, 3, 4}});

    BOOST_CHECK_EQUAL(properties.getProperty<int32_t>("int"), 42);
    BOOST_CHECK_EQUAL(properties.getProperty<float>("float"), 1.2f);
    BOOST_CHECK_EQUAL(properties.getProperty<std::string>("string"), "foo");
    BOOST_CHECK((properties.getProperty<std::array<int32_t, 2>>("vec2i") ==
                 std::array<int32_t, 2>{{1, 2}}));
    BOOST_CHECK((properties.getProperty<std::array<float, 2>>("vec2f") ==
                 std::array<float, 2>{{1, 2}}));
    BOOST_CHECK((properties.getProperty<std::array<int32_t, 3>>("vec3i") ==
                 std::array<int32_t, 3>{{1, 2, 3}}));
    BOOST_CHECK((properties.getProperty<std::array<float, 3>>("vec3f") ==
                 std::array<float, 3>{{1, 2, 3}}));
    BOOST_CHECK((properties.getProperty<std::array<float, 4>>("vec4f") ==
                 std::array<float, 4>{{1, 2, 3, 4}}));
}
