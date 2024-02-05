/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/common/propertymap/PropertyMap.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

TEST_CASE("set_property")
{
    brayns::PropertyMap properties;
    properties.add({"foo", 1});
    CHECK(properties.size() == 1);
    CHECK(properties[0].as<int32_t>() == 1);
    CHECK(properties[0].getLabel() == "");
    CHECK(properties[0].getDescription() == "no-description");
}

TEST_CASE("set_property_with_metadata")
{
    brayns::PropertyMap properties;
    properties.add({"foo", 1, {"Foo", "Foo description"}});
    CHECK(properties.size() == 1);
    CHECK(properties[0].as<int32_t>() == 1);
    CHECK(properties[0].getLabel() == "Foo");
    CHECK(properties[0].getDescription() == "Foo description");
}

TEST_CASE("set_property_from_other_property")
{
    brayns::PropertyMap properties;
    brayns::PropertyMap otherProperties;
    otherProperties.add({"bar", 42.});
    properties.add(otherProperties[0]);
    CHECK(properties[0].as<double>() == 42.);
    CHECK_THROWS(otherProperties.add({"bar", std::string("no double")}));
}

TEST_CASE("update_property")
{
    brayns::PropertyMap properties;
    properties.add({"foo", 1});
    properties.update("foo", 42);
    CHECK(properties[0].as<int32_t>() == 42);
    CHECK_THROWS(properties.update("foo", false));
    CHECK(properties.find("foo"));
    properties.add({"foo", 0});
    CHECK(properties[0].as<int32_t>() == 0);
    CHECK(!properties.find("bar"));
    CHECK_NOTHROW(properties.update("bar", 42));
}

TEST_CASE("get_property")
{
    brayns::PropertyMap properties;
    properties.add({"foo", 1});
    CHECK(properties.valueOr("foo", 5) == 1);
    CHECK(properties["foo"].as<int32_t>() == 1);
    CHECK(properties.valueOr("bla", 5) == 5);
    CHECK_THROWS(properties["bar"].as<bool>());
    CHECK_THROWS(properties["bar"].getType());
}

TEST_CASE("set_and_get_all_supported_types")
{
    brayns::PropertyMap properties;

    properties.add({"int", 42});
    properties.add({"enum", {"Zero", {"Zero", "One", "Two"}}});
    properties.add({"double", 1.2});
    properties.add({"string", std::string("foo")});
    properties.add({"bool", true});
    properties.add({"vec2i", brayns::Vector2i{1, 2}});
    properties.add({"vec2d", brayns::Vector2d{1, 2}});
    properties.add({"vec3i", brayns::Vector3i{1, 2, 3}});
    properties.add({"vec3d", brayns::Vector3d{1, 2, 3}});
    properties.add({"vec4d", brayns::Vector4d{1, 2, 3, 4}});

    CHECK(properties["int"].as<int32_t>() == 42);
    CHECK(properties["enum"].as<brayns::EnumProperty>().toString() == "Zero");
    CHECK(properties["enum"].as<brayns::EnumProperty>().getValues().size() ==
          3);
    CHECK(properties["double"].as<double>() == 1.2);
    CHECK(properties["string"].as<std::string>() == "foo");
    CHECK(properties["bool"].as<bool>());
    CHECK(
        (properties["vec2i"].as<brayns::Vector2i>() == brayns::Vector2i{1, 2}));
    CHECK(
        (properties["vec2d"].as<brayns::Vector2d>() == brayns::Vector2d{1, 2}));
    CHECK((properties["vec3i"].as<brayns::Vector3i>() ==
           brayns::Vector3i{1, 2, 3}));
    CHECK((properties["vec3d"].as<brayns::Vector3d>() ==
           brayns::Vector3d{1, 2, 3}));
    CHECK((properties["vec4d"].as<brayns::Vector4d>() ==
           brayns::Vector4d{1, 2, 3, 4}));

    CHECK(properties["int"].is<int32_t>());
    CHECK(properties["double"].is<double>());
    CHECK(properties["string"].is<std::string>());
    CHECK(properties["bool"].is<bool>());
    CHECK(properties["vec2i"].is<brayns::Vector2i>());
    CHECK(properties["vec2d"].is<brayns::Vector2d>());
    CHECK(properties["vec3i"].is<brayns::Vector3i>());
    CHECK(properties["vec3d"].is<brayns::Vector3d>());
    CHECK(properties["vec4d"].is<brayns::Vector4d>());
}

