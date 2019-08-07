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

#include <brayns/common/PropertyMap.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

TEST_CASE("set_property")
{
    brayns::PropertyMap properties;
    properties.setProperty({"foo", 1});
    REQUIRE_EQ(properties.getProperties().size(), 1);
    CHECK_EQ(properties.getProperties()[0]->get<int32_t>(), 1);
    CHECK_EQ(properties.getProperties()[0]->metaData.label, "");
    CHECK_EQ(properties.getProperties()[0]->metaData.description,
             "no-description");
}

TEST_CASE("set_property_with_metadata")
{
    brayns::PropertyMap properties;

    properties.setProperty({"foo", 1, {"Foo", "Foo description"}});
    REQUIRE_EQ(properties.getProperties().size(), 1);
    CHECK_EQ(properties.getProperties()[0]->get<int32_t>(), 1);
    CHECK_EQ(properties.getProperties()[0]->metaData.label, "Foo");
    CHECK_EQ(properties.getProperties()[0]->metaData.description,
             "Foo description");
}

TEST_CASE("set_property_with_limits")
{
    brayns::PropertyMap properties;
    properties.setProperty({"limit", 0.5, 0., 1.});
    REQUIRE_EQ(properties.getProperties().size(), 1);
    CHECK_EQ(properties.getProperties()[0]->get<double>(), 0.5);
    CHECK_EQ(properties.getProperties()[0]->min<double>(), 0.);
    CHECK_EQ(properties.getProperties()[0]->max<double>(), 1.);
    CHECK_EQ(properties.getProperties()[0]->metaData.label, "");
    CHECK_EQ(properties.getProperties()[0]->metaData.description,
             "no-description");
}

TEST_CASE("set_property_with_limits_and_metadata")
{
    brayns::PropertyMap properties;
    properties.setProperty(
        {"limit", 0.5, 0., 1., {"With limits", "Limits description"}});
    REQUIRE_EQ(properties.getProperties().size(), 1);
    CHECK_EQ(properties.getProperties()[0]->get<double>(), 0.5);
    CHECK_EQ(properties.getProperties()[0]->min<double>(), 0.);
    CHECK_EQ(properties.getProperties()[0]->max<double>(), 1.);
    CHECK_EQ(properties.getProperties()[0]->metaData.label, "With limits");
    CHECK_EQ(properties.getProperties()[0]->metaData.description,
             "Limits description");
}

TEST_CASE("set_property_from_other_property")
{
    brayns::PropertyMap properties;

    brayns::PropertyMap otherProperties;
    otherProperties.setProperty({"bar", 42.});
    properties.setProperty(*otherProperties.getProperties()[0]);
    CHECK_EQ(properties.getProperties()[0]->get<double>(), 42.);

    CHECK_THROWS_AS(otherProperties.setProperty(
                        {"bar", std::string("no double")}),
                    std::runtime_error);
}

TEST_CASE("update_property")
{
    brayns::PropertyMap properties;
    properties.setProperty({"foo", 1});
    properties.updateProperty("foo", 42);
    CHECK_EQ(properties.getProperties()[0]->get<int32_t>(), 42);

    CHECK_THROWS_AS(properties.updateProperty("foo", false),
                    std::runtime_error);

    CHECK(properties.hasProperty("foo"));
    properties.setProperty({"foo", 0});
    CHECK_EQ(properties.getProperties()[0]->get<int32_t>(), 0);

    CHECK(!properties.hasProperty("bar"));
    CHECK_NOTHROW(properties.updateProperty("bar", 42));
}

TEST_CASE("get_property")
{
    brayns::PropertyMap properties;
    properties.setProperty({"foo", 1});

    CHECK_EQ(properties.getProperty("foo", 5), 1);
    CHECK_EQ(properties.getProperty<int32_t>("foo"), 1);
    CHECK_EQ(properties.getProperty("bla", 5), 5);
    CHECK_THROWS_AS(properties.getProperty<bool>("bar"), std::runtime_error);
    CHECK_THROWS_AS(properties.getPropertyType("bar"), std::runtime_error);
}

