/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "TestCase.h"

#include "PlaceholderEngine.h"

#include <brayns/core/engine/components/Geometries.h>
#include <brayns/core/engine/geometry/types/Sphere.h>
#include <brayns/core/engine/model/ModelInstance.h>
#include <brayns/core/engine/systems/GenericBoundsSystem.h>

TEST_CASE("ModelInstance")
{
    BRAYNS_TESTS_PLACEHOLDER_ENGINE

    SUBCASE("Bounds")
    {
        auto model = std::make_shared<brayns::Model>("");
        auto &components = model->getComponents();
        components.add<brayns::Geometries>(brayns::Sphere{brayns::Vector3f(0.f), 10.f});
        auto &systems = model->getSystems();
        systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();

        auto instance = brayns::ModelInstance(0, model);
        auto &bounds = instance.getBounds();
        CHECK(bounds.getMin() == brayns::Vector3f(-10.f));
        CHECK(bounds.getMax() == brayns::Vector3f(10.f));

        auto transform = brayns::Transform{brayns::Vector3f(0.f, 100.f, 0.f)};
        instance.setTransform(transform);
        CHECK(bounds.getMin() == brayns::Vector3f(-10.f, 90.f, -10.f));
        CHECK(bounds.getMax() == brayns::Vector3f(10.f, 110.f, 10.f));
    }
    SUBCASE("Transform")
    {
        auto model = std::make_shared<brayns::Model>("");
        auto instance = brayns::ModelInstance(0, model);
        instance.commit();
        CHECK(!instance.commit());

        auto transformCopy = instance.getTransform();
        instance.setTransform(transformCopy);
        CHECK(!instance.commit());

        auto transform = brayns::Transform{brayns::Vector3f(0.f, 100.f, 0.f)};
        instance.setTransform(transform);
        CHECK(instance.commit());
    }
    SUBCASE("Visibility")
    {
        auto model = std::make_shared<brayns::Model>("");
        auto instance = brayns::ModelInstance(0, model);
        instance.commit();
        CHECK(!instance.commit());

        instance.setVisible(instance.isVisible());
        CHECK(!instance.commit());

        instance.setVisible(!instance.isVisible());
        CHECK(instance.commit());
    }
}
