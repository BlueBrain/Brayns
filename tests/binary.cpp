/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <array>

#include <brayns/utils/binary/ByteConverter.h>
#include <brayns/utils/binary/ByteParser.h>
#include <brayns/utils/binary/ByteStream.h>
#include <brayns/utils/binary/Endian.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

TEST_CASE("byte_converter")
{
    uint32_t test = 1;
    CHECK_EQ(brayns::ByteConverter::swapBytes(test), 0x01000000);
}

TEST_CASE("byte_parser")
{
    constexpr int32_t integer = 1234;
    auto bytes = brayns::ByteConverter::getBytes(integer);
    auto data = std::string_view(bytes, sizeof(integer));
    CHECK_EQ(brayns::ByteParser<int32_t>::parse(bytes), integer);

    constexpr double number = 1.234;
    bytes = brayns::ByteConverter::getBytes(number);
    data = std::string_view(bytes, sizeof(number));
    CHECK_EQ(brayns::ByteParser<double>::parse(bytes), number);

    constexpr uint8_t byte = 123;
    bytes = brayns::ByteConverter::getBytes(byte);
    data = std::string_view(bytes, sizeof(byte));
    CHECK_EQ(brayns::ByteParser<uint8_t>::parse(bytes), byte);
}

TEST_CASE("byte_stream")
{
    std::array<double, 3> tests = {1.2, 2.0, 3.5};
    auto bytes = brayns::ByteConverter::getBytes(tests[0]);
    auto data = std::string_view(bytes, 3 * sizeof(double));
    auto stream = brayns::ByteStream(data);
    for (auto test : tests)
    {
        CHECK_EQ(stream.extract<double>(), test);
    }
    CHECK_FALSE(stream.canExtract(1));
}

TEST_CASE("endian")
{
    uint32_t test = 1;
    auto swapped = brayns::Endian::convertLittleEndianToLocalEndian(test);
    if (brayns::Endian::isBigEndian())
    {
        CHECK_EQ(swapped, brayns::ByteConverter::swapBytes(test));
    }
    if (brayns::Endian::isLittleEndian())
    {
        CHECK_EQ(swapped, test);
    }

    swapped = brayns::Endian::convertBigEndianToLocalEndian(test);
    if (brayns::Endian::isBigEndian())
    {
        CHECK_EQ(swapped, test);
    }
    if (brayns::Endian::isLittleEndian())
    {
        CHECK_EQ(swapped, brayns::ByteConverter::swapBytes(test));
    }
}
