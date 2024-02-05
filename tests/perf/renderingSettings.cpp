/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <brayns/Brayns.h>

#include <brayns/common/Timer.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>
#include <brayns/parameters/ParametersManager.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

TEST_CASE("default_scene_benchmark")
{
    const char* argv[] = {"brayns"};
    brayns::Brayns brayns(1, argv);

    uint64_t reference, shadows, softShadows, ambientOcclusion, allOptions;

    // Set default rendering parameters
    brayns::ParametersManager& params = brayns.getParametersManager();
    params.getRenderingParameters().setSamplesPerPixel(32);
    brayns.commit();

    // Start timer
    brayns::Timer timer;
    timer.start();
    brayns.render();
    timer.stop();
    reference = timer.milliseconds();

    auto& renderer = brayns.getEngine().getRenderer();

    // Shadows
    auto props = renderer.getPropertyMap();
    props.update("shadows", 1.);
    renderer.updateProperties(props);
    brayns.commit();

    timer.start();
    brayns.render();
    timer.stop();
    shadows = timer.milliseconds();

    // Shadows
    float t = float(shadows) / float(reference);
    CHECK_MESSAGE(t < 1.65f, "Shadows cost. expected: 165%");

    props.update("softShadows", 1.);
    renderer.updateProperties(props);
    brayns.commit();

    timer.start();
    brayns.render();
    timer.stop();
    softShadows = timer.milliseconds();

    // Soft shadows
    t = float(softShadows) / float(reference);
    CHECK_MESSAGE(t < 1.85f, "Soft shadows cost. expected: 185%");

    // Ambient occlustion
    props.update("shadows", 0.);
    props.update("softShadows", 0.);
    props.update("aoWeight", 1.);
    renderer.updateProperties(props);
    brayns.commit();

    timer.start();
    brayns.render();
    timer.stop();
    ambientOcclusion = timer.milliseconds();

    // Ambient occlusion
    t = float(ambientOcclusion) / float(reference);
    CHECK_MESSAGE(t < 2.5f, "Ambient occlusion cost. expected: 250%");

    // All options
    props.update("shadows", 1.);
    props.update("softShadows", 1.);
    props.update("aoWeight", 1.);
    renderer.updateProperties(props);
    brayns.commit();

    timer.start();
    brayns.render();
    timer.stop();
    allOptions = timer.milliseconds();

    // All options
    t = float(allOptions) / float(reference);
    CHECK_MESSAGE(t < 3.5f, "All options cost. expected: 350%");
}
