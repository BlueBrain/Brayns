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

#include <brayns/utils/StringUtils.h>

#include <io/nrrdloader/data/DataFlipper.h>

namespace
{
template<typename T>
class StringCast
{
public:
    static T cast(std::string_view token)
    {
        throw std::runtime_error("Not implemented");
    }
};

template<>
class StringCast<char>
{
public:
    static char cast(std::string_view token)
    {
        return token.at(0);
    }
};

template<>
class StringCast<uint8_t>
{
public:
    static uint8_t cast(std::string_view token)
    {
        return static_cast<uint8_t>(token.at(0));
    }
};

template<>
class StringCast<int16_t>
{
public:
    static int16_t cast(std::string_view token)
    {
        return static_cast<int16_t>(std::stoi(std::string(token)));
    }
};

template<>
class StringCast<uint16_t>
{
public:
    static uint16_t cast(std::string_view token)
    {
        return static_cast<uint16_t>(std::stoul(std::string(token)));
    }
};

template<>
class StringCast<int32_t>
{
public:
    static uint32_t cast(std::string_view token)
    {
        return std::stoi(std::string(token));
    }
};

template<>
class StringCast<uint32_t>
{
public:
    static uint32_t cast(std::string_view token)
    {
        return std::stoul(std::string(token));
    }
};

template<>
class StringCast<int64_t>
{
public:
    static int64_t cast(std::string_view token)
    {
        return std::stol(std::string(token));
    }
};

template<>
class StringCast<uint64_t>
{
public:
    static uint64_t cast(std::string_view token)
    {
        return std::stoul(std::string(token));
    }
};

template<>
class StringCast<float>
{
public:
    static float cast(std::string_view token)
    {
        return std::stof(std::string(token));
    }
};

template<>
class StringCast<double>
{
public:
    static double cast(std::string_view token)
    {
        return std::stod(std::string(token));
    }
};

class AsciiParser
{
public:
    template<typename T>
    static std::vector<T> parse(const std::vector<std::string_view> &tokens)
    {
        std::vector<T> result(tokens.size());

        for (size_t i = 0; i < tokens.size(); ++i)
        {
            result[i] = StringCast<T>::cast(tokens[i]);
        }
        return result;
    }
};

class DecodedDataBuilder
{
public:
    template<typename T>
    static std::unique_ptr<IDataMangler> parseAndBuild(
        const NRRDHeader &header,
        const std::vector<std::string_view> &tokens)
    {
        auto data = AsciiParser::parse<T>(tokens);
        auto flippedData = DataFlipper::flipVertically(header, data);
        return std::make_unique<DataMangler<T>>(std::move(flippedData));
    }
};
}

std::unique_ptr<IDataMangler> AsciiDecoder::decode(const NRRDHeader &header, std::string_view input) const
{
    const auto tokens = brayns::string_utils::split(input, " /r/v/f/t/n");

    const auto expectedSize = NRRDExpectedSize::compute(header);
    if (expectedSize != tokens.size())
    {
        throw std::runtime_error("Expected size and parsed element count is different");
    }

    const auto type = header.type;

    if (type == NRRDType::CHAR)
    {
        return DecodedDataBuilder::parseAndBuild<char>(header, tokens);
    }

    if (type == NRRDType::UNSIGNED_CHAR)
    {
        return DecodedDataBuilder::parseAndBuild<uint8_t>(header, tokens);
    }

    if (type == NRRDType::SHORT)
    {
        return DecodedDataBuilder::parseAndBuild<int16_t>(header, tokens);
    }

    if (type == NRRDType::UNSIGNED_SHORT)
    {
        return DecodedDataBuilder::parseAndBuild<uint16_t>(header, tokens);
    }

    if (type == NRRDType::INT)
    {
        return DecodedDataBuilder::parseAndBuild<int32_t>(header, tokens);
    }

    if (type == NRRDType::UNSIGNED_INT)
    {
        return DecodedDataBuilder::parseAndBuild<uint32_t>(header, tokens);
    }

    if (type == NRRDType::LONG)
    {
        return DecodedDataBuilder::parseAndBuild<int64_t>(header, tokens);
    }

    if (type == NRRDType::UNSIGNED_LONG)
    {
        return DecodedDataBuilder::parseAndBuild<uint64_t>(header, tokens);
    }

    if (type == NRRDType::FLOAT)
    {
        return DecodedDataBuilder::parseAndBuild<float>(header, tokens);
    }

    return DecodedDataBuilder::parseAndBuild<double>(header, tokens);
}
