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

#include <brayns/Brayns.h>

#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>

#define BOOST_TEST_MODULE braynsSubsampling
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(subsampling_buffer_size)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();
    const char* app = testSuite.argv[0];
    const char* argv[] = {app, "--window-size", "400", "200", "--subsampling",
                          "4", "demo"};
    const int argc = sizeof(argv) / sizeof(char*);
    brayns::Brayns brayns(argc, argv);

    brayns.commitAndRender();
    BOOST_CHECK_EQUAL(brayns.getEngine().getFrameBuffer().getSize(),
                      brayns::Vector2ui(100, 50));

    brayns.commitAndRender();
    BOOST_CHECK_EQUAL(brayns.getEngine().getFrameBuffer().getSize(),
                      brayns::Vector2ui(400, 200));
}

BOOST_AUTO_TEST_CASE(no_subsampling_needed)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();
    const char* app = testSuite.argv[0];
    const char* argv[] = {app,   "--window-size",       "400",
                          "200", "--samples-per-pixel", "2",
                          "demo"};
    const int argc = sizeof(argv) / sizeof(char*);
    brayns::Brayns brayns(argc, argv);

    brayns.commitAndRender();
    BOOST_CHECK_EQUAL(brayns.getEngine().getFrameBuffer().getSize(),
                      brayns::Vector2ui(400, 200));

    brayns.commitAndRender();
    BOOST_CHECK_EQUAL(brayns.getEngine().getFrameBuffer().getSize(),
                      brayns::Vector2ui(400, 200));
}
