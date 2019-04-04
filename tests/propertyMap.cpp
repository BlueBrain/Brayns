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
    properties.setProperty({"foo", 1});
    BOOST_REQUIRE_EQUAL(properties.getProperties().size(), 1);
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->get<int32_t>(), 1);
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->metaData.label, "");
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->metaData.description,
                      "no-description");
}

BOOST_AUTO_TEST_CASE(set_property_with_metadata)
{
    brayns::PropertyMap properties;

    properties.setProperty({"foo", 1, {"Foo", "Foo description"}});
    BOOST_REQUIRE_EQUAL(properties.getProperties().size(), 1);
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->get<int32_t>(), 1);
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->metaData.label, "Foo");
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->metaData.description,
                      "Foo description");
}

BOOST_AUTO_TEST_CASE(set_property_with_limits)
{
    brayns::PropertyMap properties;
    properties.setProperty({"limit", 0.5, 0., 1.});
    BOOST_REQUIRE_EQUAL(properties.getProperties().size(), 1);
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->get<double>(), 0.5);
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->min<double>(), 0.);
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->max<double>(), 1.);
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->metaData.label, "");
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->metaData.description,
                      "no-description");
}

BOOST_AUTO_TEST_CASE(set_property_with_limits_and_metadata)
{
    brayns::PropertyMap properties;
    properties.setProperty(
        {"limit", 0.5, 0., 1., {"With limits", "Limits description"}});
    BOOST_REQUIRE_EQUAL(properties.getProperties().size(), 1);
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->get<double>(), 0.5);
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->min<double>(), 0.);
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->max<double>(), 1.);
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->metaData.label,
                      "With limits");
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->metaData.description,
                      "Limits description");
}

BOOST_AUTO_TEST_CASE(set_property_from_other_property)
{
    brayns::PropertyMap properties;

    brayns::PropertyMap otherProperties;
    otherProperties.setProperty({"bar", 42.});
    properties.setProperty(*otherProperties.getProperties()[0]);
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->get<double>(), 42.);

    BOOST_CHECK_THROW(otherProperties.setProperty(
                          {"bar", std::string("no double")}),
                      std::runtime_error);
}

BOOST_AUTO_TEST_CASE(update_property)
{
    brayns::PropertyMap properties;
    properties.setProperty({"foo", 1});
    properties.updateProperty("foo", 42);
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->get<int32_t>(), 42);

    BOOST_CHECK_THROW(properties.updateProperty("foo", false),
                      std::runtime_error);

    BOOST_CHECK(properties.hasProperty("foo"));
    properties.setProperty({"foo", 0});
    BOOST_CHECK_EQUAL(properties.getProperties()[0]->get<int32_t>(), 0);

    BOOST_CHECK(!properties.hasProperty("bar"));
    BOOST_CHECK_NO_THROW(properties.updateProperty("bar", 42));
}

