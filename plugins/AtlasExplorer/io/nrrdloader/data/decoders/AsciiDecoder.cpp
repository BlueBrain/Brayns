/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "AsciiDecoder.h"

#include <brayns/utils/parsing/Parser.h>
#include <brayns/utils/string/StringCounter.h>
#include <brayns/utils/string/StringExtractor.h>
#include <brayns/utils/string/StringParser.h>

#include <stdexcept>

namespace
{
class AsciiParser
{
public:
    template<typename T>
    static std::vector<T> parse(std::string_view input, size_t tokenCount)
    {
        std::vector<T> result;
        result.reserve(tokenCount);
        while (!input.empty())
        {
            auto &value = result.emplace_back();
            brayns::Parser::extractToken<T>(input, value);
            brayns::StringExtractor::extract(input, 1);
        }
        return result;
    }
};

template<typename T>
class DecodedDataBuilder
{
public:
    static std::unique_ptr<IDataMangler> parseAndBuild(std::string_view input, size_t tokenCount)
    {
        auto data = AsciiParser::parse<T>(input, tokenCount);
        return std::make_unique<DataMangler<T>>(std::move(data));
    }
};
}

std::unique_ptr<IDataMangler> AsciiDecoder::decode(const NRRDHeader &header, std::string_view input) const
{
    auto tokenCount = brayns::StringCounter::countTokens(input);
    auto expectedTokens = NRRDExpectedSize::compute(header);
    if (tokenCount != expectedTokens)
    {
        throw std::runtime_error("Expected size and parsed element count is different");
    }

    switch (header.type)
    {
    case NRRDType::Char:
        return DecodedDataBuilder<char>::parseAndBuild(input, tokenCount);
    case NRRDType::UnsignedChar:
        return DecodedDataBuilder<uint8_t>::parseAndBuild(input, tokenCount);
    case NRRDType::Short:
        return DecodedDataBuilder<int16_t>::parseAndBuild(input, tokenCount);
    case NRRDType::UnsignedShort:
        return DecodedDataBuilder<uint16_t>::parseAndBuild(input, tokenCount);
    case NRRDType::Int:
        return DecodedDataBuilder<int32_t>::parseAndBuild(input, tokenCount);
    case NRRDType::UnsignedInt:
        return DecodedDataBuilder<uint32_t>::parseAndBuild(input, tokenCount);
    case NRRDType::Long:
        return DecodedDataBuilder<int64_t>::parseAndBuild(input, tokenCount);
    case NRRDType::UnsignedLong:
        return DecodedDataBuilder<uint64_t>::parseAndBuild(input, tokenCount);
    case NRRDType::Float:
        return DecodedDataBuilder<float>::parseAndBuild(input, tokenCount);
    default:
        return DecodedDataBuilder<double>::parseAndBuild(input, tokenCount);
    }
}
