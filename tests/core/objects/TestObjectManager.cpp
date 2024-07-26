/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <brayns/core/objects/ObjectManager.h>

#include <doctest.h>

using namespace brayns;

namespace brayns
{
struct TestProperties
{
    int someInt = 0;
    std::string someString = {};
};

template<>
struct JsonObjectReflector<TestProperties>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<TestProperties>();
        builder.field("some_int", [](auto &object) { return &object.someInt; });
        builder.field("some_string", [](auto &object) { return &object.someString; });
        return builder.build();
    }
};

using TestObject = UserObject<TestProperties>;
}

TEST_CASE("Create and remove objects")
{
    auto objects = ObjectManager();

    auto &object = objects.create<TestObject>({"type", "tag", "someUserData"}, {123, "123"});
    CHECK_EQ(object.metadata.id, 1);
    CHECK_EQ(object.metadata.type, "type");
    CHECK_EQ(object.metadata.tag, "tag");
    CHECK_EQ(object.metadata.userData.extract<std::string>(), "someUserData");
    CHECK_EQ(object.properties.someInt, 123);
    CHECK_EQ(object.properties.someString, "123");

    auto &another = objects.create<TestObject>({"type"}, {});
    CHECK_EQ(another.metadata.id, 2);

    CHECK_EQ(objects.getAllMetadata().size(), 2);

    CHECK_EQ(&objects.get<TestObject>(1), &object);

    CHECK_EQ(&objects.getMetadata(1), &object.metadata);
    CHECK_EQ(objects.getId("tag"), 1);

    auto shared = objects.getShared<TestObject>(1);

    objects.remove(1);

    CHECK_EQ(shared->metadata.id, nullId);

    CHECK_THROWS_AS(objects.getMetadata(1), InvalidParams);

    objects.clear();

    CHECK(objects.getAllMetadata().empty());
}

TEST_CASE("Errors")
{
    auto objects = ObjectManager();

    objects.create<TestObject>({"type", "tag"}, {});

    CHECK_THROWS_AS(objects.create<TestObject>({"type", "tag"}, {}), InvalidParams);

    CHECK_THROWS_AS(objects.getId("invalid tag"), InvalidParams);
    CHECK_THROWS_AS(objects.getMetadata(0), InvalidParams);
    CHECK_THROWS_AS(objects.getMetadata(2), InvalidParams);
    CHECK_THROWS_AS(objects.remove(2), InvalidParams);
    CHECK_THROWS_AS(objects.get<UserObject<int>>(1), InvalidParams);
}
