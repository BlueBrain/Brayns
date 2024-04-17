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

#include "HeaderEntryParser.h"

#include <brayns/core/utils/Log.h>
#include <brayns/core/utils/string/StringCase.h>
#include <brayns/core/utils/string/StringSplitter.h>
#include <brayns/core/utils/string/StringTrimmer.h>

#include <fmt/printf.h>

namespace
{
class TypeTableBuilder
{
public:
    static std::unordered_map<std::string, NRRDType> build()
    {
        return {
            {"signed char", NRRDType::Char},
            {"int8", NRRDType::Char},
            {"int8_t", NRRDType::Char},
            {"uchar", NRRDType::UnsignedChar},
            {"unsigned char", NRRDType::UnsignedChar},
            {"uint8", NRRDType::UnsignedChar},
            {"uint8_t", NRRDType::UnsignedChar},
            {"short", NRRDType::Short},
            {"short int", NRRDType::Short},
            {"signed short", NRRDType::Short},
            {"signed short int", NRRDType::Short},
            {"int16", NRRDType::Short},
            {"int16_t", NRRDType::Short},
            {"ushort", NRRDType::UnsignedShort},
            {"unsigned short", NRRDType::UnsignedShort},
            {"unsigned short int", NRRDType::UnsignedShort},
            {"uint16", NRRDType::UnsignedShort},
            {"uint16_t", NRRDType::UnsignedShort},
            {"int", NRRDType::Int},
            {"signed int", NRRDType::Int},
            {"int32", NRRDType::Int},
            {"int32_t", NRRDType::Int},
            {"uint", NRRDType::UnsignedInt},
            {"unsigned int", NRRDType::UnsignedInt},
            {"uint32", NRRDType::UnsignedInt},
            {"uint32_t", NRRDType::UnsignedInt},
            {"longlong", NRRDType::Long},
            {"long long", NRRDType::Long},
            {"long long int", NRRDType::Long},
            {"signed long long", NRRDType::Long},
            {"signed long long int", NRRDType::Long},
            {"int64", NRRDType::Long},
            {"int64_t", NRRDType::Long},
            {"ulonglong", NRRDType::UnsignedLong},
            {"unsigned long long", NRRDType::UnsignedLong},
            {"unsigned long long int", NRRDType::UnsignedLong},
            {"uint64", NRRDType::UnsignedLong},
            {"uint64_t", NRRDType::UnsignedLong},
            {"float", NRRDType::Float},
            {"double", NRRDType::Double}};
    }
};

class EncodingTableBuilder
{
public:
    static std::unordered_map<std::string, NRRDEncoding> build()
    {
        return {
            {"raw", NRRDEncoding::Raw},
            {"txt", NRRDEncoding::Ascii},
            {"text", NRRDEncoding::Ascii},
            {"ascii", NRRDEncoding::Ascii},
            {"hex", NRRDEncoding::Hex},
            {"gz", NRRDEncoding::Gzip},
            {"gzip", NRRDEncoding::Gzip},
            {"bz2", NRRDEncoding::Bzip2},
            {"bzip2", NRRDEncoding::Bzip2}};
    }
};

class SpaceTableBuilder
{
public:
    static std::unordered_map<std::string, int32_t> build()
    {
        return {
            {"right-anterior-superior", 3},
            {"RAS", 3},
            {"left-anterior-superior", 3},
            {"LAS", 3},
            {"left-posterior-superior", 3},
            {"LPS", 3},
            {"right-anterior-superior-time", 4},
            {"RAST", 4},
            {"left-anterior-superior-time", 4},
            {"LAST", 4},
            {"left-posterior-superior-time", 4},
            {"LPST", 4},
            {"scanner-xyz", 3},
            {"scanner-xyz-time", 4},
            {"3D-right-handed", 3},
            {"3D-left-handed", 3},
            {"3D-right-handed-time", 4},
            {"3D-left-handed-time", 4}};
    }
};

class KindsTableBuilder
{
public:
    static std::unordered_map<std::string, NRRDKind> build()
    {
        return {
            {"domain", NRRDKind::Domain},
            {"space", NRRDKind::Space},
            {"scalar", NRRDKind::Scalar},
            {"vector", NRRDKind::Vector},
            {"2-vector", NRRDKind::Vector2D},
            {"3-vector", NRRDKind::Vector3D},
            {"3-normal", NRRDKind::Normal3D},
            {"quaternion", NRRDKind::Quaternion},
            {"3-gradient", NRRDKind::Gradient3},
            {"3-color", NRRDKind::RgbColor},
            {"4-color", NRRDKind::RgbaColor},
            {"RGB-color", NRRDKind::RgbColor},
            {"HSV-color", NRRDKind::HsvColor},
            {"XYZ-color", NRRDKind::XyzColor},
            {"RGBA-color", NRRDKind::RgbaColor},
            {"none", NRRDKind::None},
            {"???", NRRDKind::None}};
    }
};

class HeaderEntryParseUtils
{
public:
    static std::string sanitizeValue(std::string_view value)
    {
        value = brayns::StringTrimmer::trim(value);
        return brayns::StringCase::toLower(value);
    }

