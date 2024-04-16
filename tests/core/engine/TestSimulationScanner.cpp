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

#include <brayns/core/engine/common/SimulationScanner.h>
#include <brayns/core/engine/components/SimulationInfo.h>
#include <brayns/core/engine/scene/ModelManager.h>

#include "PlaceholderEngine.h"

#include <doctest.h>

namespace
{
class AddSimulatedModels
{
public:
    static void add(brayns::ModelManager &models, double start, double end, double dt, bool enabled)
    {
        auto model = std::make_shared<brayns::Model>("test");
        auto &components = model->getComponents();
        auto &simInfo = components.add<brayns::SimulationInfo>();
        simInfo.dt = dt;
        simInfo.enabled = enabled;
        simInfo.endTime = end;
        simInfo.startTime = start;
        models.add(std::move(model));
    }
};
}

TEST_CASE("simulation_scanner")
{
    BRAYNS_TESTS_PLACEHOLDER_ENGINE

    auto models = brayns::ModelManager();
    auto referenceSimulation = brayns::SimulationParameters();

    SUBCASE("empty_model_list")
    {
        auto simulation = brayns::SimulationParameters();
        simulation.resetModified();

        brayns::SimulationScanner::scanAndUpdate(models, simulation);
        CHECK(simulation.getDt() == referenceSimulation.getDt());
        CHECK(simulation.getEndFrame() == referenceSimulation.getEndFrame());
        CHECK(simulation.getFrame() == referenceSimulation.getFrame());
        CHECK(simulation.getStartFrame() == referenceSimulation.getStartFrame());
        CHECK(!simulation.isModified());
    }

    SUBCASE("no_simulated_models")
    {
        auto simulation = brayns::SimulationParameters();
        simulation.resetModified();

        models.add(std::make_shared<brayns::Model>("test"));
        brayns::SimulationScanner::scanAndUpdate(models, simulation);
        CHECK(simulation.getDt() == referenceSimulation.getDt());
        CHECK(simulation.getEndFrame() == referenceSimulation.getEndFrame());
        CHECK(simulation.getFrame() == referenceSimulation.getFrame());
        CHECK(simulation.getStartFrame() == referenceSimulation.getStartFrame());
        CHECK(!simulation.isModified());
    }

    SUBCASE("simulated_models")
    {
        auto simulation = brayns::SimulationParameters();
        simulation.resetModified();

        AddSimulatedModels::add(models, 1., 10., 0.1, true);
        brayns::SimulationScanner::scanAndUpdate(models, simulation);
        CHECK(simulation.getDt() == doctest::Approx(0.1));
        CHECK(simulation.getEndFrame() == 99);
        CHECK(simulation.getFrame() == 9);
        CHECK(simulation.getStartFrame() == 9);
        CHECK(simulation.isModified());

        simulation.resetModified();

        AddSimulatedModels::add(models, 1., 10., 0.01, false);
        brayns::SimulationScanner::scanAndUpdate(models, simulation);
        CHECK(simulation.getDt() == doctest::Approx(0.01));
        CHECK(simulation.getEndFrame() == 999);
        CHECK(simulation.getFrame() == 99);
        CHECK(simulation.getStartFrame() == 99);
        CHECK(simulation.isModified());

        AddSimulatedModels::add(models, 0., 5., 0.1, true);
        brayns::SimulationScanner::scanAndUpdate(models, simulation);
        CHECK(simulation.getDt() == doctest::Approx(0.01));
        CHECK(simulation.getEndFrame() == 999);
        CHECK(simulation.getFrame() == 99);
        CHECK(simulation.getStartFrame() == 0);
        CHECK(simulation.isModified());
    }
}
