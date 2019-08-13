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

#include <jsonSerialization.h>

#include <tests/ClientServer.h>
#include <tests/paths.h>

#include <BBP/TestDatasets.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "tests/doctest.h"

const std::string ADD_MODEL("add-model");

TEST_CASE("file_no_extension_blueconfig")
{
    ClientServer clientServer({"--plugin", "braynsCircuitViewer --density 1"});

    auto circuit = makeRequest<brayns::ModelParams, brayns::ModelDescriptor>(
        ADD_MODEL, {"circuit", BBP_TEST_BLUECONFIG});
    CHECK_EQ(circuit.getName(), "circuit");
    CHECK_EQ(circuit.getPath(), BBP_TEST_BLUECONFIG);
}
