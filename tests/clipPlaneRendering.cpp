/* Copyright (c) 2018, EPFL/Blue Brain Project
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

#define BOOST_TEST_MODULE braynsClipPlaneRendering

#include "PDiffHelpers.h"

#include <brayns/Brayns.h>
#include <brayns/common/camera/Camera.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/scene/ClipPlane.h>
#include <brayns/common/scene/Scene.h>

#include <boost/test/unit_test.hpp>

class Demo
{
public:
    Demo()
        : _argv{"clipPlanes", "demo", "--disable-accumulation",
            "--window-size", "50",  "50"}
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

BOOST_GLOBAL_FIXTURE(Demo);

void testClipping(brayns::Brayns& brayns, bool orthographic = false)
{
    const std::string original =
        orthographic ? "demo_ortho.png" : "snapshot.png";

    const std::string clipped = orthographic ? "demo_clipped_ortho.png"
                                             : "demo_clipped_perspective.png";

    auto& engine = brayns.getEngine();
    auto& scene = engine.getScene();
    auto& camera = engine.getCamera();

    camera.setInitialState(scene.getBounds());
    if (orthographic)
        camera.setCurrentType("orthographic");
    else
        camera.setCurrentType("perspective");
    brayns.commitAndRender();
    BOOST_CHECK(compareTestImage(original, engine.getFrameBuffer()));

    auto id1 = scene.addClipPlane({{1.0, 0.0, 0.0, -0.5}});
    auto id2 = scene.addClipPlane({{0.0, -1.0, 0.0, 0.5}});
    brayns.commitAndRender();
    BOOST_CHECK(compareTestImage(clipped, engine.getFrameBuffer()));

    scene.removeClipPlane(id1);
    scene.removeClipPlane(id2);
    brayns.commitAndRender();
    BOOST_CHECK(compareTestImage(original, engine.getFrameBuffer()));

    id1 = scene.addClipPlane({{1.0, 0.0, 0.0, -0.5}});
    id2 = scene.addClipPlane({{0.0, 1.0, 0.0, 0.5}});
    scene.getClipPlane(id2)->setPlane({{0.0, -1.0, 0.0, 0.5}});
    brayns.commitAndRender();
    BOOST_CHECK(compareTestImage(clipped, engine.getFrameBuffer()));

    scene.removeClipPlane(id1);
    scene.removeClipPlane(id2);
}

BOOST_AUTO_TEST_CASE(perspective)
{
    testClipping(*Demo::instance);
}

BOOST_AUTO_TEST_CASE(orthographic)
{
    testClipping(*Demo::instance, true);
}

