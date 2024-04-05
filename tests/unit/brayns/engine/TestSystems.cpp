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

#include <doctest/doctest.h>

#include <brayns/core/engine/components/Transform.h>
#include <brayns/core/engine/model/SystemsView.h>

namespace
{
class MockBoundsSystem : public brayns::BoundsSystem
{
public:
    brayns::Bounds compute(const brayns::TransformMatrix &matrix, brayns::Components &components) override
    {
        auto &vector = components.add<brayns::Vector3f>(0.f, 0.f, 0.f);
        vector = matrix.transformPoint(vector);
        return brayns::Bounds();
    }
};

class MockColorSystem : public brayns::ColorSystem
{
public:
    std::vector<std::string> getMethods() const override
    {
        return {"method1", "method2"};
    }

    std::vector<std::string> getValues(const std::string &method, brayns::Components &components) const override
    {
        (void)components;

        if (method == "method1")
        {
            return {"a", "b"};
        }

        return {"c", "d"};
    }

    void apply(
        const std::string &method,
        const brayns::ColorMethodInput &input,
        brayns::Components &components) const override
    {
        auto &color = components.add<brayns::Vector4f>();

        if (method == "method1")
        {
            color = input.at("a");
            return;
        }

        color = input.at("c");
    }
};

class MockInspectSystem : public brayns::InspectSystem
{
public:
    brayns::InspectResultData execute(const brayns::InspectContext &context, brayns::Components &components) override
    {
        (void)context;
        (void)components;
        auto result = brayns::InspectResultData();
        result.set("key", "a_value");
        return result;
    }
};

class MockUpdateSystem : public brayns::UpdateSystem
{
public:
    void execute(const brayns::ParametersManager &parameters, brayns::Components &components) override
    {
        auto &simulation = parameters.getSimulationParameters();
        components.add<uint32_t>(simulation.getFrame());
    }
};
}

TEST_CASE("Systems")
{
    SUBCASE("Bounds system")
    {
        auto components = brayns::Components();
        auto systems = brayns::Systems();
        systems.setBoundsSystem<MockBoundsSystem>();
        auto view = brayns::SystemsView(systems, components);
        auto matrix = brayns::Transform{.translation = brayns::Vector3f(0.f, 0.f, 100.f)}.toMatrix();

        CHECK(!components.has<brayns::Vector3f>());

        view.computeBounds(matrix);

        CHECK(components.has<brayns::Vector3f>());
        CHECK(components.get<brayns::Vector3f>() == brayns::Vector3f(0.f, 0.f, 100.f));
    }
    SUBCASE("Color system")
    {
        auto components = brayns::Components();
        auto systems = brayns::Systems();
        systems.setColorSystem<MockColorSystem>();
        auto view = brayns::SystemsView(systems, components);

        CHECK(view.getColorMethods() == std::vector<std::string>{"method1", "method2"});
        CHECK(view.getColorValues("method1") == std::vector<std::string>{"a", "b"});
        CHECK(view.getColorValues("method2") == std::vector<std::string>{"c", "d"});

        CHECK(!components.has<brayns::Vector4f>());
        view.applyColor("method2", {{"c", brayns::Vector4f(1.f, 2.f, 3.f, 4.f)}});

        CHECK(components.has<brayns::Vector4f>());
        CHECK(components.get<brayns::Vector4f>() == brayns::Vector4f(1.f, 2.f, 3.f, 4.f));
    }
    SUBCASE("Inspect system")
    {
        auto components = brayns::Components();
        auto systems = brayns::Systems();
        systems.setInspectSystem<MockInspectSystem>();
        auto view = brayns::SystemsView(systems, components);
        auto result = view.inspect(brayns::InspectContext());
        CHECK(result.has("key"));
    }
    SUBCASE("Update system")
    {
        auto components = brayns::Components();
        auto systems = brayns::Systems();
        systems.setUpdateSystem<MockUpdateSystem>();
        auto view = brayns::SystemsView(systems, components);

        auto parameters = brayns::ParametersManager(0, nullptr);
        auto &simulation = parameters.getSimulationParameters();
        simulation.setFrame(101);

        CHECK(!components.has<uint32_t>());
        view.update(parameters);
        CHECK(components.has<uint32_t>());
        CHECK(components.get<uint32_t>() == simulation.getFrame());
    }
}
