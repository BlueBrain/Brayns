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
struct TestSettings
{
    int someInt = 0;
    std::string someString = {};
};

template<>
struct JsonObjectReflector<TestSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<TestSettings>();
        builder.field("some_int", [](auto &object) { return &object.someInt; });
        builder.field("some_string", [](auto &object) { return &object.someString; });
        return builder.build();
    }
};

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

struct TestValue
{
    ObjectId id;
    TestSettings settings;
};

using TestObject = UserObject<TestValue>;

template<>
struct ObjectReflector<TestObject>
{
    using Settings = TestSettings;

    static std::string getType()
    {
        return "test";
    }

    static TestProperties getProperties(const TestObject &object)
    {
        return {static_cast<float>(object.value.settings.someInt)};
    }

    static std::size_t getSize(const TestObject &object)
    {
        return object.value.settings.someString.size();
    }
};

void registerTestObject(ObjectManager &objects)
{
    objects.addFactory<TestObject>([](auto id, auto settings) { return TestValue{id, std::move(settings)}; });
}
}

TEST_CASE("Create and remove objects")
{
    auto objects = ObjectManager();
    registerTestObject(objects);

    auto &object = objects.create<TestObject>({2, "data"}, "someUserData");

    CHECK_EQ(object.id, 1);

    auto metadata = objects.getMetadata(object.id);

    CHECK_EQ(metadata.id, object.id);
    CHECK_EQ(metadata.type, "test");
    CHECK_EQ(metadata.size, 4);
    CHECK_EQ(metadata.userData.extract<std::string>(), "someUserData");

    auto properties = objects.getProperties<TestObject>(object.id);

    CHECK_EQ(properties.someFloat, 2.0F);

    auto &retreived = objects.get<TestObject>(object.id);

    CHECK_EQ(retreived.id, object.id);
    CHECK_EQ(retreived.userData, object.userData);
    CHECK_EQ(object.value.settings.someInt, 2);
    CHECK_EQ(object.value.settings.someString, "data");

    auto shared = objects.getShared<TestObject>(object.id);

    auto &object2 = objects.create<TestObject>({3, "data2"}, "someUserData2");

    CHECK_EQ(objects.getAllMetadata().size(), 2);

    objects.remove(object.id);

    CHECK_EQ(shared->id, nullId);

    CHECK_THROWS_AS(objects.getMetadata(object.id), InvalidParams);
    CHECK_THROWS_AS(objects.getProperties<TestObject>(object.id), InvalidParams);
    CHECK_THROWS_AS(objects.getResult<TestObject>(object.id), InvalidParams);

    auto shared2 = objects.getShared<TestObject>(object2.id);

    objects.clear();

    CHECK_EQ(shared2->id, nullId);

    CHECK(objects.getAllMetadata().empty());
}

TEST_CASE("Errors")
{
    auto objects = ObjectManager();
    registerTestObject(objects);

    objects.create<TestObject>({});

    CHECK_THROWS_AS(objects.getMetadata(0), InvalidParams);
    CHECK_THROWS_AS(objects.getMetadata(2), InvalidParams);
    CHECK_THROWS_AS(objects.remove(2), InvalidParams);
}
