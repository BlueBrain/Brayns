/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jonas Karlsson <jonas.karlsson@epfl.ch>
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

#define BOOST_TEST_MODULE transferFunctionLoader

#include <brayns/common/transferFunction/TransferFunction.h>
#include <brayns/io/TransferFunctionLoader.h>

#include <boost/test/unit_test.hpp>

#include <brayns/common/types.h>
#include <tests/paths.h>

BOOST_AUTO_TEST_CASE(load_transfer_function)
{
    brayns::TransferFunction tf;

    const auto fullPath = std::string(BRAYNS_TESTDATA) + "files/rat0.1dt";
    const auto v = brayns::Vector2f(-100.0f, 100.0f);

    BOOST_CHECK(brayns::loadTransferFunctionFromFile(fullPath, v, tf));
    BOOST_CHECK_EQUAL(tf.getDiffuseColors().size(), 16);
    BOOST_CHECK_EQUAL(tf.getValuesRange()[0], -100.0f);
    BOOST_CHECK_EQUAL(tf.getValuesRange()[1], 100.0f);
}
