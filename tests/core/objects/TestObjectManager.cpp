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
struct TestObject
{
    std::string type;
    ObjectId id = nullId;
};

template<>
struct ObjectReflector<TestObject>
{
    static std::string getType(const TestObject &object)
    {
        return object.type;
    }

    static void add(TestObject &object, ObjectId id)
    {
        object.id = id;
    }

    static void remove(TestObject &object)
    {
        object.id = nullId;
    }
};
}

TEST_CASE("Create and remove objects")
{
    auto objects = ObjectManager();

    auto object = objects.add(TestObject{"Type"});

    auto id = object.getId();

    CHECK_EQ(id, 1);

    auto info = objects.getObject(id);

    CHECK_EQ(info.id, id);
    CHECK_EQ(info.type, "Type");
    CHECK(info.userData.isEmpty());

    auto &retreived = objects.get<TestObject>(id);

    CHECK_EQ(retreived.type, "Type");
    CHECK_EQ(retreived.id, id);

    auto stored = objects.getStored<TestObject>(id);

    CHECK_EQ(stored.getId(), id);

    auto object2 = objects.add(TestObject());
    auto id2 = object2.getId();

    CHECK_EQ(objects.getAllObjects().size(), 2);

    objects.remove(id);

    CHECK(stored.isRemoved());
    CHECK_EQ(stored.get().id, nullId);

    CHECK_THROWS_AS(objects.getObject(id), InvalidParams);
    CHECK_THROWS_AS(objects.get<TestObject>(id), InvalidParams);
    CHECK_THROWS_AS(objects.getStored<TestObject>(id), InvalidParams);

    auto object3 = objects.add(TestObject());
    CHECK_EQ(object3.getId(), 1);

    auto stored2 = objects.getStored<TestObject>(id2);

    objects.clear();

    CHECK(stored2.isRemoved());
    CHECK(object3.isRemoved());

    CHECK(objects.getAllObjects().empty());
}

TEST_CASE("Errors")
{
    auto objects = ObjectManager();

    objects.add<TestObject>({});

    CHECK_THROWS_AS(objects.getObject(0), InvalidParams);
    CHECK_THROWS_AS(objects.getObject(2), InvalidParams);
    CHECK_THROWS_AS(objects.remove(2), InvalidParams);
}
