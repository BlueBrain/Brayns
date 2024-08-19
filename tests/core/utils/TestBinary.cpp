/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <brayns/core/utils/Binary.h>

#include <doctest.h>

using namespace brayns;

TEST_CASE("As bytes")
{
    auto test = std::uint32_t(1);

    auto bytes = asBytes(test);

    CHECK_EQ(bytes.size(), 4);

    if (std::endian::native == std::endian::big)
    {
        CHECK_EQ(bytes[0], '\0');
        CHECK_EQ(bytes[3], '\1');
    }
    else
    {
        CHECK_EQ(bytes[0], '\1');
        CHECK_EQ(bytes[3], '\0');
    }

    CHECK_EQ(bytes[1], '\0');
    CHECK_EQ(bytes[2], '\0');

    swapBytes(test);

    CHECK_EQ(test, 16777216);

    swapBytes(test);

    CHECK_EQ(test, 1);
}

TEST_CASE("Extract")
{
    const auto test = std::int64_t(1);
    auto bytes = asBytes(test);

    CHECK_EQ(extractBytesAs<std::int64_t>(bytes, std::endian::native), test);
    CHECK(bytes.empty());

    const auto vector = Vector3(1, 2, 3);
    bytes = asBytes(vector);

    CHECK_EQ(extractBytesAs<Vector3>(bytes, std::endian::native), vector);
    CHECK(bytes.empty());

    const auto quaternion = Quaternion(0, 1, 2, 3);
    bytes = asBytes(quaternion);

    CHECK_EQ(extractBytesAs<Quaternion>(bytes, std::endian::native), quaternion);
    CHECK(bytes.empty());
}

TEST_CASE("Compose")
{
    for (auto endian : {std::endian::little, std::endian::big})
    {
        auto buffer = std::string();
        composeBytesTo(std::uint32_t(1), endian, buffer);
        composeBytesTo(2.0F, endian, buffer);
        composeBytesTo(3.0, endian, buffer);

        auto data = std::string_view(buffer);

        CHECK_EQ(extractBytesAs<std::uint32_t>(data, endian), 1);
        CHECK_EQ(extractBytesAs<float>(data, endian), 2.0F);
        CHECK_EQ(extractBytesAs<double>(data, endian), 3.0);

        CHECK(data.empty());
    }
}