    static std::vector<std::string> parseStringArray(std::string_view value, char delimiter = ' ')
    {
        value = brayns::StringTrimmer::trim(value);
        return brayns::StringSplitter::split(value, delimiter);
    }

    static std::vector<int32_t> parseIntArray(std::string_view value)
    {
        auto tokens = parseStringArray(value);
        std::vector<int32_t> result(tokens.size());
        for (size_t i = 0; i < tokens.size(); ++i)
        {
            result[i] = std::stoi(tokens[i]);
        }
        return result;
    }

    static std::vector<float> parseStringListToFloat(const std::vector<std::string> &tokens)
    {
        std::vector<float> result(tokens.size());
        for (size_t i = 0; i < tokens.size(); ++i)
        {
            result[i] = std::stof(tokens[i]);
        }
        return result;
    }

    static std::vector<float> parseFloatArray(std::string_view value)
    {
        const auto tokens = parseStringArray(value);
        return parseStringListToFloat(tokens);
    }

    static std::vector<std::vector<float>> parseFloatFrame(std::string_view value)
    {
        auto tokens = parseStringArray(value);
        std::vector<std::vector<float>> result;
        result.reserve(tokens.size());
        for (size_t i = 0; i < tokens.size(); ++i)
        {
            if (tokens[i] == "none")
            {
                continue;
            }
            result.push_back(parseFloatVector(tokens[i]));
        }
        return result;
    }

    static std::vector<float> parseFloatVector(std::string_view value)
    {
        auto firstParenthesis = value.find("(");
        if (firstParenthesis == std::string_view::npos)
        {
            throw std::invalid_argument("Ill-formed NRRD vector: No opening parenthesis");
        }

        auto secondParenthesis = value.find(")", firstParenthesis + 1);
        if (secondParenthesis == std::string_view::npos)
        {
            throw std::invalid_argument("Ill-fored NRRD vector: No closing parenthesis");
        }

        auto data = value.substr(firstParenthesis + 1, secondParenthesis - firstParenthesis);
        const auto tokens = parseStringArray(data, ',');
        return parseStringListToFloat(tokens);
    }
};

class DimensionParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        header.dimensions = std::stoi(std::string(value));
    }
};

class TypeParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        auto valueStr = HeaderEntryParseUtils::sanitizeValue(value);

        const auto typeTable = TypeTableBuilder::build();
        auto it = typeTable.find(valueStr);

        if (it == typeTable.end())
        {
            if (valueStr == "block")
            {
                throw std::invalid_argument("block data-type is not handled");
            }

            throw std::invalid_argument("Ill-formed NRRD header: Unknown type " + valueStr);
        }

        header.type = it->second;
    }
};

class EncodingParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        auto valueStr = HeaderEntryParseUtils::sanitizeValue(value);

        const auto encodingTable = EncodingTableBuilder::build();
        auto it = encodingTable.find(valueStr);

        if (it == encodingTable.end())
        {
            throw std::invalid_argument("Ill-formed NRRD header: Unknown encoding " + valueStr);
        }

        header.encoding = it->second;
    }
};

class EndianParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        auto valueStr = HeaderEntryParseUtils::sanitizeValue(value);

        if (valueStr == "little")
        {
            header.endian = NRRDEndianness::Little;
            return;
        }

        header.endian = NRRDEndianness::Big;
    }
};

class ContentParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        header.content = brayns::StringTrimmer::trim(value);
    }
};

class MinParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        header.min = std::stod(std::string(value));
    }
};

class MaxParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        header.max = std::stod(std::string(value));
    }
};

class DatafileParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        value = brayns::StringTrimmer::trim(value);

        auto tokens = brayns::StringSplitter::split(value, ' ');

        // 1st form: a filename
        if (tokens.size() == 1)
        {
            header.dataFiles = {std::string(value)};
            return;
        }

        // 2nd form: a list of files whose filename can be build from the format and bounds
        // <format> <min> <max> <step> [<subdim>]
        // If is this form, the second argument must be parseable as integer
        if (tokens.size() == 4 || tokens.size() == 5)
        {
            try
            {
                auto min = std::stoi(tokens[1]);
                auto max = std::stoi(tokens[2]);
                auto step = std::stoi(tokens[3]);
                // We dont care for the subdim as we will just concatenate all files contents together
                header.dataFiles = _handleFileFormat(tokens[0], min, max, step);
                return;
            }
            catch (...)
            {
            }
        }

        // 3rd form: a list of files
        header.dataFiles = _handleFilelist(tokens);
    }

private:
    static std::vector<std::string> _handleFileFormat(const std::string &format, int min, int max, int step)
    {
        if (step == 0)
        {
            throw std::invalid_argument("Ill-formed NRRD header: datafile has a zero step");
        }
        if (step < 0 && min < max)
        {
            throw std::invalid_argument(
                "Ill-formed NRRD header: datafile has negative step, but min is smaller than max");
        }
        if (step > 0 && max < min)
        {
            throw std::invalid_argument(
                "Ill-formed NRRD header: datafile has positive step, but max is smaller than max");
        }

        auto numberOfSteps = std::abs(max - min) / std::abs(step);
        numberOfSteps = numberOfSteps == 0 ? 1 : numberOfSteps;

        std::vector<std::string> result(numberOfSteps);

        for (int i = 0; i < numberOfSteps; ++i)
        {
            auto fileNumber = min + step * i;
            result[i] = fmt::sprintf(format, fileNumber);
        }

        return result;
    }

    static std::vector<std::string> _handleFilelist(std::vector<std::string> inputList)
    {
        // Last element might be subdimensions. Since we dont care about it, we just check for it,
        // remove if present, and return
        auto &last = inputList.back();
        try
        {
            auto subDimensions = std::stoi(last);
            (void)subDimensions;
            inputList.pop_back();
            return inputList;
        }
        catch (...)
        {
        }

        return inputList;
    }
};

class SampleUnitsParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        header.sampleUnits = brayns::StringTrimmer::trim(value);
    }
};

class SizesParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        if (header.dimensions == -1)
        {
            throw std::invalid_argument("Ill-formed NRRD header: dimensions not setted before sizes");
        }

        header.sizes = HeaderEntryParseUtils::parseIntArray(value);

        if (header.sizes.size() != static_cast<size_t>(header.dimensions))
        {
            throw std::invalid_argument("Ill-formed NRRD header: dimensions and sizes length missmatch");
        }
    }
};

class SpacingsParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        if (header.dimensions == -1)
        {
            throw std::invalid_argument("Ill-formed NRRD header: dimensions not setted before spacings");
        }

        header.spacings = HeaderEntryParseUtils::parseFloatArray(value);

        if (header.spacings->size() != static_cast<size_t>(header.dimensions))
        {
            throw std::invalid_argument("Ill-formed NRRD header: dimensions and spacings length missmatch");
        }
    }
};

class KindsParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        if (header.dimensions == -1)
        {
            throw std::invalid_argument("Ill-formed NRRD header: dimensions not setted before kinds");
        }

        auto kindTokens = HeaderEntryParseUtils::parseStringArray(value);
        if (kindTokens.size() != static_cast<size_t>(header.dimensions))
        {
            throw std::invalid_argument("Ill-formed NRRD header: dimensions and kinds length missmatch");
        }

        const auto kindTable = KindsTableBuilder::build();
        std::vector<NRRDKind> kinds(kindTokens.size());
        for (size_t i = 0; i < kindTokens.size(); ++i)
        {
            auto token = kindTokens[i];
            token = brayns::StringTrimmer::trim(token);
            auto kindIterator = kindTable.find(token);
            if (kindIterator == kindTable.end())
            {
                throw std::invalid_argument("Unsupported NRRD Kind: " + token);
            }

            kinds[i] = kindIterator->second;
        }

        header.kinds = std::move(kinds);
    }
};

class SpaceParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        auto valueStr = std::string(value);
        valueStr = brayns::StringTrimmer::trim(valueStr);

        const auto spaceTable = SpaceTableBuilder::build();
        auto spaceIterator = spaceTable.find(valueStr);
        if (spaceIterator == spaceTable.end())
        {
            throw std::invalid_argument("Ill-formed NRRD header: unknown space value " + valueStr);
        }

        header.spaceDimensions = spaceIterator->second;
    }
};

class SpaceDimensionsParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        header.spaceDimensions = std::stoi(std::string(value));
    }
};

class SpaceUnitsParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        if (!header.spaceDimensions)
        {
            throw std::invalid_argument("Ill-formed NRRD header: space dimensions not setted before space units");
        }

        auto units = HeaderEntryParseUtils::parseStringArray(value);
        if (units.size() != static_cast<size_t>(*header.spaceDimensions))
        {
            throw std::invalid_argument("Ill-formed NRRD header: space dimensions and space units size missmatch");
        }

        header.spaceUnits = units;
    }
};

class SpaceOriginParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        if (!header.spaceDimensions)
        {
            throw std::invalid_argument("Ill-formed NRRD header: space dimensions not setted before space origin");
        }

        auto origin = HeaderEntryParseUtils::parseFloatVector(value);
        if (origin.size() != static_cast<size_t>(*header.spaceDimensions))
        {
            throw std::invalid_argument("Ill-formed NRRD header: space dimensions and space origin size missmatch");
        }

        header.spaceOrigin = origin;
    }
};

class SpaceDirectionsParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        if (!header.spaceDimensions)
        {
            throw std::invalid_argument("Ill-formed NRRD header: space dimensions not setted before space directions");
        }

        auto dimensions = static_cast<size_t>(*header.spaceDimensions);
        auto directions = HeaderEntryParseUtils::parseFloatFrame(value);
        if (directions.size() != dimensions)
        {
            throw std::invalid_argument("Ill-formed NRRD header: space dimensions and space directions size missmatch");
        }

        for (const auto &direction : directions)
        {
            if (direction.size() != dimensions)
            {
                throw std::invalid_argument(
                    "Ill-formed NRRD header: space dimensions and space directions size missmatch");
            }
        }

        header.spaceDirections = directions;
    }
};

class MeasurementFrameParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        if (!header.spaceDimensions)
        {
            throw std::invalid_argument("Ill-formed NRRD header: space dimensions not setted before measurement frame");
        }

        auto dimensions = static_cast<size_t>(*header.spaceDimensions);
        auto measurementFrame = HeaderEntryParseUtils::parseFloatFrame(value);
        if (measurementFrame.size() != dimensions)
        {
            throw std::invalid_argument(
                "Ill-formed NRRD header: space dimensions and measurement frame size missmatch");
        }

        for (const auto &frame : measurementFrame)
        {
            if (frame.size() != dimensions)
            {
                throw std::invalid_argument(
                    "Ill-formed NRRD header: space dimensions and measurement frame size missmatch");
            }
        }

        header.measurementFrame = measurementFrame;
    }
};

class NOOPParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        (void)header;
        brayns::Log::debug("Ignoring NRRD header entry and value for '{}'", value);
    }
};

class HeaderEntryParserTableBuilder
{
public:
    using ParserCallback = std::function<void(std::string_view, NRRDHeader &)>;
    using EntryParserMap = std::unordered_map<std::string, ParserCallback>;