BOOST_AUTO_TEST_CASE(get_property)
{
    brayns::PropertyMap properties;
    properties.setProperty({"foo", 1});

    BOOST_CHECK_EQUAL(properties.getProperty("foo", 5), 1);
    BOOST_CHECK_EQUAL(properties.getProperty<int32_t>("foo"), 1);
    BOOST_CHECK_EQUAL(properties.getProperty("bla", 5), 5);
    BOOST_CHECK_THROW(properties.getProperty<bool>("bar"), std::runtime_error);
    BOOST_CHECK_THROW(properties.getPropertyType("bar"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(set_and_get_all_supported_types)
{
    brayns::PropertyMap properties;
    properties.setProperty({"int", 42});
    properties.setProperty(
        {"enum", std::string("Zero"), {"Zero", "One", "Two"}, {}});
    properties.setProperty({"double", 1.2});
    properties.setProperty({"string", std::string("foo")});
    properties.setProperty({"bool", true});
    properties.setProperty({"vec2i", std::array<int32_t, 2>{{1, 2}}});
    properties.setProperty({"vec2d", std::array<double, 2>{{1, 2}}});
    properties.setProperty({"vec3i", std::array<int32_t, 3>{{1, 2, 3}}});
    properties.setProperty({"vec3d", std::array<double, 3>{{1, 2, 3}}});
    properties.setProperty({"vec4d", std::array<double, 4>{{1, 2, 3, 4}}});

    BOOST_CHECK_EQUAL(properties.getProperty<int32_t>("int"), 42);
    BOOST_CHECK_EQUAL(properties.getProperty<std::string>("enum"), "Zero");
    BOOST_CHECK_EQUAL(properties.getEnums("enum").size(), 3);
    BOOST_CHECK_EQUAL(properties.getProperty<double>("double"), 1.2);
    BOOST_CHECK_EQUAL(properties.getProperty<std::string>("string"), "foo");
    BOOST_CHECK(properties.getProperty<bool>("bool"));
    BOOST_CHECK((properties.getProperty<std::array<int32_t, 2>>("vec2i") ==
                 std::array<int32_t, 2>{{1, 2}}));
    BOOST_CHECK((properties.getProperty<std::array<double, 2>>("vec2d") ==
                 std::array<double, 2>{{1, 2}}));
    BOOST_CHECK((properties.getProperty<std::array<int32_t, 3>>("vec3i") ==
                 std::array<int32_t, 3>{{1, 2, 3}}));
    BOOST_CHECK((properties.getProperty<std::array<double, 3>>("vec3d") ==
                 std::array<double, 3>{{1, 2, 3}}));
    BOOST_CHECK((properties.getProperty<std::array<double, 4>>("vec4d") ==
                 std::array<double, 4>{{1, 2, 3, 4}}));

    using Type = brayns::Property::Type;
    BOOST_CHECK(properties.getPropertyType("int") == Type::Int);
    BOOST_CHECK(properties.getPropertyType("double") == Type::Double);
    BOOST_CHECK(properties.getPropertyType("string") == Type::String);
    BOOST_CHECK(properties.getPropertyType("bool") == Type::Bool);
    BOOST_CHECK(properties.getPropertyType("vec2i") == Type::Vec2i);
    BOOST_CHECK(properties.getPropertyType("vec2d") == Type::Vec2d);
    BOOST_CHECK(properties.getPropertyType("vec3i") == Type::Vec3i);
    BOOST_CHECK(properties.getPropertyType("vec3d") == Type::Vec3d);
    BOOST_CHECK(properties.getPropertyType("vec4d") == Type::Vec4d);
}

BOOST_AUTO_TEST_CASE(fill_property_map)
{
    using Type = brayns::Property::Type;

    brayns::PropertyMap propInts;
    propInts.setProperty({"number", 42});
    propInts.setProperty({"vec2", std::array<int32_t, 2>{{1, 2}}});
    propInts.setProperty({"vec3", std::array<int32_t, 3>{{1, 2, 3}}});

    brayns::PropertyMap propDoubles;
    propDoubles.setProperty({"number", -42.0});
    propDoubles.setProperty({"vec2", std::array<double, 2>{{-1, -2}}});
    propDoubles.setProperty({"vec3", std::array<double, 3>{{-1, -2, -3}}});

    propInts.merge(propDoubles);
    propDoubles.merge(propInts);

    BOOST_CHECK(propInts.getPropertyType("number") == Type::Int);
    BOOST_CHECK(propInts.getPropertyType("vec2") == Type::Vec2i);
    BOOST_CHECK(propInts.getPropertyType("vec3") == Type::Vec3i);

    BOOST_CHECK(propDoubles.getPropertyType("number") == Type::Double);
    BOOST_CHECK(propDoubles.getPropertyType("vec2") == Type::Vec2d);
    BOOST_CHECK(propDoubles.getPropertyType("vec3") == Type::Vec3d);

    BOOST_CHECK_EQUAL(propInts.getProperty<int32_t>("number"),
                      propDoubles.getProperty<double>("number"));
    {
        const auto aInts = propInts.getProperty<std::array<int, 2>>("vec2");
        const auto aDoubles =
            propDoubles.getProperty<std::array<double, 2>>("vec2");
        BOOST_CHECK_EQUAL(aInts[0], aDoubles[0]);
        BOOST_CHECK_EQUAL(aInts[1], aDoubles[1]);
    }
    {
        const auto aInts = propInts.getProperty<std::array<int, 3>>("vec3");
        const auto aDoubles =
            propDoubles.getProperty<std::array<double, 3>>("vec3");
        BOOST_CHECK_EQUAL(aInts[0], aDoubles[0]);
        BOOST_CHECK_EQUAL(aInts[1], aDoubles[1]);
        BOOST_CHECK_EQUAL(aInts[2], aDoubles[2]);
    }

    propInts.setProperty({"foo", std::string("string")});
    propDoubles.setProperty({"foo", 42});
    BOOST_CHECK_THROW(propInts.merge(propDoubles), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(update_properties)
{
    brayns::PropertyMap source;
    source.setProperty({"number", 42});
    source.setProperty({"vec2", std::array<int32_t, 2>{{1, 2}}});
    source.setProperty({"vec3", std::array<int32_t, 3>{{1, 2, 3}}});

    brayns::PropertyMap dest;
    dest.setProperty({"number", 27});
    dest.setProperty({"vec2", std::array<int32_t, 2>{{0, 1}}});

    dest.update(source);
    BOOST_CHECK_EQUAL(dest.getProperty<int32_t>("number"), 42);
    const auto array = dest.getProperty<std::array<int32_t, 2>>("vec2");
    BOOST_CHECK_EQUAL(1, array[0]);
    BOOST_CHECK_EQUAL(2, array[1]);

    dest.setProperty({"vec3", 10});
    BOOST_CHECK_THROW(dest.update(source), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(merge_enums)
{
    using Type = brayns::Property::Type;

    const std::vector<std::string> enums = {"a", "b", "c"};
    brayns::PropertyMap propInts;
    propInts.setProperty({"abc", 1, enums, {}});
    brayns::PropertyMap propStrings;
    propStrings.setProperty({"abc", std::string("c"), enums, {}});

    {
        brayns::PropertyMap propIntsTmp;

        propIntsTmp.update(propInts);
        BOOST_CHECK(propIntsTmp.empty());
        propIntsTmp.update(propStrings);
        BOOST_CHECK(propIntsTmp.empty());

        propIntsTmp.merge(propInts);
        propIntsTmp.merge(propStrings);

        BOOST_CHECK(propIntsTmp.getPropertyType("abc") == Type::Int);
        BOOST_CHECK_EQUAL(propIntsTmp.getProperty<int32_t>("abc"), 2);

        propIntsTmp.update(propInts);
        BOOST_CHECK(propIntsTmp.getPropertyType("abc") == Type::Int);
        BOOST_CHECK_EQUAL(propIntsTmp.getProperty<int32_t>("abc"), 1);

        propIntsTmp.update(propStrings);
        BOOST_CHECK(propIntsTmp.getPropertyType("abc") == Type::Int);
        BOOST_CHECK_EQUAL(propIntsTmp.getProperty<int32_t>("abc"), 2);
    }

    {
        brayns::PropertyMap propStringsTmp;
        propStringsTmp.merge(propStrings);
        propStringsTmp.merge(propInts);

        BOOST_CHECK(propStringsTmp.getPropertyType("abc") == Type::String);
        BOOST_CHECK_EQUAL(propStringsTmp.getProperty<std::string>("abc"), "b");

        propStringsTmp.update(propStrings);
        BOOST_CHECK(propStringsTmp.getPropertyType("abc") == Type::String);
        BOOST_CHECK_EQUAL(propStringsTmp.getProperty<std::string>("abc"), "c");

        propStringsTmp.update(propInts);
        BOOST_CHECK(propStringsTmp.getPropertyType("abc") == Type::String);
        BOOST_CHECK_EQUAL(propStringsTmp.getProperty<std::string>("abc"), "b");
    }
}

BOOST_AUTO_TEST_CASE(commandline)
{
    brayns::PropertyMap properties;
    properties.setProperty({"int", 42});
    properties.setProperty(
        {"enum", std::string("Zero"), {"Zero", "One", "Two"}, {}});
    properties.setProperty({"intEnum", 1, {"One", "Two", "Three"}, {}});
    properties.setProperty({"double", 1.2});
    properties.setProperty({"string", std::string("foo")});
    properties.setProperty({"bool", true});
    properties.setProperty({"boolSwitch", false});
    properties.setProperty({"vec2i", std::array<int32_t, 2>{{1, 2}}});
    properties.setProperty({"vec2d", std::array<double, 2>{{1, 2}}});
    properties.setProperty({"vec3i", std::array<int32_t, 3>{{1, 2, 3}}});
    properties.setProperty({"vec3d", std::array<double, 3>{{1, 2, 3}}});
    properties.setProperty({"vec4d", std::array<double, 4>{{1, 2, 3, 4}}});

    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {
        app,       "--int",         "5",       "--double", "0.4",     "--enum",
        "One",     "--int-enum",    "Three",   "--string", "bar",     "--bool",
        "off",     "--bool-switch", "--vec2i", "3",        "4",       "--vec2d",
        "1.2",     "2.3",           "--vec3i", "3",        "4",       "5",
        "--vec3d", "1.2",           "2.3",     "3.4",      "--vec4d", "1.2",
        "2.3",     "3.4",           "4.5"};
    const int argc = sizeof(argv) / sizeof(char*);

    BOOST_REQUIRE(properties.parse(argc, argv));

    BOOST_CHECK_EQUAL(properties.getProperty<int32_t>("int"), 5);
    BOOST_CHECK_EQUAL(properties.getProperty<double>("double"), 0.4);
    BOOST_CHECK_EQUAL(properties.getProperty<std::string>("enum"), "One");
    BOOST_CHECK_EQUAL(properties.getProperty<int32_t>("intEnum"), 2);
    BOOST_CHECK_EQUAL(properties.getProperty<std::string>("string"), "bar");
    BOOST_CHECK(!properties.getProperty<bool>("bool"));
    BOOST_CHECK(properties.getProperty<bool>("boolSwitch"));
    BOOST_CHECK((properties.getProperty<std::array<int32_t, 2>>("vec2i") ==
                 std::array<int32_t, 2>{{3, 4}}));
    BOOST_CHECK((properties.getProperty<std::array<double, 2>>("vec2d") ==
                 std::array<double, 2>{{1.2, 2.3}}));
    BOOST_CHECK((properties.getProperty<std::array<int32_t, 3>>("vec3i") ==
                 std::array<int32_t, 3>{{3, 4, 5}}));
    BOOST_CHECK((properties.getProperty<std::array<double, 3>>("vec3d") ==
                 std::array<double, 3>{{1.2, 2.3, 3.4}}));
    BOOST_CHECK((properties.getProperty<std::array<double, 4>>("vec4d") ==
                 std::array<double, 4>{{1.2, 2.3, 3.4, 4.5}}));
}

BOOST_AUTO_TEST_CASE(commandline_no_option)
{
    brayns::PropertyMap properties;
    properties.setProperty({"int", 42});

    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app};
    const int argc = sizeof(argv) / sizeof(char*);

    BOOST_REQUIRE(properties.parse(argc, argv));

    BOOST_CHECK_EQUAL(properties.getProperty<int32_t>("int"), 42);
}

BOOST_AUTO_TEST_CASE(commandline_unknown_option)
{
    brayns::PropertyMap properties;
    properties.setProperty({"vec2i", std::array<int32_t, 2>{{1, 2}}});

    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app, "--vec3i", "3", "4", "5"};
    const int argc = sizeof(argv) / sizeof(char*);

    BOOST_CHECK(!properties.parse(argc, argv));
}

BOOST_AUTO_TEST_CASE(commandline_too_many_vector_values)
{
    brayns::PropertyMap properties;
    properties.setProperty({"vec2i", std::array<int32_t, 2>{{1, 2}}});

    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app, "--vec2i", "3", "4", "5"};
    const int argc = sizeof(argv) / sizeof(char*);

    BOOST_CHECK(properties.parse(argc, argv));
}

BOOST_AUTO_TEST_CASE(commandline_wrong_enum_value)
{
    brayns::PropertyMap properties;
    properties.setProperty(
        {"enum", std::string("Zero"), {"Zero", "One", "Two"}, {}});

    auto& testSuite = boost::unit_test::framework::master_test_suite();

    const char* app = testSuite.argv[0];
    const char* argv[] = {app, "--enum", "Four"};
    const int argc = sizeof(argv) / sizeof(char*);

    BOOST_CHECK(!properties.parse(argc, argv));
}
