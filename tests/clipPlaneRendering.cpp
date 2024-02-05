/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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
#include <brayns/common/scene/ClipPlane.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/Scene.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "helpers/ImageValidator.h"

class Demo
{
public:
    Demo()
        : _argv{"clipPlanes",    "demo", "--disable-accumulation",
                "--window-size", "50",   "50"}
        , _brayns(_argv.size(), _argv.data())
    {
        instance = &_brayns;
    }

    static brayns::Brayns* instance;

private:
    std::vector<const char*> _argv;
    brayns::Brayns _brayns;
};
brayns::Brayns* Demo::instance = nullptr;

void testClipping(brayns::Brayns& brayns, bool orthographic = false)
{
    const std::string original =
        orthographic ? "demo_ortho.png" : "snapshot.png";

    const std::string clipped = orthographic ? "demo_clipped_ortho.png"
                                             : "demo_clipped_perspective.png";

    auto& engine = brayns.getEngine();
    auto& scene = engine.getScene();
    auto& camera = engine.getCamera();

    auto position = scene.getBounds().getCenter();
    position.z += glm::compMax(scene.getBounds().getSize());

    camera.setInitialState(position, glm::identity<brayns::Quaterniond>());

    if (orthographic)
        camera.setCurrentType("orthographic");
    else
        camera.setCurrentType("perspective");
    brayns.commitAndRender();
    CHECK(ImageValidator::validate(engine, original));

    auto id1 = scene.addClipPlane({1.0, 0.0, 0.0, -0.5});
    auto id2 = scene.addClipPlane({0.0, -1.0, 0.0, 0.5});
    brayns.commitAndRender();
    CHECK(ImageValidator::validate(engine, clipped));

    scene.removeClipPlane(id1);
    scene.removeClipPlane(id2);
    brayns.commitAndRender();
    CHECK(ImageValidator::validate(engine, original));

    id1 = scene.addClipPlane({1.0, 0.0, 0.0, -0.5});
    id2 = scene.addClipPlane({0.0, 1.0, 0.0, 0.5});
    scene.getClipPlane(id2)->setPlane({0.0, -1.0, 0.0, 0.5});
    brayns.commitAndRender();
    CHECK(ImageValidator::validate(engine, clipped));

    scene.removeClipPlane(id1);
    scene.removeClipPlane(id2);
}

TEST_CASE_FIXTURE(Demo, "perspective")
{
    testClipping(*Demo::instance);
}

TEST_CASE_FIXTURE(Demo, "orthographic")
{
    testClipping(*Demo::instance, true);
}
