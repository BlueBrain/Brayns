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

#include <brayns/core/utils/parsing/ChunkExtractor.h>
#include <brayns/core/utils/parsing/FileStream.h>
#include <brayns/core/utils/parsing/Parser.h>
#include <brayns/core/utils/parsing/TokenExtractor.h>

#include "TestCase.h"

TEST_CASE("Chunk extractor")
{
    std::string_view data;
    auto endian = std::endian::native;

    data = "\1\2\3\4";
    uint32_t ui = 0;
    brayns::ChunkExtractor<uint32_t>::extract(data, ui, std::endian::big);
    CHECK_EQ(ui, 0x01020304);

    const float ref = 1.23f;
    data = {brayns::ByteConverter::getBytes(ref), sizeof(ref)};
    float f = 0.0f;
    brayns::ChunkExtractor<float>::extract(data, f, endian);
    CHECK_EQ(f, ref);

    std::vector<int> refs = {1, 2, 3};
    data = {brayns::ByteConverter::getBytes(refs[0]), 3 * sizeof(int)};
    std::vector<int> is;
    brayns::ChunkExtractor<std::vector<int>>::extract(data, is, endian);
    CHECK_EQ(is, refs);

    brayns::Vector2f refv = {1.2f, 2.3f};
    data = {brayns::ByteConverter::getBytes(refv[0]), 2 * sizeof(float)};
    brayns::Vector2f v;
    brayns::ChunkExtractor<brayns::Vector2f>::extract(data, v, endian);
    CHECK_EQ(v, refv);
}

TEST_CASE("File stream")
{
    auto test = "test1\ntest2\n";
    auto stream = brayns::FileStream(test);

    CHECK_EQ(stream.getLine(), "");

    CHECK(stream.nextLine());
    CHECK_EQ(stream.getLine(), "test1");

    CHECK(stream.nextLine());
    CHECK_EQ(stream.getLine(), "test2");

    CHECK(stream.nextLine());
    CHECK_EQ(stream.getLine(), "");

    CHECK_FALSE(stream.nextLine());
    CHECK_EQ(stream.getLine(), "");
}

TEST_CASE("Parser")
{
    std::string_view data;

    data = "123";
    CHECK_EQ(brayns::Parser::parseString<int64_t>(data), 123);

    data = "agss";
    CHECK_THROWS_AS(brayns::Parser::parseString<int64_t>(data), std::invalid_argument);

    data = "1.22";
    CHECK_EQ(brayns::Parser::parseString<double>(data), 1.22);
    CHECK_THROWS_AS(brayns::Parser::parseString<int32_t>(data), std::invalid_argument);

    data = "1234";
    CHECK_THROWS_AS(brayns::Parser::parseString<int8_t>(data), std::out_of_range);

    int32_t test = 123;
    auto bytes = brayns::ByteConverter::getBytes(test);
    auto endian = std::endian::native;
    data = {bytes, sizeof(test)};
    CHECK_EQ(brayns::Parser::parseBytes<int32_t>(data, endian), test);

    data = "1 2 3";
    auto ref = brayns::Vector3f(1, 2, 3);
    CHECK_EQ(brayns::Parser::extractToken<brayns::Vector3f>(data), ref);

    bytes = brayns::ByteConverter::getBytes(ref);
    data = {bytes, sizeof(ref)};
    CHECK_EQ(brayns::Parser::extractChunk<brayns::Vector3f>(data, endian), ref);
}

TEST_CASE("Token extractor")
{
    std::string_view data;

    data = " 1234 test";
    uint32_t ui = 0;
    brayns::TokenExtractor<uint32_t>::extract(data, ui);
    CHECK_EQ(ui, 1234);
    CHECK_EQ(data, " test");

    data = "\n12.34  \n test";
    float f = 0.0f;
    brayns::TokenExtractor<float>::extract(data, f);
    CHECK_EQ(f, 12.34f);
    CHECK_EQ(data, "  \n test");

    data = "1 2 3";
    std::vector<int> is;
    brayns::TokenExtractor<std::vector<int>>::extract(data, is);
    CHECK_EQ(is, std::vector<int>({1, 2, 3}));
    CHECK_EQ(data, "");

    data = "1.2 3.4";
    brayns::Vector2f v;
    brayns::TokenExtractor<brayns::Vector2f>::extract(data, v);
    CHECK_EQ(v, brayns::Vector2f(1.2f, 3.4f));
    CHECK_EQ(data, "");
}
