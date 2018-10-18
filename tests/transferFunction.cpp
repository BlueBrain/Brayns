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

#define BOOST_TEST_MODULE transferFunction
#include <boost/test/unit_test.hpp>

#include "ClientServer.h"
#include "PDiffHelpers.h"

BOOST_GLOBAL_FIXTURE(ClientServer);

const std::string SET_TF("set-model-transfer-function");
const std::string GET_TF("get-model-transfer-function");

BOOST_AUTO_TEST_CASE(set_transfer_function)
{
    brayns::TransferFunction tf;
    tf.setValuesRange({0, 10});
    tf.setColorMap({"test", {{1, 1, 1}, {0, 1, 0}, {0.1, 1, 0.5}}});
    tf.setControlPoints({{0, 0.1}, {0.2, 1.}, {1., 0.8}});

    makeNotification<brayns::ModelTransferFunction>(SET_TF, {0, tf});

    const auto& newTF =
        getScene().getModel(0)->getModel().getTransferFunction();
    BOOST_CHECK(tf.getColorMap() == newTF.getColorMap());
    BOOST_CHECK(tf.getControlPoints() == newTF.getControlPoints());
    BOOST_CHECK_EQUAL(tf.getValuesRange(), newTF.getValuesRange());
}

BOOST_AUTO_TEST_CASE(set_transfer_function_invalid_model)
{
    brayns::TransferFunction tf;

    BOOST_CHECK_THROW(
        (makeRequest<brayns::ModelTransferFunction, bool>(SET_TF, {42, tf})),
        std::runtime_error);
}

BOOST_AUTO_TEST_CASE(get_transfer_function)
{
    const auto& tf = getScene().getModel(0)->getModel().getTransferFunction();

    const auto rpcTF =
        makeRequest<brayns::ObjectID, brayns::TransferFunction>(GET_TF, {0});

    BOOST_CHECK(tf.getColorMap() == rpcTF.getColorMap());
    BOOST_CHECK(tf.getControlPoints() == rpcTF.getControlPoints());
    BOOST_CHECK_EQUAL(tf.getValuesRange(), rpcTF.getValuesRange());
}

BOOST_AUTO_TEST_CASE(get_transfer_function_invalid_model)
{
    BOOST_CHECK_THROW(
        (makeRequest<brayns::ObjectID, brayns::TransferFunction>(GET_TF, {42})),
        std::runtime_error);
}

#ifdef BRAYNS_USE_OSPRAY
BOOST_AUTO_TEST_CASE(validate_opacity_interpolation)
{
    auto& model = getScene().getModel(0)->getModel();
    auto& tf = model.getTransferFunction();
    tf.clear();

    commitAndRender();

    auto& ospModel = static_cast<brayns::OSPRayModel&>(model);
    auto ospTF =
        reinterpret_cast<ospray::ManagedObject*>(ospModel.transferFunction());
    auto colors = ospTF->getParamData("colors", nullptr);
    BOOST_REQUIRE_EQUAL(colors->size(), 2);
    BOOST_CHECK_EQUAL(brayns::Vector3f(((float*)colors->data)[0]),
                      brayns::Vector3f(tf.getColors()[0]));
    BOOST_CHECK_EQUAL(brayns::Vector3f(((float*)colors->data)[3]),
                      brayns::Vector3f(tf.getColors()[1]));

    auto opacities = ospTF->getParamData("opacities", nullptr);
    BOOST_REQUIRE_EQUAL(opacities->size(), 256);
    for (size_t i = 0; i < 256; ++i)
        BOOST_CHECK_EQUAL(((float*)opacities->data)[i], i / 255.f);
}
#endif
