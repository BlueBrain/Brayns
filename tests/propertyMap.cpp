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
    properties.setProperty({"foo", "Foo", 1});
    BOOST_REQUIRE_EQUAL(properties.getProperties().size(), 1);
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->get<int32_t>(), 1);
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->label, "Foo");

    properties.setProperty({"limit", "With limits", 0.5f, {0.f, 1.f}});
    BOOST_CHECK_EQUAL(properties.getProperties()[1]->get<float>(), 0.5f);
    BOOST_CHECK_EQUAL(properties.getProperties()[1]->label, "With limits");
    BOOST_CHECK_EQUAL(properties.getProperties()[1]->min<float>(), 0.f);
    BOOST_CHECK_EQUAL(properties.getProperties()[1]->max<float>(), 1.f);

    brayns::PropertyMap otherProperties;
    otherProperties.setProperty({"bar", "Bar", 42.f});
    properties.setProperty(*otherProperties.getProperties()[0]);
    BOOST_CHECK_EQUAL(properties.getProperties()[2]->get<float>(), 42.f);

    BOOST_CHECK_THROW(otherProperties.setProperty(
                          {"bar", "Bar", std::string("no float")}),
                      std::runtime_error);
}

BOOST_AUTO_TEST_CASE(update_property)
{
    brayns::PropertyMap properties;
    properties.setProperty({"foo", "Foo", 1});
    properties.updateProperty("foo", 42);
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->get<int32_t>(), 42);

    BOOST_CHECK_THROW(properties.updateProperty("foo", false),
                      std::runtime_error);

    BOOST_CHECK(properties.hasProperty("foo"));
    properties.setProperty({"foo", "Foo", 0});
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->get<int32_t>(), 0);

    BOOST_CHECK(!properties.hasProperty("bar"));
    BOOST_CHECK_NO_THROW(properties.updateProperty("bar", 42));
}

BOOST_AUTO_TEST_CASE(get_property)
{
    brayns::PropertyMap properties;
    properties.setProperty({"foo", "Foo", 1});

    BOOST_CHECK_EQUAL(properties.getProperty("foo", 5), 1);
    BOOST_CHECK_EQUAL(properties.getProperty<int32_t>("foo"), 1);
    BOOST_CHECK_EQUAL(properties.getProperty("bla", 5), 5);
    BOOST_CHECK_THROW(properties.getProperty<bool>("bar"), std::runtime_error);
    BOOST_CHECK_THROW(properties.getPropertyType("bar"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(set_and_get_all_supported_types)
{
    brayns::PropertyMap properties;
    properties.setProperty({"int", "Int", 42});
    properties.setProperty({"enum", "Enum", 0, {"Zero", "One", "Two"}});
    properties.setProperty({"float", "Float", 1.2f});
    properties.setProperty({"string", "String", std::string("foo")});
    properties.setProperty({"const char", "ConstChar", (const char*)"bar"});
    properties.setProperty({"bool", "bool", true});
    properties.setProperty({"vec2i", "Vec2i", std::array<int32_t, 2>{{1, 2}}});
    properties.setProperty({"vec2f", "Vec2f", std::array<float, 2>{{1, 2}}});
    properties.setProperty(
        {"vec3i", "Vec3i", std::array<int32_t, 3>{{1, 2, 3}}});
    properties.setProperty({"vec3f", "Vec3f", std::array<float, 3>{{1, 2, 3}}});
    properties.setProperty(
        {"vec4f", "Vec4f", std::array<float, 4>{{1, 2, 3, 4}}});

    BOOST_CHECK_EQUAL(properties.getProperty<int32_t>("int"), 42);
    BOOST_CHECK_EQUAL(properties.getProperty<int32_t>("enum"), 0);
    BOOST_CHECK_EQUAL(properties.getEnums("enum").size(), 3);
    BOOST_CHECK_EQUAL(properties.getProperty<float>("float"), 1.2f);
    BOOST_CHECK_EQUAL(properties.getProperty<std::string>("string"), "foo");
    BOOST_CHECK_EQUAL(properties.getProperty<const char*>("const char"), "bar");
    BOOST_CHECK(properties.getProperty<bool>("bool"));
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

    using Type = brayns::PropertyMap::Property::Type;
    BOOST_CHECK(properties.getPropertyType("int") == Type::Int);
    BOOST_CHECK(properties.getPropertyType("float") == Type::Float);
    BOOST_CHECK(properties.getPropertyType("string") == Type::String);
    BOOST_CHECK(properties.getPropertyType("const char") == Type::String);
    BOOST_CHECK(properties.getPropertyType("bool") == Type::Bool);
    BOOST_CHECK(properties.getPropertyType("vec2i") == Type::Vec2i);
    BOOST_CHECK(properties.getPropertyType("vec2f") == Type::Vec2f);
    BOOST_CHECK(properties.getPropertyType("vec3i") == Type::Vec3i);
    BOOST_CHECK(properties.getPropertyType("vec3f") == Type::Vec3f);
    BOOST_CHECK(properties.getPropertyType("vec4f") == Type::Vec4f);
}
