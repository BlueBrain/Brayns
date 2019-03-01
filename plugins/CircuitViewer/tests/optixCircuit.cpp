/* Copyright (c) 2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include "CircuitViewer/tests/paths.h"
#include "tests/PDiffHelpers.h"
#include <jsonSerialization.h>

#include <brayns/Brayns.h>

#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>

#include <brayns/parameters/ParametersManager.h>

#include <BBP/TestDatasets.h>

#define BOOST_TEST_MODULE optixCircuit
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(circuit_with_simulation_mapping_optix)
{
    std::vector<const char*> argv = {
        "brayns", BBP_TEST_BLUECONFIG3, "--animation-frame", "50", "--engine",
        "optix", "--samples-per-pixel", "16", "--window-size", "200", "1000",
        "--plugin",
        "braynsCircuitViewer --targets allmini50 --report "
        "voltages --display-mode soma --synchronous-mode"};

    brayns::Brayns brayns(argv.size(), argv.data());

    auto modelDesc = brayns.getEngine().getScene().getModel(0);
    modelDesc->getModel().getTransferFunction().setValuesRange({-66, -62});
    brayns.commit();
    modelDesc->getModel().getSimulationHandler()->waitReady();
    brayns.commitAndRender();

    BOOST_CHECK(compareTestImage("testdataallmini50basicsimulation_optix.png",
                                 brayns.getEngine().getFrameBuffer()));
}