    static EntryParserMap build()
    {
        EntryParserMap entryParserMap;
        // a static var with the entry name on each parser so that it can be registered using a template function
        // least not forget problems derived from static variable in runtime loaded libraries
        // Furhtermore, some of them has multiple possible key representations
        _addParser(entryParserMap, "dimension", [](auto data, auto &header) { DimensionParser::parse(data, header); });
        _addParser(entryParserMap, "type", [](auto data, auto &header) { TypeParser::parse(data, header); });
        _addParser(entryParserMap, "encoding", [](auto data, auto &header) { EncodingParser::parse(data, header); });
        _addParser(entryParserMap, "endian", [](auto data, auto &header) { EndianParser::parse(data, header); });
        _addParser(entryParserMap, "content", [](auto data, auto &header) { ContentParser::parse(data, header); });
        _addParser(entryParserMap, "min", [](auto data, auto &header) { MinParser::parse(data, header); });
        _addParser(entryParserMap, "max", [](auto data, auto &header) { MaxParser::parse(data, header); });
        _addParser(entryParserMap, "data file", [](auto data, auto &header) { DatafileParser::parse(data, header); });
        _addParser(entryParserMap, "datafile", [](auto data, auto &header) { DatafileParser::parse(data, header); });
        _addParser(
            entryParserMap,
            "sample units",
            [](auto data, auto &header) { SampleUnitsParser::parse(data, header); });
        _addParser(
            entryParserMap,
            "sampleunits",
            [](auto data, auto &header) { SampleUnitsParser::parse(data, header); });
        _addParser(entryParserMap, "sizes", [](auto data, auto &header) { SizesParser::parse(data, header); });
        _addParser(entryParserMap, "spacings", [](auto data, auto &header) { SpacingsParser::parse(data, header); });
        _addParser(entryParserMap, "space", [](auto data, auto &header) { SpaceParser::parse(data, header); });
        _addParser(
            entryParserMap,
            "space dimension",
            [](auto data, auto &header) { SpaceDimensionsParser::parse(data, header); });
        _addParser(
            entryParserMap,
            "space units",
            [](auto data, auto &header) { SpaceUnitsParser::parse(data, header); });
        _addParser(
            entryParserMap,
            "space origin",
            [](auto data, auto &header) { SpaceOriginParser::parse(data, header); });
        _addParser(
            entryParserMap,
            "space directions",
            [](auto data, auto &header) { SpaceDirectionsParser::parse(data, header); });
        _addParser(
            entryParserMap,
            "measurement frame",
            [](auto data, auto &header) { MeasurementFrameParser::parse(data, header); });
        _addParser(entryParserMap, "kinds", [](auto data, auto &header) { KindsParser::parse(data, header); });

        // Not implemented entries
        const auto noopCallback = [](auto data, auto &header) { NOOPParser::parse(data, header); };
        _addParser(entryParserMap, "block size", noopCallback);
        _addParser(entryParserMap, "blocksize", noopCallback);
        _addParser(entryParserMap, "old min", noopCallback);
        _addParser(entryParserMap, "oldmin", noopCallback);
        _addParser(entryParserMap, "old max", noopCallback);
        _addParser(entryParserMap, "oldmax", noopCallback);
        _addParser(entryParserMap, "line skip", noopCallback);
        _addParser(entryParserMap, "lineskip", noopCallback);
        _addParser(entryParserMap, "byte skip", noopCallback);
        _addParser(entryParserMap, "byteskip", noopCallback);
        _addParser(entryParserMap, "number", noopCallback);
        _addParser(entryParserMap, "thicknesses", noopCallback);
        _addParser(entryParserMap, "axis mins", noopCallback);
        _addParser(entryParserMap, "axismins", noopCallback);
        _addParser(entryParserMap, "axis maxs", noopCallback);
        _addParser(entryParserMap, "axismaxs", noopCallback);
        _addParser(entryParserMap, "centers", noopCallback);
        _addParser(entryParserMap, "centerings", noopCallback);
        _addParser(entryParserMap, "labels", noopCallback);
        _addParser(entryParserMap, "units", noopCallback);

        return entryParserMap;
    }

private:
    static void _addParser(EntryParserMap &entryParserMap, std::string_view entryName, ParserCallback callback)
    {
        entryParserMap[std::string(entryName)] = std::move(callback);
    }
};
}

HeaderEntryParser::HeaderEntryParser():
    _table(HeaderEntryParserTableBuilder::build())
{
}

void HeaderEntryParser::parseEntry(std::string_view key, std::string_view value, NRRDHeader &header)
{
    auto keyStr = std::string(key);
    keyStr = brayns::StringCase::toLower(key);
    auto parserIterator = _table.find(keyStr);
    if (parserIterator == _table.end())
    {
        throw std::invalid_argument("Unknown key " + std::string(key));
    }

    auto &callback = parserIterator->second;
    callback(value, header);
}
