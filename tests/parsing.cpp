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

#include <brayns/common/GlmParsers.h>

#include <brayns/utils/parsing/ByteConverter.h>
#include <brayns/utils/parsing/Endian.h>
#include <brayns/utils/parsing/FileStream.h>
#include <brayns/utils/parsing/Parse.h>
#include <brayns/utils/parsing/StringConverter.h>
#include <brayns/utils/parsing/StringCounter.h>
#include <brayns/utils/parsing/StringInfo.h>
#include <brayns/utils/parsing/StringJoiner.h>
#include <brayns/utils/parsing/StringStream.h>
#include <brayns/utils/parsing/StringTrimmer.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

TEST_CASE("byte_converter")
{
    int32_t test = 1;
    int32_t copy = ~0;

    brayns::ByteConverter::copyBytes(test, copy);
    CHECK_EQ(test, copy);

    brayns::ByteConverter::swapBytes(test);
    CHECK_EQ(test, 0x01000000);
}

TEST_CASE("endian")
{
    int32_t test = 1;

    brayns::EndianConverter::convertToLocalEndian(test, brayns::Endian::Local);
    CHECK_EQ(test, 1);

    brayns::EndianConverter::convertFromLocalEndian(test, brayns::Endian::Local);
    CHECK_EQ(test, 1);

    brayns::EndianConverter::convertFromLocalEndian(test, ~brayns::Endian::Local);
    CHECK_EQ(test, 0x01000000);
}

TEST_CASE("file_stream")
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

TEST_CASE("parse")
{
    std::string_view data;

    data = "123";
    CHECK_EQ(brayns::Parse::fromString<int64_t>(data), 123);

    data = "agss";
    CHECK_THROWS_AS(brayns::Parse::fromString<int64_t>(data), std::invalid_argument);

    data = "1.22";
    CHECK_EQ(brayns::Parse::fromString<double>(data), 1.22);
    CHECK_THROWS_AS(brayns::Parse::fromString<int32_t>(data), std::invalid_argument);

    data = "1234";
    CHECK_THROWS_AS(brayns::Parse::fromString<int8_t>(data), std::out_of_range);

    int32_t test = 123;
    auto bytes = brayns::ByteConverter::getBytes(test);
    data = {bytes, sizeof(test)};
    CHECK_EQ(brayns::Parse::fromBytes<int32_t>(data), test);

    data = "1 2 3";
    auto ref = brayns::Vector3f(1, 2, 3);
    CHECK_EQ(brayns::Parse::fromTokens<brayns::Vector3f>(data), ref);

    bytes = brayns::ByteConverter::getBytes(ref);
    data = {bytes, sizeof(ref)};
    CHECK_EQ(brayns::Parse::fromBytes<brayns::Vector3f>(data), ref);
}

TEST_CASE("string_converter")
{
    auto toLowerChar = brayns::StringConverter::toLower('A');
    CHECK_EQ(toLowerChar, 'a');

    auto toUpperChar = brayns::StringConverter::toUpper('a');
    CHECK_EQ(toUpperChar, 'A');

    std::string test;

    auto toLowerString = brayns::StringConverter::toLower("aBcDef");
    CHECK_EQ(toLowerString, "abcdef");

    auto toUpperString = brayns::StringConverter::toUpper("aBcDef");
    CHECK_EQ(toUpperString, "ABCDEF");

    std::string lower = "aBcDef";
    brayns::StringConverter::lower(lower);
    CHECK_EQ(lower, "abcdef");

    std::string upper = "aBcDef";
    brayns::StringConverter::upper(upper);
    CHECK_EQ(upper, "ABCDEF");
}

TEST_CASE("string_counter")
{
    auto countChar = brayns::StringCounter::count("test this please", 'e');
    CHECK_EQ(countChar, 3);

    auto countString = brayns::StringCounter::count("this this something this", "this");
    CHECK_EQ(countString, 3);

    auto countOneOf = brayns::StringCounter::countOneOf("ethis this something thisi", "aeiouy");
    CHECK_EQ(countOneOf, 8);

    auto countTokens = brayns::StringCounter::countTokens("   one\n\ttwo three \rfour\n\n");
    CHECK_EQ(countTokens, 4);

    auto countLines = brayns::StringCounter::countLines("   one\n\ttwo three \rfour\n\n");
    CHECK_EQ(countLines, 4);
}