TEST_CASE("fill_property_map")
{
    brayns::PropertyMap integers;
    integers.add({"number", 42});
    integers.add({"vec2", brayns::Vector2i{1, 2}});
    integers.add({"vec3", brayns::Vector3i{1, 2, 3}});

    brayns::PropertyMap doubles;
    doubles.add({"number", -42.0});
    doubles.add({"vec2", brayns::Vector2d{-1, -2}});
    doubles.add({"vec3", brayns::Vector3d{-1, -2, -3}});

    integers.merge(doubles);
    doubles.merge(integers);

    CHECK(integers["number"].is<int32_t>());
    CHECK(integers["vec2"].is<brayns::Vector2i>());
    CHECK(integers["vec3"].is<brayns::Vector3i>());

    CHECK(doubles["number"].is<double>());
    CHECK(doubles["vec2"].is<brayns::Vector2d>());
    CHECK(doubles["vec3"].is<brayns::Vector3d>());

    CHECK(integers["number"].as<int32_t>() == doubles["number"].as<double>());
    CHECK(integers["vec2"].as<brayns::Vector2i>()[0] ==
          doubles["vec2"].as<brayns::Vector2d>()[0]);
    CHECK(integers["vec2"].as<brayns::Vector2i>()[1] ==
          doubles["vec2"].as<brayns::Vector2d>()[1]);
    CHECK(integers["vec3"].as<brayns::Vector3i>()[0] ==
          doubles["vec3"].as<brayns::Vector3d>()[0]);
    CHECK(integers["vec3"].as<brayns::Vector3i>()[1] ==
          doubles["vec3"].as<brayns::Vector3d>()[1]);
    CHECK(integers["vec3"].as<brayns::Vector3i>()[2] ==
          doubles["vec3"].as<brayns::Vector3d>()[2]);

    integers.add({"foo", std::string("string")});
    doubles.add({"foo", 42});
    CHECK_THROWS(integers.merge(doubles));
}

TEST_CASE("update_properties")
{
    brayns::PropertyMap from;
    from.add({"number", 42});
    from.add({"vec2", brayns::Vector2i{1, 2}});
    from.add({"vec3", brayns::Vector3i{1, 2, 3}});

    brayns::PropertyMap to;
    to.add({"number", 27});
    to.add({"vec2", brayns::Vector2i{0, 1}});

    to.update(from);
    CHECK(to["number"].as<int32_t>() == 42);
    CHECK((to["vec2"].as<brayns::Vector2i>() == brayns::Vector2i{1, 2}));

    to.add({"vec3", 10});
    CHECK_THROWS(to.update(from));
}

TEST_CASE("merge_enums")
{
    const std::vector<std::string> enums = {"a", "b", "c"};

    brayns::PropertyMap integers;
    integers.add({"abc", {1, enums}});

    brayns::PropertyMap labels;
    labels.add({"abc", {"c", enums}});

    {
        brayns::PropertyMap tmp;

        tmp.update(integers);
        CHECK(tmp.empty());
        tmp.update(labels);
        CHECK(tmp.empty());

        tmp.merge(integers);
        tmp.merge(labels);

        CHECK(tmp["abc"].is<brayns::EnumProperty>());
        CHECK(tmp["abc"].to<int32_t>() == 2);
        CHECK(tmp["abc"].as<brayns::EnumProperty>().toInt() == 2);

        tmp.update(integers);
        CHECK(tmp["abc"].is<brayns::EnumProperty>());
        CHECK(tmp["abc"].to<int32_t>() == 1);
        CHECK(tmp["abc"].as<brayns::EnumProperty>().toInt() == 1);

        tmp.update(labels);
        CHECK(tmp["abc"].is<brayns::EnumProperty>());
        CHECK(tmp["abc"].to<int32_t>() == 2);
        CHECK(tmp["abc"].as<brayns::EnumProperty>().toInt() == 2);
    }

    {
        brayns::PropertyMap tmp;
        tmp.merge(labels);
        tmp.merge(integers);

        CHECK(tmp["abc"].is<brayns::EnumProperty>());
        CHECK(tmp["abc"].to<std::string>() == "b");
        CHECK(tmp["abc"].as<brayns::EnumProperty>().toString() == "b");

        tmp.update(labels);
        CHECK(tmp["abc"].is<brayns::EnumProperty>());
        CHECK(tmp["abc"].to<std::string>() == "c");
        CHECK(tmp["abc"].as<brayns::EnumProperty>().toString() == "c");

        tmp.update(integers);
        CHECK(tmp["abc"].is<brayns::EnumProperty>());
        CHECK(tmp["abc"].to<std::string>() == "b");
        CHECK(tmp["abc"].as<brayns::EnumProperty>().toString() == "b");
    }
}
