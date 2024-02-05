/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: jonas.karlsson@epfl.ch
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

#include <brayns/common/geometry/SDFGeometry.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

TEST_CASE("bounding_box")
{
    const auto sphere = brayns::createSDFSphere({1.0f, 1.0f, 1.0f}, 1.0f);
    const auto conePill =
        brayns::createSDFConePill({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 2.0f,
                                  1.0f);
    const auto pill =
        brayns::createSDFPill({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 2.0f);

    const auto boxSphere = getSDFBoundingBox(sphere);
    const auto boxConePill = getSDFBoundingBox(conePill);
    const auto boxPill = getSDFBoundingBox(pill);

    CHECK_EQ(boxSphere.getMin(), brayns::Vector3d(0.0, 0.0, 0.0));
    CHECK_EQ(boxSphere.getMax(), brayns::Vector3d(2.0, 2.0, 2.0));

    CHECK_EQ(boxConePill.getMin(), brayns::Vector3d(-2.0, -2.0, -2.0));
    CHECK_EQ(boxConePill.getMax(), brayns::Vector3d(2.0, 2.0f, 2.0));

    CHECK_EQ(boxPill.getMin(), brayns::Vector3d(-2.0, -2.0, -2.0));
    CHECK_EQ(boxPill.getMax(), brayns::Vector3d(3.0, 3.0, 3.0));
}
