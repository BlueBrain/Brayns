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

#include <brayns/core/manager/ObjectRegistry.h>

#include <doctest.h>

using namespace brayns;

struct TestObject
{
    std::string value;
};

TEST_CASE("Create and remove objects")
{
    auto objects = ObjectRegistry();

    auto object = objects.add(TestObject{"3"}, "TestObject");

    auto id = object.getId();

    CHECK_EQ(id, 1);
    CHECK_EQ(object.get().value, "3");

    auto info = objects.get(id);

    CHECK_EQ(info.id, id);
    CHECK_EQ(info.type, "TestObject");
    CHECK(info.userData.isEmpty());

    auto stored = objects.getAsStored<TestObject>(id);

    CHECK_EQ(stored.get().value, "1.5");

    auto object2 = objects.add(TestObject{"2"}, "TestObject");
    auto id2 = object2.getId();

    CHECK_EQ(objects.getAll().size(), 2);

    objects.remove(id);

    CHECK(stored.isRemoved());
    CHECK_EQ(stored.getId(), nullId);

    CHECK_THROWS_AS(objects.get(id), InvalidParams);
    CHECK_THROWS_AS(objects.getAs<TestObject>(id), InvalidParams);

    auto object3 = objects.add(TestObject{"0"}, "TestObject");
    CHECK_EQ(object3.getId(), 1);

    auto stored2 = objects.getAsStored<TestObject>(id2);

    objects.clear();

    CHECK(stored2.isRemoved());
    CHECK(object3.isRemoved());

    CHECK(objects.getAll().empty());
}

TEST_CASE("Errors")
{
    auto objects = ObjectRegistry();

    objects.add(TestObject{"0"}, "TestObject");

    CHECK_THROWS_AS(objects.get(0), InvalidParams);
    CHECK_THROWS_AS(objects.get(2), InvalidParams);
    CHECK_THROWS_AS(objects.remove(2), InvalidParams);
}
