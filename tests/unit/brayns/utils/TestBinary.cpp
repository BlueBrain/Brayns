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

#include <brayns/core/utils/binary/ByteConverter.h>

#include <doctest/doctest.h>

TEST_CASE("Raw bytes")
{
    const int32_t zero = 0;
    auto bytes = brayns::ByteConverter::getBytes(zero);
    CHECK_EQ(bytes, static_cast<const void *>(&zero));

    CHECK_EQ(brayns::ByteConverter::swapBytes(1), 0x01000000);
}

TEST_CASE("Endian conversion")
{
    int32_t test = 1;

    CHECK_EQ(brayns::ByteConverter::convertFromLocalEndian(test, std::endian::native), 1);
    CHECK_EQ(brayns::ByteConverter::convertToLocalEndian(test, std::endian::native), 1);

    if constexpr (std::endian::native == std::endian::big)
    {
        CHECK_EQ(brayns::ByteConverter::convertFromLocalEndian(test, std::endian::big), 1);
        CHECK_EQ(brayns::ByteConverter::convertFromLocalEndian(test, std::endian::little), 0x01000000);
        CHECK_EQ(brayns::ByteConverter::convertToLocalEndian(test, std::endian::big), 1);
        CHECK_EQ(brayns::ByteConverter::convertToLocalEndian(test, std::endian::little), 0x01000000);
    }

    if constexpr (std::endian::native == std::endian::little)
    {
        CHECK_EQ(brayns::ByteConverter::convertFromLocalEndian(test, std::endian::little), 1);
        CHECK_EQ(brayns::ByteConverter::convertFromLocalEndian(test, std::endian::big), 0x01000000);
        CHECK_EQ(brayns::ByteConverter::convertToLocalEndian(test, std::endian::little), 1);
        CHECK_EQ(brayns::ByteConverter::convertToLocalEndian(test, std::endian::big), 0x01000000);
    }
}

TEST_CASE("Byte parser")
{
    std::string_view data;
    auto endian = std::endian::native;

    const uint32_t refi32 = 123;
    auto bytes = brayns::ByteConverter::getBytes(refi32);
    data = {bytes, sizeof(refi32)};
    auto i32 = brayns::ByteConverter::convertFromBytes<uint32_t>(data, endian);
    CHECK_EQ(i32, refi32);

    const double refd = 1.23;
    bytes = brayns::ByteConverter::getBytes(refd);
    data = {bytes, sizeof(refd)};
    auto d = brayns::ByteConverter::convertFromBytes<double>(data, endian);
    CHECK_EQ(d, refd);
}
