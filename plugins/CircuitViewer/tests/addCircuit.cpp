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

#define BOOST_TEST_MODULE circuitViewerAddCircuit

#include <jsonSerialization.h>

#include <tests/ClientServer.h>
#include <tests/paths.h>

#include <BBP/TestDatasets.h>

#include <boost/test/unit_test.hpp>

const std::string ADD_MODEL("add-model");

BOOST_GLOBAL_FIXTURE(ClientServer);

BOOST_AUTO_TEST_CASE(file_no_extension_blueconfig)
{
    auto circuit = makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(
        ADD_MODEL, {"circuit", BBP_TEST_BLUECONFIG});
    BOOST_CHECK_EQUAL(circuit.getName(), "circuit");
    BOOST_CHECK_EQUAL(circuit.getPath(), BBP_TEST_BLUECONFIG);
}
