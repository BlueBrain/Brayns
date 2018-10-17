/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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
#include <tests/paths.h>

#include <brayns/common/camera/Camera.h>
#include <brayns/common/engine/Engine.h>
#include <brayns/common/light/DirectionalLight.h>
#include <brayns/common/material/Material.h>
#include <brayns/common/renderer/FrameBuffer.h>
#include <brayns/common/renderer/Renderer.h>
#include <brayns/common/scene/Model.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/types.h>

#define BOOST_TEST_MODULE materials
#include <boost/test/unit_test.hpp>

#include "PDiffHelpers.h"

void createScene(const brayns::MaterialShadingMode shadingMode,
                 const std::string& referenceFilename)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();
    const char* app = testSuite.argv[0];
    std::vector<const char*> argv = {{app, "--disable-accumulation",
                                      "--window-size", "512", "512",
                                      "--renderer", "advanced_simulation"}};

    brayns::Brayns brayns(argv.size(), argv.data());
    auto& scene = brayns.getEngine().getScene();

    auto model = scene.createModel();

    const brayns::Vector3f WHITE = {1.f, 1.f, 1.f};

    constexpr size_t materialId = 0;
    auto materialRed = model->createMaterial(materialId, "white");
    materialRed->setDiffuseColor(WHITE);
    materialRed->setSpecularColor(WHITE);
    materialRed->setSpecularExponent(10.f);
    materialRed->setShadingMode(shadingMode);
    model->addCylinder(materialId, {{0.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, 0.1f});
    auto modelDesc =
        std::make_shared<brayns::ModelDescriptor>(std::move(model), "Cylinder");
    scene.addModel(modelDesc);

    scene.clearLights();

    brayns::DirectionalLightPtr sunLight(
        new brayns::DirectionalLight(brayns::Vector3f(0.f, 0.f, 1.f), WHITE,
                                     1.0f));

    scene.addLight(sunLight);
    scene.commitLights();

    auto& camera = brayns.getEngine().getCamera();

    camera.setPosition(brayns::Vector3f(0.f, 0.5f, -1.f));
    camera.setTarget(brayns::Vector3f(0.f, 0.5f, 0.f));
    camera.setUp(brayns::Vector3f(0.f, 1.f, 0.f));

    brayns.commitAndRender();
    BOOST_CHECK(compareTestImage(referenceFilename,
                                 brayns.getEngine().getFrameBuffer()));
}

BOOST_AUTO_TEST_CASE(no_shading)
{
    createScene(brayns::MaterialShadingMode::none, "materialNone.png");
}

BOOST_AUTO_TEST_CASE(diffuse_shading)
{
    createScene(brayns::MaterialShadingMode::diffuse, "materialDiffuse.png");
}

BOOST_AUTO_TEST_CASE(electron_shading)
{
    createScene(brayns::MaterialShadingMode::electron, "materialElectron.png");
}
