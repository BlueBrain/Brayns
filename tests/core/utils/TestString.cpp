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

#include <brayns/core/utils/string/StringCase.h>
#include <brayns/core/utils/string/StringCounter.h>
#include <brayns/core/utils/string/StringExtractor.h>
#include <brayns/core/utils/string/StringInfo.h>
#include <brayns/core/utils/string/StringJoiner.h>
#include <brayns/core/utils/string/StringParser.h>
#include <brayns/core/utils/string/StringSplitter.h>
#include <brayns/core/utils/string/StringTrimmer.h>

#include <doctest.h>

#include <stdexcept>

TEST_CASE("String case")
{
    auto toLowerChar = brayns::StringCase::toLower('A');
    CHECK_EQ(toLowerChar, 'a');

    auto toUpperChar = brayns::StringCase::toUpper('a');
    CHECK_EQ(toUpperChar, 'A');

    auto toLowerString = brayns::StringCase::toLower("aB cDef ");
    CHECK_EQ(toLowerString, "ab cdef ");

    auto toUpperString = brayns::StringCase::toUpper("aB cDef ");
    CHECK_EQ(toUpperString, "AB CDEF ");

    std::string lower = "aB cDef ";
    brayns::StringCase::lower(lower);
    CHECK_EQ(lower, "ab cdef ");

    std::string upper = "aB cDef ";
    brayns::StringCase::upper(upper);
    CHECK_EQ(upper, "AB CDEF ");
}

TEST_CASE("String counter")
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

TEST_CASE("String extractor")
{
    auto data = std::string_view(" test1 test2 ");
    auto extracted = brayns::StringExtractor::extractUntil(data, ' ');
    CHECK_EQ(extracted, "");
    brayns::StringExtractor::extract(data, 1);
    extracted = brayns::StringExtractor::extractUntil(data, ' ');
    CHECK_EQ(extracted, "test1");
    brayns::StringExtractor::extract(data, 1);
    extracted = brayns::StringExtractor::extractUntil(data, ' ');
    CHECK_EQ(extracted, "test2");
    brayns::StringExtractor::extract(data, 1);
    extracted = brayns::StringExtractor::extractUntil(data, ' ');
    CHECK_EQ(extracted, "");
    brayns::StringExtractor::extract(data, 1);
    extracted = brayns::StringExtractor::extractUntil(data, ' ');
    CHECK_EQ(extracted, "");

    data = "septest1septest2sep";
    extracted = brayns::StringExtractor::extractUntil(data, "sep");
    CHECK_EQ(extracted, "");
    brayns::StringExtractor::extract(data, 3);
    extracted = brayns::StringExtractor::extractUntil(data, "sep");
    CHECK_EQ(extracted, "test1");
    brayns::StringExtractor::extract(data, 3);
    extracted = brayns::StringExtractor::extractUntil(data, "sep");
    CHECK_EQ(extracted, "test2");
    brayns::StringExtractor::extract(data, 3);
    extracted = brayns::StringExtractor::extractUntil(data, "sep");
    CHECK_EQ(extracted, "");
    brayns::StringExtractor::extract(data, 3);
    extracted = brayns::StringExtractor::extractUntil(data, "sep");
    CHECK_EQ(extracted, "");

    data = "atest1btest2c";
    extracted = brayns::StringExtractor::extractUntilOneOf(data, "abc");
    CHECK_EQ(extracted, "");
    brayns::StringExtractor::extract(data, 1);
    extracted = brayns::StringExtractor::extractUntilOneOf(data, "abc");
    CHECK_EQ(extracted, "test1");
    brayns::StringExtractor::extract(data, 1);
    extracted = brayns::StringExtractor::extractUntilOneOf(data, "abc");
    CHECK_EQ(extracted, "test2");
    brayns::StringExtractor::extract(data, 1);
    extracted = brayns::StringExtractor::extractUntilOneOf(data, "abc");
    CHECK_EQ(extracted, "");
    brayns::StringExtractor::extract(data, 1);
    extracted = brayns::StringExtractor::extractUntilOneOf(data, "abc");
    CHECK_EQ(extracted, "");

    data = "\n test1  \r\vtest2\t ";
    extracted = brayns::StringExtractor::extractToken(data);
    CHECK_EQ(extracted, "test1");
    extracted = brayns::StringExtractor::extractToken(data);
    CHECK_EQ(extracted, "test2");
    extracted = brayns::StringExtractor::extractToken(data);
    CHECK_EQ(extracted, "");
    extracted = brayns::StringExtractor::extractToken(data);
    CHECK_EQ(extracted, "");

    data = "\n test1  \n\r\vtest2\t \n";
    extracted = brayns::StringExtractor::extractLine(data);
    CHECK_EQ(extracted, "");
    brayns::StringExtractor::extract(data, 1);
    extracted = brayns::StringExtractor::extractLine(data);
    CHECK_EQ(extracted, " test1  ");
    brayns::StringExtractor::extract(data, 1);
    extracted = brayns::StringExtractor::extractLine(data);
    CHECK_EQ(extracted, "\r\vtest2\t ");
    brayns::StringExtractor::extract(data, 1);
    extracted = brayns::StringExtractor::extractLine(data);
    CHECK_EQ(extracted, "");
    brayns::StringExtractor::extract(data, 1);
    extracted = brayns::StringExtractor::extractLine(data);
    CHECK_EQ(extracted, "");
}

