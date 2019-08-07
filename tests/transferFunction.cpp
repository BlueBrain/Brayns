/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
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

#ifdef BRAYNS_USE_OSPRAY
#include <engines/ospray/OSPRayModel.h>
#include <ospray/SDK/common/Data.h>
#include <ospray/SDK/common/Managed.h>
#endif

#include "ClientServer.h"
#include "PDiffHelpers.h"

const std::string SET_TF("set-model-transfer-function");
const std::string GET_TF("get-model-transfer-function");

TEST_CASE_FIXTURE(ClientServer, "set_transfer_function")
{
    brayns::TransferFunction tf;
    tf.setValuesRange({0, 10});
    tf.setColorMap({"test", {{1, 1, 1}, {0, 1, 0}, {0.1, 1, 0.5}}});
    tf.setControlPoints({{0, 0.1}, {0.2, 1.}, {1., 0.8}});

    makeNotification<brayns::ModelTransferFunction>(SET_TF, {0, tf});

    const auto& newTF =
        getScene().getModel(0)->getModel().getTransferFunction();
    CHECK(tf.getColorMap() == newTF.getColorMap());
    CHECK(tf.getControlPoints() == newTF.getControlPoints());
    CHECK_EQ(tf.getValuesRange(), newTF.getValuesRange());
}

TEST_CASE_FIXTURE(ClientServer, "set_transfer_function_invalid_model")
{
    brayns::TransferFunction tf;

    CHECK_THROWS_AS(
        (makeRequest<brayns::ModelTransferFunction, bool>(SET_TF, {42, tf})),
        std::runtime_error);
}

TEST_CASE_FIXTURE(ClientServer, "get_transfer_function")
{
    const auto& tf = getScene().getModel(0)->getModel().getTransferFunction();

    const auto rpcTF =
        makeRequest<brayns::ObjectID, brayns::TransferFunction>(GET_TF, {0});

    CHECK(tf.getColorMap() == rpcTF.getColorMap());
    CHECK(tf.getControlPoints() == rpcTF.getControlPoints());
    CHECK_EQ(tf.getValuesRange(), rpcTF.getValuesRange());
}

TEST_CASE_FIXTURE(ClientServer, "get_transfer_function_invalid_model")
{
    CHECK_THROWS_AS(
        (makeRequest<brayns::ObjectID, brayns::TransferFunction>(GET_TF, {42})),
        std::runtime_error);
}

#ifdef BRAYNS_USE_OSPRAY
TEST_CASE_FIXTURE(ClientServer, "validate_opacity_interpolation")
{
    auto& model = getScene().getModel(0)->getModel();
    auto& tf = model.getTransferFunction();
    tf.clear();

    commitAndRender();

    auto& ospModel = static_cast<brayns::OSPRayModel&>(model);
    auto ospTF =
        reinterpret_cast<ospray::ManagedObject*>(ospModel.transferFunction());
    auto colors = ospTF->getParamData("colors", nullptr);
    REQUIRE_EQ(colors->size(), 2);
    CHECK_EQ(brayns::Vector3f(((float*)colors->data)[0]),
             brayns::Vector3f(tf.getColors()[0]));
    CHECK_EQ(brayns::Vector3f(((float*)colors->data)[3]),
             brayns::Vector3f(tf.getColors()[1]));

    auto opacities = ospTF->getParamData("opacities", nullptr);
    REQUIRE_EQ(opacities->size(), 256);
    for (size_t i = 0; i < 256; ++i)
        CHECK_EQ(((float*)opacities->data)[i], i / 255.f);
}
#endif
