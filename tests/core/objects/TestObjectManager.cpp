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
    float someFloat = 0.0F;
};

template<>
struct JsonObjectReflector<TestProperties>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<TestProperties>();
        builder.field("some_float", [](auto &object) { return &object.someFloat; });
        return builder.build();
    }
};

struct TestObject
{
    std::string type;
    std::size_t size;
    bool removed = false;
};

template<>
struct ObjectReflector<TestObject>
{
    static std::string getType(const TestObject &object)
    {
        return object.type;
    }

    static std::size_t getSize(const TestObject &object)
    {
        return object.size;
    }

    static void remove(TestObject &object)
    {
        object.removed = true;
    }
};
}

TEST_CASE("Create and remove objects")
{
    auto objects = ObjectManager();

    auto object = objects.add<TestObject>({"type", 4}, "someUserData");

    auto id = object.getId();

    CHECK_EQ(id, 1);

    auto metadata = objects.getMetadata(id);

    CHECK_EQ(metadata.id, id);
    CHECK_EQ(metadata.type, "type");
    CHECK_EQ(metadata.size, 4);
    CHECK_EQ(metadata.userData.extract<std::string>(), "someUserData");

    auto &retreived = objects.get<TestObject>(id);

    CHECK_EQ(retreived.size, 4);
    CHECK_EQ(retreived.type, "type");
    CHECK_FALSE(retreived.removed);

    auto stored = objects.getStored<TestObject>(id);

    CHECK_EQ(stored.getId(), id);

    auto object2 = objects.add<TestObject>({});
    auto id2 = object2.getId();

    CHECK_EQ(objects.getAllMetadata().size(), 2);

    objects.remove(id);

    CHECK(stored.isRemoved());
    CHECK(stored.get().removed);

    CHECK_THROWS_AS(objects.getMetadata(id), InvalidParams);
    CHECK_THROWS_AS(objects.get<TestObject>(id), InvalidParams);
    CHECK_THROWS_AS(objects.getStored<TestObject>(id), InvalidParams);

    auto object3 = objects.add<TestObject>({});
    CHECK_EQ(object3.getId(), 1);

    auto stored2 = objects.getStored<TestObject>(id2);

    objects.clear();

    CHECK(stored2.isRemoved());
    CHECK(object3.isRemoved());

    CHECK(objects.getAllMetadata().empty());
}

TEST_CASE("Errors")
{
    auto objects = ObjectManager();

    objects.add<TestObject>({});

    CHECK_THROWS_AS(objects.getMetadata(0), InvalidParams);
    CHECK_THROWS_AS(objects.getMetadata(2), InvalidParams);
    CHECK_THROWS_AS(objects.remove(2), InvalidParams);
}
