/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <doctest/doctest.h>

#include <brayns/engine/volume/Volume.h>
#include <brayns/engine/volume/types/RegularVolume.h>

#include <tests/unit/PlaceholderEngine.h>

TEST_CASE("Volume")
{
    BRAYNS_TESTS_PLACEHOLDER_ENGINE

    SUBCASE("Casting")
    {
        auto volume = brayns::Volume(brayns::RegularVolume());
        CHECK(volume.as<brayns::RegularVolume>());
        CHECK(!volume.as<int>());
    }
    SUBCASE("Iteration")
    {
        auto grid = brayns::RegularVolume();
        grid.dataType = brayns::VolumeDataType::UnsignedChar;
        grid.perVertexData = true;
        grid.size = brayns::Vector3ui(5);
        grid.spacing = brayns::Vector3f(1.f);
        grid.voxels = std::vector<uint8_t>(glm::compMul(grid.size), 0);

        auto volume = brayns::Volume(std::move(grid));
        volume.commit();

        CHECK(!volume.commit());
        volume.manipulate([&](brayns::RegularVolume &data) { (void)data; });
        CHECK(volume.commit());
    }
    SUBCASE("Compute bounds")
    {
        auto grid = brayns::RegularVolume();
        grid.dataType = brayns::VolumeDataType::UnsignedChar;
        grid.perVertexData = true;
        grid.size = brayns::Vector3ui(5);
        grid.spacing = brayns::Vector3f(1.f);
        grid.voxels = std::vector<uint8_t>(glm::compMul(grid.size), 0);

        auto volume = brayns::Volume(std::move(grid));

        auto bounds = volume.computeBounds(brayns::Matrix4f(1.f));
        auto min = bounds.getMin();
        auto max = bounds.getMax();
        CHECK(min == brayns::Vector3f(0.f));
        CHECK(max == brayns::Vector3f(5.f));

        auto transform = glm::translate(brayns::Vector3f(100.f, 0.f, 0.f));
        bounds = volume.computeBounds(transform);
        min = bounds.getMin();
        max = bounds.getMax();
        CHECK(min == brayns::Vector3f(100.f, 0.f, 0.f));
        CHECK(max == brayns::Vector3f(105.f, 5.f, 5.f));
    }
}