TEST_CASE("set_and_get_all_supported_types")
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

    CHECK_EQ(properties.getProperty<int32_t>("int"), 42);
    CHECK_EQ(properties.getProperty<std::string>("enum"), "Zero");
    CHECK_EQ(properties.getEnums("enum").size(), 3);
    CHECK_EQ(properties.getProperty<double>("double"), 1.2);
    CHECK_EQ(properties.getProperty<std::string>("string"), "foo");
    CHECK(properties.getProperty<bool>("bool"));
    CHECK((properties.getProperty<std::array<int32_t, 2>>("vec2i") ==
           std::array<int32_t, 2>{{1, 2}}));
    CHECK((properties.getProperty<std::array<double, 2>>("vec2d") ==
           std::array<double, 2>{{1, 2}}));
    CHECK((properties.getProperty<std::array<int32_t, 3>>("vec3i") ==
           std::array<int32_t, 3>{{1, 2, 3}}));
    CHECK((properties.getProperty<std::array<double, 3>>("vec3d") ==
           std::array<double, 3>{{1, 2, 3}}));
    CHECK((properties.getProperty<std::array<double, 4>>("vec4d") ==
           std::array<double, 4>{{1, 2, 3, 4}}));

    using Type = brayns::Property::Type;
    CHECK(properties.getPropertyType("int") == Type::Int);
    CHECK(properties.getPropertyType("double") == Type::Double);
    CHECK(properties.getPropertyType("string") == Type::String);
    CHECK(properties.getPropertyType("bool") == Type::Bool);
    CHECK(properties.getPropertyType("vec2i") == Type::Vec2i);
    CHECK(properties.getPropertyType("vec2d") == Type::Vec2d);
    CHECK(properties.getPropertyType("vec3i") == Type::Vec3i);
    CHECK(properties.getPropertyType("vec3d") == Type::Vec3d);
    CHECK(properties.getPropertyType("vec4d") == Type::Vec4d);
}

TEST_CASE("fill_property_map")
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

    CHECK(propInts.getPropertyType("number") == Type::Int);
    CHECK(propInts.getPropertyType("vec2") == Type::Vec2i);
    CHECK(propInts.getPropertyType("vec3") == Type::Vec3i);

    CHECK(propDoubles.getPropertyType("number") == Type::Double);
    CHECK(propDoubles.getPropertyType("vec2") == Type::Vec2d);
    CHECK(propDoubles.getPropertyType("vec3") == Type::Vec3d);

    CHECK_EQ(propInts.getProperty<int32_t>("number"),
             propDoubles.getProperty<double>("number"));
    {
        const auto aInts = propInts.getProperty<std::array<int, 2>>("vec2");
        const auto aDoubles =
            propDoubles.getProperty<std::array<double, 2>>("vec2");
        CHECK_EQ(aInts[0], aDoubles[0]);
        CHECK_EQ(aInts[1], aDoubles[1]);
    }
    {
        const auto aInts = propInts.getProperty<std::array<int, 3>>("vec3");
        const auto aDoubles =
            propDoubles.getProperty<std::array<double, 3>>("vec3");
        CHECK_EQ(aInts[0], aDoubles[0]);
        CHECK_EQ(aInts[1], aDoubles[1]);
        CHECK_EQ(aInts[2], aDoubles[2]);
    }

    propInts.setProperty({"foo", std::string("string")});
    propDoubles.setProperty({"foo", 42});
    CHECK_THROWS_AS(propInts.merge(propDoubles), std::runtime_error);
}

TEST_CASE("update_properties")
{
    brayns::PropertyMap source;
    source.setProperty({"number", 42});
    source.setProperty({"vec2", std::array<int32_t, 2>{{1, 2}}});
    source.setProperty({"vec3", std::array<int32_t, 3>{{1, 2, 3}}});

    brayns::PropertyMap dest;
    dest.setProperty({"number", 27});
    dest.setProperty({"vec2", std::array<int32_t, 2>{{0, 1}}});

    dest.update(source);
    CHECK_EQ(dest.getProperty<int32_t>("number"), 42);
    const auto array = dest.getProperty<std::array<int32_t, 2>>("vec2");
    CHECK_EQ(1, array[0]);
    CHECK_EQ(2, array[1]);

    dest.setProperty({"vec3", 10});
    CHECK_THROWS_AS(dest.update(source), std::runtime_error);
}

