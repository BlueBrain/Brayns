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

#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

TEST_CASE("subsampling_buffer_size")
{
    const char* argv[] = {"subsampling", "--window-size", "400",
                          "200",         "--subsampling", "4",
                          "demo"};
    const int argc = sizeof(argv) / sizeof(char*);
    brayns::Brayns brayns(argc, argv);

    brayns.commitAndRender();
    CHECK_EQ(brayns.getEngine().getFrameBuffer().getSize(),
             brayns::Vector2ui(100, 50));

    brayns.commitAndRender();
    CHECK_EQ(brayns.getEngine().getFrameBuffer().getSize(),
             brayns::Vector2ui(400, 200));
}

TEST_CASE("no_subsampling_needed")
{
    const char* argv[] = {"subsampling", "--window-size",       "400",
                          "200",         "--samples-per-pixel", "2",
                          "demo"};
    const int argc = sizeof(argv) / sizeof(char*);
    brayns::Brayns brayns(argc, argv);

    brayns.commitAndRender();
    CHECK_EQ(brayns.getEngine().getFrameBuffer().getSize(),
             brayns::Vector2ui(400, 200));

    brayns.commitAndRender();
    CHECK_EQ(brayns.getEngine().getFrameBuffer().getSize(),
             brayns::Vector2ui(400, 200));
}
