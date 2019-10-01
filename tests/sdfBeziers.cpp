
/* Copyright (c) 2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: sebastien.speierer@epfl.ch
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

#include <brayns/common/geometry/SDFBezier.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "tests/doctest.h"

TEST_CASE("bezier_bounding_box")
{
    const brayns::SDFBezier bezier = {
        0,                   // userdata
        {-2.0f, 0.0f, 0.0f}, // p0
        {-2.0f, 0.0f, 0.0f}, // c0
        1.f,                 // r0
        {1.0f, 0.0f, 0.0f},  // p1
        {1.0f, 0.0f, 0.0f},  // c1
        1.0f                 // r1
    };

    const auto bbox = bezierBounds(bezier);

    CHECK_EQ(bbox.getMin(), brayns::Vector3d(-3.0, -1.0, -1.0));
    CHECK_EQ(bbox.getMax(), brayns::Vector3d(2.0, 1.0, 1.0));
}