TEST_CASE("merge_enums")
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
        CHECK(propIntsTmp.empty());
        propIntsTmp.update(propStrings);
        CHECK(propIntsTmp.empty());

        propIntsTmp.merge(propInts);
        propIntsTmp.merge(propStrings);

        CHECK(propIntsTmp.getPropertyType("abc") == Type::Int);
        CHECK_EQ(propIntsTmp.getProperty<int32_t>("abc"), 2);

        propIntsTmp.update(propInts);
        CHECK(propIntsTmp.getPropertyType("abc") == Type::Int);
        CHECK_EQ(propIntsTmp.getProperty<int32_t>("abc"), 1);

        propIntsTmp.update(propStrings);
        CHECK(propIntsTmp.getPropertyType("abc") == Type::Int);
        CHECK_EQ(propIntsTmp.getProperty<int32_t>("abc"), 2);
    }

    {
        brayns::PropertyMap propStringsTmp;
        propStringsTmp.merge(propStrings);
        propStringsTmp.merge(propInts);

        CHECK(propStringsTmp.getPropertyType("abc") == Type::String);
        CHECK_EQ(propStringsTmp.getProperty<std::string>("abc"), "b");

        propStringsTmp.update(propStrings);
        CHECK(propStringsTmp.getPropertyType("abc") == Type::String);
        CHECK_EQ(propStringsTmp.getProperty<std::string>("abc"), "c");

        propStringsTmp.update(propInts);
        CHECK(propStringsTmp.getPropertyType("abc") == Type::String);
        CHECK_EQ(propStringsTmp.getProperty<std::string>("abc"), "b");
    }
}

TEST_CASE("commandline")
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

    const char* argv[] = {"propertyMap", "--int",
                          "5",           "--double",
                          "0.4",         "--enum",
                          "One",         "--int-enum",
                          "Three",       "--string",
                          "bar",         "--bool",
                          "off",         "--bool-switch",
                          "--vec2i",     "3",
                          "4",           "--vec2d",
                          "1.2",         "2.3",
                          "--vec3i",     "3",
                          "4",           "5",
                          "--vec3d",     "1.2",
                          "2.3",         "3.4",
                          "--vec4d",     "1.2",
                          "2.3",         "3.4",
                          "4.5"};
    const int argc = sizeof(argv) / sizeof(char*);

    REQUIRE(properties.parse(argc, argv));

    CHECK_EQ(properties.getProperty<int32_t>("int"), 5);
    CHECK_EQ(properties.getProperty<double>("double"), 0.4);
    CHECK_EQ(properties.getProperty<std::string>("enum"), "One");
    CHECK_EQ(properties.getProperty<int32_t>("intEnum"), 2);
    CHECK_EQ(properties.getProperty<std::string>("string"), "bar");
    CHECK(!properties.getProperty<bool>("bool"));
    CHECK(properties.getProperty<bool>("boolSwitch"));
    CHECK((properties.getProperty<std::array<int32_t, 2>>("vec2i") ==
           std::array<int32_t, 2>{{3, 4}}));
    CHECK((properties.getProperty<std::array<double, 2>>("vec2d") ==
           std::array<double, 2>{{1.2, 2.3}}));
    CHECK((properties.getProperty<std::array<int32_t, 3>>("vec3i") ==
           std::array<int32_t, 3>{{3, 4, 5}}));
    CHECK((properties.getProperty<std::array<double, 3>>("vec3d") ==
           std::array<double, 3>{{1.2, 2.3, 3.4}}));
    CHECK((properties.getProperty<std::array<double, 4>>("vec4d") ==
           std::array<double, 4>{{1.2, 2.3, 3.4, 4.5}}));
}

TEST_CASE("commandline_no_option")
{
    brayns::PropertyMap properties;
    properties.setProperty({"int", 42});

    const char* argv[] = {"propertyMap"};
    const int argc = sizeof(argv) / sizeof(char*);

    REQUIRE(properties.parse(argc, argv));

    CHECK_EQ(properties.getProperty<int32_t>("int"), 42);
}

TEST_CASE("commandline_unknown_option")
{
    brayns::PropertyMap properties;
    properties.setProperty({"vec2i", std::array<int32_t, 2>{{1, 2}}});

    const char* argv[] = {"propertyMap", "--vec3i", "3", "4", "5"};
    const int argc = sizeof(argv) / sizeof(char*);

    CHECK(!properties.parse(argc, argv));
}

TEST_CASE("commandline_too_many_vector_values")
{
    brayns::PropertyMap properties;
    properties.setProperty({"vec2i", std::array<int32_t, 2>{{1, 2}}});

    const char* argv[] = {"propertyMap", "--vec2i", "3", "4", "5"};
    const int argc = sizeof(argv) / sizeof(char*);

    CHECK(properties.parse(argc, argv));
}

TEST_CASE("commandline_wrong_enum_value")
{
    brayns::PropertyMap properties;
    properties.setProperty(
        {"enum", std::string("Zero"), {"Zero", "One", "Two"}, {}});

    const char* argv[] = {"propertyMap", "--enum", "Four"};
    const int argc = sizeof(argv) / sizeof(char*);

    CHECK(!properties.parse(argc, argv));
}
