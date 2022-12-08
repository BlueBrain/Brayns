/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Authors: Daniel.Nachbaur@epfl.ch
 *                      Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/Brayns.h>

#include <brayns/engine/renderer/types/Interactive.h>

#include <doctest/doctest.h>

TEST_CASE("Simple construction")
{
    std::vector<const char *> argv = {"brayns"};
    CHECK_NOTHROW(brayns::Brayns(static_cast<int>(argv.size()), argv.data()));
}

TEST_CASE("Defaults")
{
    brayns::Brayns brayns(0, nullptr);

    auto &engine = brayns.getEngine();

    auto &camera = engine.getCamera();
    const auto &view = camera.getView();
    CHECK_EQ(camera.getName(), "perspective");
    CHECK_EQ(view.position, brayns::Vector3f(0.f));
    CHECK_EQ(view.target, brayns::Vector3f(0.f, 0.f, 1.f));
    CHECK_EQ(view.up, brayns::Vector3f(0.f, 1.f, 0.f));

    auto &renderer = engine.getRenderer();
    CHECK_EQ(renderer.getName(), "interactive");
    CHECK_EQ(renderer.getSamplesPerPixel(), 1);
    auto interactive = renderer.as<brayns::Interactive>();
    CHECK_EQ(interactive->backgroundColor, brayns::Vector4f(0.004f, 0.016f, 0.102f, 0.f));

    const auto &pm = brayns.getParametersManager();
    const auto &appParams = pm.getApplicationParameters();
    CHECK_EQ(appParams.getWindowSize(), brayns::Vector2ui(800, 600));

    const auto &simulation = pm.getSimulationParameters();
    CHECK_EQ(simulation.getFrame(), 0);
}
