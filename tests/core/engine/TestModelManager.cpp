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

#include <brayns/core/engine/scene/ModelManager.h>

#include "PlaceholderEngine.h"

TEST_CASE("ModelManager")
{
    BRAYNS_TESTS_PLACEHOLDER_ENGINE

    SUBCASE("Add models")
    {
        auto manager = brayns::ModelManager();

        auto model = std::make_shared<brayns::Model>("");
        auto instance = manager.add(model);

        CHECK(instance->getID() == 0);
        CHECK(&instance->getModel() == model.get());

        auto modelList = std::vector<std::shared_ptr<brayns::Model>>{
            std::make_shared<brayns::Model>(""),
            std::make_shared<brayns::Model>(""),
            std::make_shared<brayns::Model>("")};
        auto instances = manager.add(modelList);

        CHECK(manager.getAllModelInstances().size() == 4);

        for (size_t i = 0; i < 3; ++i)
        {
            CHECK(instances[i]->getID() == static_cast<uint32_t>(i + 1));
            CHECK(&instances[i]->getModel() == modelList[i].get());
        }
    }
    SUBCASE("Instantiation")
    {
        auto manager = brayns::ModelManager();

        auto model = std::make_shared<brayns::Model>("");
        auto firstInstance = manager.add(model);

        auto instanceList = manager.createInstances(firstInstance->getID(), 1);
        auto secondInstance = instanceList.front();

        CHECK(firstInstance->getID() == 0);
        CHECK(secondInstance->getID() == 1);
        CHECK(&firstInstance->getModel() == &secondInstance->getModel());
    }
    SUBCASE("Getters")
    {
        auto manager = brayns::ModelManager();
        auto modelList = std::vector<std::shared_ptr<brayns::Model>>{
            std::make_shared<brayns::Model>(""),
            std::make_shared<brayns::Model>(""),
            std::make_shared<brayns::Model>("")};
        auto instances = manager.add(modelList);

        auto &retrived = manager.getModelInstance(instances[1]->getID());
        CHECK(retrived.getID() == instances[1]->getID());

        auto &allInstances = manager.getAllModelInstances();
        for (size_t i = 0; i < 3; ++i)
        {
            CHECK(allInstances[i]->getID() == instances[i]->getID());
        }
    }
    SUBCASE("Remove instances by id")
    {
        auto manager = brayns::ModelManager();
        auto modelList = std::vector<std::shared_ptr<brayns::Model>>{
            std::make_shared<brayns::Model>(""),
            std::make_shared<brayns::Model>(""),
            std::make_shared<brayns::Model>("")};
        auto instances = manager.add(modelList);

        manager.removeModelInstancesById({instances[1]->getID()});
        auto &allInstances = manager.getAllModelInstances();
        CHECK(allInstances.size() == 2);
        CHECK(allInstances[0]->getID() == instances[0]->getID());
        CHECK(allInstances[1]->getID() == instances[2]->getID());

        manager.removeAllModelInstances();
        CHECK(manager.getAllModelInstances().empty());
    }
    SUBCASE("Remove instance by condition")
    {
        auto manager = brayns::ModelManager();

        auto model1 = std::make_shared<brayns::Model>("");
        auto &components1 = model1->getComponents();
        components1.add<int>();
        components1.add<float>();
        manager.add(model1);

        auto model2 = std::make_shared<brayns::Model>("");
        auto &components2 = model2->getComponents();
        components2.add<int>();
        components2.add<bool>();
        auto instance2 = manager.add(model2);

        manager.removeModelInstances(
            [](const brayns::ModelInstance &instance)
            {
                auto &model = instance.getModel();
                auto &components = model.getComponents();
                return components.has<float>();
            });

        auto &allInstances = manager.getAllModelInstances();
        CHECK(allInstances.size() == 1);
        CHECK(allInstances[0]->getID() == instance2->getID());
    }
}