TEST_CASE("String info")
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

    CHECK_FALSE((brayns::StringInfo::contains("test", 'd')));
    CHECK((brayns::StringInfo::contains("test", 'e')));

    CHECK_FALSE((brayns::StringInfo::contains("test", "ext")));
    CHECK((brayns::StringInfo::contains("test", "es")));

    CHECK_FALSE((brayns::StringInfo::containsOneOf("test", "abc")));
    CHECK((brayns::StringInfo::containsOneOf("test", "aec")));

    CHECK_FALSE((brayns::StringInfo::containsToken(" test1  test23 ", "test2")));
    CHECK((brayns::StringInfo::containsOneOf(" test1  test2 fyxdg", "test2")));
}

TEST_CASE("String joiner")
{
    auto joinEmpty = brayns::StringJoiner::join({}, '.');
    CHECK_EQ(joinEmpty, "");

    auto joinChar = brayns::StringJoiner::join({"1", "2", "3"}, '.');
    CHECK_EQ(joinChar, "1.2.3");

    auto joinString = brayns::StringJoiner::join({"1", "2", "3"}, "--");
    CHECK_EQ(joinString, "1--2--3");
}

TEST_CASE("String parser")
{
    std::string_view data;

    data = "123";
    uint64_t ui64 = 0;
    brayns::StringParser<uint64_t>::parse(data, ui64);
    CHECK_EQ(ui64, 123);

    data = "123.567";
    double d = 0.0;
    brayns::StringParser<double>::parse(data, d);
    CHECK_EQ(d, 123.567);

    data = "true";
    bool b = false;
    brayns::StringParser<bool>::parse(data, b);
    CHECK(b);

    data = "1.5";
    int i = 0;
    CHECK_THROWS_AS((brayns::StringParser<int>::parse(data, i)), std::invalid_argument);

    data = "dgsdghsh";
    float f = 0;
    CHECK_THROWS_AS((brayns::StringParser<float>::parse(data, f)), std::invalid_argument);

    data = "1234";
    uint8_t ui8 = 0;
    CHECK_THROWS_AS((brayns::StringParser<uint8_t>::parse(data, ui8)), std::out_of_range);
}

TEST_CASE("String splitter")
{
    std::vector<std::string> ref = {"", "test1", "", "test2", ""};

    CHECK_EQ(brayns::StringSplitter::split(" test1  test2 ", ' '), ref);
    CHECK_EQ(brayns::StringSplitter::split("septest1sepseptest2sep", "sep"), ref);
    CHECK_EQ(brayns::StringSplitter::splitOneOf("atest1bctest2d", "abcd"), ref);
    CHECK_EQ(brayns::StringSplitter::splitLines("\ntest1\n\ntest2\n"), ref);

    ref = {"test1", "test2"};
    CHECK_EQ(brayns::StringSplitter::splitTokens(" test1 \n test2   "), ref);
}

TEST_CASE("String trimmer")
{
    CHECK_EQ(brayns::StringTrimmer::trimLeft("  \ttest\n "), "test\n ");
    CHECK_EQ(brayns::StringTrimmer::trimRight("  \ttest\n "), "  \ttest");
    CHECK_EQ(brayns::StringTrimmer::trim("  \ttest\n "), "test");
}
