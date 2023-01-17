/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <brayns/utils/binary/ByteConverter.h>
#include <brayns/utils/binary/ByteOrder.h>
#include <brayns/utils/binary/ByteParser.h>

#include <doctest/doctest.h>

TEST_CASE("Byte converter")
{
    const int32_t zero = 0;
    auto bytes = brayns::ByteConverter::getBytes(zero);
    CHECK_EQ(bytes, static_cast<const void *>(&zero));
    for (size_t i = 0; i < sizeof(zero); ++i)
    {
        CHECK_EQ(bytes[i], 0);
    }

    int32_t test = 1;
    int32_t copy = ~0;

    brayns::ByteConverter::copyBytes(test, copy);
    CHECK_EQ(test, copy);

    brayns::ByteConverter::swapBytes(test);
    CHECK_EQ(test, 0x01000000);
}

TEST_CASE("Byte order")
{
    int32_t test = 1;
    auto bytes = brayns::ByteConverter::getBytes(test);

    brayns::ByteOrderHelper::convertFromSystemByteOrder(test, brayns::ByteOrderHelper::getSystemByteOrder());
    CHECK_EQ(test, 1);

    test = 1;
    brayns::ByteOrderHelper::convertFromSystemByteOrder(test, brayns::ByteOrder::BigEndian);
    CHECK_EQ(bytes[3], 1);

    test = 1;
    brayns::ByteOrderHelper::convertFromSystemByteOrder(test, brayns::ByteOrder::LittleEndian);
    CHECK_EQ(bytes[0], 1);

    test = 0;
    bytes[3] = 1;
    brayns::ByteOrderHelper::convertToSystemByteOrder(test, brayns::ByteOrder::BigEndian);
    CHECK_EQ(test, 1);

    test = 0;
    bytes[0] = 1;
    brayns::ByteOrderHelper::convertToSystemByteOrder(test, brayns::ByteOrder::LittleEndian);
    CHECK_EQ(test, 1);
}

TEST_CASE("Byte parser")
{
    std::string_view data;
    auto order = brayns::ByteOrderHelper::getSystemByteOrder();

    uint32_t i32 = 0;
    const uint32_t refi32 = 123;
    auto bytes = brayns::ByteConverter::getBytes(refi32);
    data = {bytes, 4};
    brayns::ByteParser<uint32_t>::parse(data, i32, order);
    CHECK_EQ(i32, refi32);

    double d = 0.0;
    const double refd = 1.23;
    bytes = brayns::ByteConverter::getBytes(refd);
    data = {bytes, 8};
    brayns::ByteParser<double>::parse(data, d, order);
    CHECK_EQ(d, refd);
}
