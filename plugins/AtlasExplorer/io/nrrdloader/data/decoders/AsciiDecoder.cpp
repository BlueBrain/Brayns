/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "common/Tokenizer.h"

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
    static std::vector<T> parse(const std::vector<std::string_view> &tokens)
    {
        std::vector<T> result(tokens.size());

        for (size_t i = 0; i < tokens.size(); ++i)
        {
            brayns::StringParser<T>::parse(tokens[i], result[i]);
        }
        return result;
    }
};

template<typename T>
class DecodedDataBuilder
{
public:
    static std::unique_ptr<IDataMangler> parseAndBuild(const std::vector<std::string_view> &tokens)
    {
        auto data = AsciiParser::parse<T>(tokens);
        return std::make_unique<DataMangler<T>>(std::move(data));
    }
};
}

std::unique_ptr<IDataMangler> AsciiDecoder::decode(const NRRDHeader &header, std::string_view input) const
{
    auto tokens = Tokenizer::fromView(input);

    auto expectedSize = NRRDExpectedSize::compute(header);
    if (expectedSize != tokens.size())
    {
        throw std::runtime_error("Expected size and parsed element count is different");
    }

    switch (header.type)
    {
    case NRRDType::Char:
        return DecodedDataBuilder<char>::parseAndBuild(tokens);
    case NRRDType::UnsignedChar:
        return DecodedDataBuilder<uint8_t>::parseAndBuild(tokens);
    case NRRDType::Short:
        return DecodedDataBuilder<int16_t>::parseAndBuild(tokens);
    case NRRDType::UnsignedShort:
        return DecodedDataBuilder<uint16_t>::parseAndBuild(tokens);
    case NRRDType::Int:
        return DecodedDataBuilder<int32_t>::parseAndBuild(tokens);
    case NRRDType::UnsignedInt:
        return DecodedDataBuilder<uint32_t>::parseAndBuild(tokens);
    case NRRDType::Long:
        return DecodedDataBuilder<int64_t>::parseAndBuild(tokens);
    case NRRDType::UnsignedLong:
        return DecodedDataBuilder<uint64_t>::parseAndBuild(tokens);
    case NRRDType::Float:
        return DecodedDataBuilder<float>::parseAndBuild(tokens);
    default:
        return DecodedDataBuilder<double>::parseAndBuild(tokens);
    }
}
