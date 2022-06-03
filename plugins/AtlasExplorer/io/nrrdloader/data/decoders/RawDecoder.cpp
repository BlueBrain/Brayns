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

#include <io/nrrdloader/data/DataFlipper.h>

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
        case NRRDType::Char:
        case NRRDType::UnsignedChar:
            return 1;
        case NRRDType::Short:
        case NRRDType::UnsignedShort:
            return 2;
        case NRRDType::Float:
        case NRRDType::Int:
        case NRRDType::UnsignedInt:
            return 4;
        case NRRDType::Double:
        case NRRDType::Long:
        case NRRDType::UnsignedLong:
            return 8;
        default:
            throw std::runtime_error("Unsupported type");
        }
    }
};

class SystemEndiannessChecker
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
        const bool systemIsLittleEndian = SystemEndiannessChecker::isLittleEndian();

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
        std::vector<T> result(numElements, T());

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
        std::vector<T> result(numElements, T());

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

template<typename T>
class ByteAssemblerDecorator
{
public:
    static std::vector<T> assemble(std::string_view input, bool isLittleEndian)
    {
        return ByteAssembler::assemble<T>(input, isLittleEndian);
    }
};

template<>
class ByteAssemblerDecorator<char>
{
public:
    static std::vector<char> assemble(std::string_view input, bool isLittleEndian)
    {
        (void)isLittleEndian;
        return std::vector<char>(input.begin(), input.end());
    }
};

template<>
class ByteAssemblerDecorator<uint8_t>
{
public:
    static std::vector<uint8_t> assemble(std::string_view input, bool isLittleEndian)
    {
        (void)isLittleEndian;
        return std::vector<uint8_t>(input.begin(), input.end());
    }
};

template<typename T>
class DecodedDataBuilder
{
public:
    static std::unique_ptr<IDataMangler> parseAndBuild(const NRRDHeader &header, std::string_view input)
    {
        bool isLittleEndian = header.endian == NRRDEndianness::Little;
        auto data = ByteAssemblerDecorator<T>::assemble(input, isLittleEndian);
        auto flippedData = DataFlipper::flipVertically(header, data);
        return std::make_unique<DataMangler<T>>(std::move(flippedData));
    }
};
}

std::unique_ptr<IDataMangler> RawDecoder::decode(const NRRDHeader &header, std::string_view input) const
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

    if (type == NRRDType::Char)
    {
        return DecodedDataBuilder<char>::parseAndBuild(header, input);
    }

    if (type == NRRDType::UnsignedChar)
    {
        return DecodedDataBuilder<uint8_t>::parseAndBuild(header, input);
    }

    if (type == NRRDType::Short)
    {
        return DecodedDataBuilder<int16_t>::parseAndBuild(header, input);
    }

    if (type == NRRDType::UnsignedShort)
    {
        return DecodedDataBuilder<uint16_t>::parseAndBuild(header, input);
    }

    if (type == NRRDType::Int)
    {
        return DecodedDataBuilder<int32_t>::parseAndBuild(header, input);
    }

    if (type == NRRDType::UnsignedInt)
    {
        return DecodedDataBuilder<uint32_t>::parseAndBuild(header, input);
    }

    if (type == NRRDType::Long)
    {
        return DecodedDataBuilder<int64_t>::parseAndBuild(header, input);
    }

    if (type == NRRDType::UnsignedLong)
    {
        return DecodedDataBuilder<uint64_t>::parseAndBuild(header, input);
    }

    if (type == NRRDType::Float)
    {
        return DecodedDataBuilder<float>::parseAndBuild(header, input);
    }

    return DecodedDataBuilder<double>::parseAndBuild(header, input);
}
