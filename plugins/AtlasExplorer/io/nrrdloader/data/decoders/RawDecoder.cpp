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

#include "RawDecoder.h"

#include <cassert>
#include <cstring>

namespace
{
class InputSanitizer
{
public:
    static std::string_view sanitize(std::string_view input)
    {
        size_t startOffset = 0;
        while (input[startOffset] == '\n')
        {
            ++startOffset;
        }

        size_t endOffset = 0;
        while (input[input.size() - 1 - endOffset] == '\n')
        {
            ++endOffset;
        }

        return input.substr(startOffset, input.size() - startOffset - endOffset);
    }
};

class NRRDTypeSize
{
public:
    static size_t inBytes(NRRDType type)
    {
        switch (type)
        {
        case NRRDType::CHAR:
        case NRRDType::UNSIGNED_CHAR:
            return 1;
        case NRRDType::SHORT:
        case NRRDType::UNSIGNED_SHORT:
            return 2;
        case NRRDType::FLOAT:
        case NRRDType::INT:
        case NRRDType::UNSIGNED_INT:
            return 4;
        case NRRDType::DOUBLE:
        case NRRDType::LONG:
        case NRRDType::UNSIGNED_LONG:
            return 8;
        default:
            break;
        }

        assert(false);
    }
};

class SystemEndianChecker
{
public:
    static bool isLittleEndian()
    {
        int test = 1;
        auto temp = static_cast<void *>(&test);
        auto bytes = static_cast<char *>(temp);
        return bytes[0] == 1;
    }
};

class ByteAssembler
{
public:
    template<typename T>
    static std::vector<T> assemble(std::string_view input, bool inputIsLittleEndian)
    {
        const bool systemIsLittleEndian = SystemEndianChecker::isLittleEndian();

        if (inputIsLittleEndian && systemIsLittleEndian)
        {
            return _assembleSameEndianness<T>(input);
        }

        if (!inputIsLittleEndian && !systemIsLittleEndian)
        {
            return _assembleSameEndianness<T>(input);
        }

        return _assembleDifferentEndianness<T>(input);
    }

private:
    template<typename T>
    static std::vector<T> _assembleSameEndianness(std::string_view input)
    {
        const auto numElements = input.size() / sizeof(T);
        std::vector<T> result(numElements, 0);

        for (size_t i = 0; i < numElements; ++i)
        {
            auto inputIndex = i * sizeof(T);
            auto inputElementData = &input[inputIndex];
            auto &element = result[i];
            auto tempCast = static_cast<void *>(&element);
            auto elementBytes = static_cast<char *>(tempCast);
            std::memcpy(elementBytes, inputElementData, sizeof(T));
        }

        return result;
    }

    template<typename T>
    static std::vector<T> _assembleDifferentEndianness(std::string_view input)
    {
        const auto numElements = input.size() / sizeof(T);
        std::vector<T> result(numElements);

        for (size_t i = 0; i < numElements; ++i)
        {
            auto inputIndex = i * sizeof(T);
            auto inputElementData = &input[inputIndex];
            auto &element = result[i];
            auto tempCast = static_cast<void *>(&element);
            auto byteCast = static_cast<char *>(tempCast);

            for (size_t j = 0; j < sizeof(T); ++j)
            {
                byteCast[j] = inputElementData[sizeof(T) - j - 1];
            }
        }

        return result;
    }
};

class DecodedDataBuilder
{
public:
    template<typename T>
    static std::unique_ptr<INRRDData> parseAndBuild(std::string_view input, bool isLittleEndian)
    {
        auto data = ByteAssembler::assemble<T>(input, isLittleEndian);
        return std::make_unique<NRRDData<T>>(std::move(data));
    }
};
}

std::unique_ptr<INRRDData> RawDecoder::decode(const NRRDHeader &header, std::string_view input) const
{
    input = InputSanitizer::sanitize(input);

    const auto type = header.type;
    const auto typeSize = NRRDTypeSize::inBytes(type);
    const auto availableSize = input.size() / typeSize;
    const auto expectedSize = NRRDExpectedSize::compute(header);

    if (availableSize != expectedSize)
    {
        throw std::runtime_error("Expected size and parsed element count is different");
    }

    const auto isLittleEndian = header.endian == NRRDEndianness::LITTLE;

    if (type == NRRDType::CHAR)
    {
        auto data = std::vector<char>(input.begin(), input.end());
        return std::make_unique<NRRDData<char>>(std::move(data));
    }

    if (type == NRRDType::UNSIGNED_CHAR)
    {
        auto data = std::vector<uint8_t>(input.begin(), input.end());
        return std::make_unique<NRRDData<uint8_t>>(std::move(data));
    }

    if (type == NRRDType::SHORT)
    {
        return DecodedDataBuilder::parseAndBuild<int16_t>(input, isLittleEndian);
    }

    if (type == NRRDType::UNSIGNED_SHORT)
    {
        return DecodedDataBuilder::parseAndBuild<uint16_t>(input, isLittleEndian);
    }

    if (type == NRRDType::INT)
    {
        return DecodedDataBuilder::parseAndBuild<int32_t>(input, isLittleEndian);
    }

    if (type == NRRDType::UNSIGNED_INT)
    {
        return DecodedDataBuilder::parseAndBuild<uint32_t>(input, isLittleEndian);
    }

    if (type == NRRDType::LONG)
    {
        return DecodedDataBuilder::parseAndBuild<int64_t>(input, isLittleEndian);
    }

    if (type == NRRDType::UNSIGNED_LONG)
    {
        return DecodedDataBuilder::parseAndBuild<uint64_t>(input, isLittleEndian);
    }

    if (type == NRRDType::FLOAT)
    {
        return DecodedDataBuilder::parseAndBuild<float>(input, isLittleEndian);
    }

    return DecodedDataBuilder::parseAndBuild<double>(input, isLittleEndian);
}