TEST_CASE("string_info")
{
    CHECK_FALSE(brayns::StringInfo::isSpace('c'));
    CHECK(brayns::StringInfo::isSpace(' '));

    CHECK_FALSE(brayns::StringInfo::isSpace("  1  "));
    CHECK(brayns::StringInfo::isSpace("\n  \r  \t"));

    CHECK_FALSE(brayns::StringInfo::isLower('C'));
    CHECK(brayns::StringInfo::isLower('c'));

    CHECK_FALSE(brayns::StringInfo::isLower("teSt"));
    CHECK(brayns::StringInfo::isLower("test"));

    CHECK_FALSE(brayns::StringInfo::isUpper('c'));
    CHECK(brayns::StringInfo::isUpper('C'));

    CHECK_FALSE(brayns::StringInfo::isUpper("TEsT"));
    CHECK(brayns::StringInfo::isUpper("TEST"));

    CHECK_FALSE((brayns::StringInfo::startsWith("test", 'c')));
    CHECK((brayns::StringInfo::startsWith("test", 't')));

    CHECK_FALSE((brayns::StringInfo::startsWith("test", "tex")));
    CHECK((brayns::StringInfo::startsWith("test", "tes")));

    CHECK_FALSE((brayns::StringInfo::endsWith("test", 'c')));
    CHECK((brayns::StringInfo::endsWith("test", 't')));

    CHECK_FALSE((brayns::StringInfo::endsWith("test", "ext")));
    CHECK((brayns::StringInfo::endsWith("test", "est")));
}

TEST_CASE("string_joiner")
{
    auto joinEmpty = brayns::StringJoiner::join({}, '.');
    CHECK_EQ(joinEmpty, "");

    auto joinChar = brayns::StringJoiner::join({"1", "2", "3"}, '.');
    CHECK_EQ(joinChar, "1.2.3");

    auto joinString = brayns::StringJoiner::join({"1", "2", "3"}, "--");
    CHECK_EQ(joinString, "1--2--3");
}

TEST_CASE("string_stream")
{
    auto stream = brayns::StringStream();
    auto extracted = std::string_view();

    stream = {" test1 test2 "};
    extracted = stream.extractUntil(' ');
    CHECK_EQ(extracted, "");
    extracted = stream.extractUntil(' ');
    CHECK_EQ(extracted, "test1");
    extracted = stream.extractUntil(' ');
    CHECK_EQ(extracted, "test2");
    extracted = stream.extractUntil(' ');
    CHECK_EQ(extracted, "");
    extracted = stream.extractUntil(' ');
    CHECK_EQ(extracted, "");

    stream = {"septest1septest2sep"};
    extracted = stream.extractUntil("sep");
    CHECK_EQ(extracted, "");
    extracted = stream.extractUntil("sep");
    CHECK_EQ(extracted, "test1");
    extracted = stream.extractUntil("sep");
    CHECK_EQ(extracted, "test2");
    extracted = stream.extractUntil("sep");
    CHECK_EQ(extracted, "");
    extracted = stream.extractUntil("sep");
    CHECK_EQ(extracted, "");

    stream = {"atest1btest2c"};
    extracted = stream.extractUntilOneOf("abc");
    CHECK_EQ(extracted, "");
    extracted = stream.extractUntilOneOf("abc");
    CHECK_EQ(extracted, "test1");
    extracted = stream.extractUntilOneOf("abc");
    CHECK_EQ(extracted, "test2");
    extracted = stream.extractUntilOneOf("abc");
    CHECK_EQ(extracted, "");
    extracted = stream.extractUntilOneOf("abc");
    CHECK_EQ(extracted, "");

    stream = {"\n test1  \r\vtest2\t "};
    extracted = stream.extractToken();
    CHECK_EQ(extracted, "test1");
    extracted = stream.extractToken();
    CHECK_EQ(extracted, "test2");
    extracted = stream.extractToken();
    CHECK_EQ(extracted, "");
    extracted = stream.extractToken();
    CHECK_EQ(extracted, "");

    stream = {"\n test1  \n\r\vtest2\t \n"};
    extracted = stream.extractLine();
    CHECK_EQ(extracted, "");
    extracted = stream.extractLine();
    CHECK_EQ(extracted, " test1  ");
    extracted = stream.extractLine();
    CHECK_EQ(extracted, "\r\vtest2\t ");
    extracted = stream.extractLine();
    CHECK_EQ(extracted, "");
    extracted = stream.extractLine();
    CHECK_EQ(extracted, "");
}

TEST_CASE("string_trimmer")
{
    CHECK_EQ(brayns::StringTrimmer::trimLeft("  \ttest\n "), "test\n ");
    CHECK_EQ(brayns::StringTrimmer::trimRight("  \ttest\n "), "  \ttest");
    CHECK_EQ(brayns::StringTrimmer::trim("  \ttest\n "), "test");
}
