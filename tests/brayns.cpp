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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

TEST_CASE("simple_construction")
{
    std::vector<const char *> argv = {"brayns"};
    CHECK_NOTHROW(brayns::Brayns(static_cast<int>(argv.size()), argv.data()));
}

TEST_CASE("defaults")
{
    brayns::Brayns brayns(0, nullptr);

    auto &engine = brayns.getEngine();

    auto &camera = engine.getCamera();
    const auto &lookAt = camera.getLookAt();
    CHECK_EQ(camera.getName(), "perspective");
    CHECK_EQ(lookAt.position, brayns::Vector3f(0.f));
    CHECK_EQ(lookAt.target, brayns::Vector3f(0.f, 0.f, 1.f));
    CHECK_EQ(lookAt.up, brayns::Vector3f(0.f, 1.f, 0.f));

    auto &fb = engine.getFrameBuffer();
    CHECK(!fb.getColorBuffer());
    CHECK_EQ(fb.getFrameBufferFormat(), brayns::PixelFormat::SRGBA_I8);
    CHECK_EQ(fb.getFrameSize(), brayns::Vector2ui(800, 600));

    auto &renderer = engine.getRenderer();
    CHECK_EQ(renderer.getName(), "interactive");
    CHECK_EQ(renderer.getSamplesPerPixel(), 1);
    CHECK_EQ(renderer.getBackgroundColor(), brayns::Vector4f(0.004f, 0.016f, 0.102f, 0.f));

    const auto &pm = brayns.getParametersManager();
    const auto &appParams = pm.getApplicationParameters();
    CHECK_EQ(appParams.getWindowSize(), brayns::Vector2ui(800, 600));
    CHECK_EQ(appParams.getJpegCompression(), 90);

    const auto &simulation = pm.getSimulationParameters();
    CHECK_EQ(simulation.getFrame(), 0);
}
