
/* Copyright (c) 2018, EPFL/Blue Brain Project
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

#define BOOST_TEST_MODULE sdfGeometries
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(bounding_box)
{
    const auto sphere = brayns::createSDFSphere({1.0f, 1.0f, 1.0f}, 1.0f);
    const auto conePill =
        brayns::createSDFConePill({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 2.0f,
                                  1.0f);
    const auto pill =
        brayns::createSDFPill({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 2.0f);

    const brayns::Boxf boxSphere = getSDFBoundingBox(sphere);
    const brayns::Boxf boxConePill = getSDFBoundingBox(conePill);
    const brayns::Boxf boxPill = getSDFBoundingBox(pill);

    BOOST_CHECK_EQUAL(boxSphere.getMin(), brayns::Vector3f(0.0f, 0.0f, 0.0f));
    BOOST_CHECK_EQUAL(boxSphere.getMax(), brayns::Vector3f(2.0f, 2.0f, 2.0f));

    BOOST_CHECK_EQUAL(boxConePill.getMin(),
                      brayns::Vector3f(-2.0f, -2.0f, -2.0f));
    BOOST_CHECK_EQUAL(boxConePill.getMax(), brayns::Vector3f(2.0f, 2.0f, 2.0f));

    BOOST_CHECK_EQUAL(boxPill.getMin(), brayns::Vector3f(-2.0f, -2.0f, -2.0f));
    BOOST_CHECK_EQUAL(boxPill.getMax(), brayns::Vector3f(3.0f, 3.0f, 3.0f));
}
