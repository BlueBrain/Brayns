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

#include <brayns/utils/string/StringConverter.h>
#include <brayns/utils/string/StringCounter.h>
#include <brayns/utils/string/StringInfo.h>
#include <brayns/utils/string/StringJoiner.h>
#include <brayns/utils/string/StringParser.h>
#include <brayns/utils/string/StringStream.h>
#include <brayns/utils/string/StringTrimmer.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

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

TEST_CASE("string_parser")
{
    auto integer = brayns::StringParser<int>::parse("123");
    CHECK_EQ(integer, 123);

    auto number = brayns::StringParser<float>::parse("1.23");
    CHECK_EQ(number, 1.23f);

    auto intAsNumber = brayns::StringParser<double>::parse("1");
    CHECK_EQ(intAsNumber, 1.0);

    CHECK_THROWS_AS(brayns::StringParser<uint32_t>::parse("-123"), std::runtime_error);

    CHECK_THROWS_AS(brayns::StringParser<int8_t>::parse("5000"), std::runtime_error);
}

TEST_CASE("string_strean")
{
    auto stream = brayns::StringStream();
    auto extracted = std::string_view();

    stream = {" test1 test2 "};
    extracted = stream.extract(' ');
    CHECK_EQ(extracted, "");
    extracted = stream.extract(' ');
    CHECK_EQ(extracted, "test1");
    extracted = stream.extract(' ');
    CHECK_EQ(extracted, "test2");
    extracted = stream.extract(' ');
    CHECK_EQ(extracted, "");
    extracted = stream.extract(' ');
    CHECK_EQ(extracted, "");

    stream = {"septest1septest2sep"};
    extracted = stream.extract("sep");
    CHECK_EQ(extracted, "");
    extracted = stream.extract("sep");
    CHECK_EQ(extracted, "test1");
    extracted = stream.extract("sep");
    CHECK_EQ(extracted, "test2");
    extracted = stream.extract("sep");
    CHECK_EQ(extracted, "");
    extracted = stream.extract("sep");
    CHECK_EQ(extracted, "");

    stream = {"atest1btest2c"};
    extracted = stream.extractOneOf("abc");
    CHECK_EQ(extracted, "");
    extracted = stream.extractOneOf("abc");
    CHECK_EQ(extracted, "test1");
    extracted = stream.extractOneOf("abc");
    CHECK_EQ(extracted, "test2");
    extracted = stream.extractOneOf("abc");
    CHECK_EQ(extracted, "");
    extracted = stream.extractOneOf("abc");
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
