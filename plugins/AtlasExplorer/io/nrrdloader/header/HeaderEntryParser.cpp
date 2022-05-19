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

#include "HeaderEntryParser.h"

#include <brayns/common/Log.h>
#include <brayns/utils/StringUtils.h>

#include <spdlog/fmt/bundled/printf.h>

namespace
{
class TypeTableBuilder
{
public:
    static std::unordered_map<std::string, NRRDType> build()
    {
        return {
            {"signed char", NRRDType::CHAR},
            {"int8", NRRDType::CHAR},
            {"int8_t", NRRDType::CHAR},
            {"uchar", NRRDType::UNSIGNED_CHAR},
            {"unsigned char", NRRDType::UNSIGNED_CHAR},
            {"uint8", NRRDType::UNSIGNED_CHAR},
            {"uint8_t", NRRDType::UNSIGNED_CHAR},
            {"short", NRRDType::SHORT},
            {"short int", NRRDType::SHORT},
            {"signed short", NRRDType::SHORT},
            {"signed short int", NRRDType::SHORT},
            {"int16", NRRDType::SHORT},
            {"int16_t", NRRDType::SHORT},
            {"ushort", NRRDType::UNSIGNED_SHORT},
            {"unsigned short", NRRDType::UNSIGNED_SHORT},
            {"unsigned short int", NRRDType::UNSIGNED_SHORT},
            {"uint16", NRRDType::UNSIGNED_SHORT},
            {"uint16_t", NRRDType::UNSIGNED_SHORT},
            {"int", NRRDType::INT},
            {"signed int", NRRDType::INT},
            {"int32", NRRDType::INT},
            {"int32_t", NRRDType::INT},
            {"uint", NRRDType::UNSIGNED_INT},
            {"unsigned int", NRRDType::UNSIGNED_INT},
            {"uint32", NRRDType::UNSIGNED_INT},
            {"uint32_t", NRRDType::UNSIGNED_INT},
            {"longlong", NRRDType::LONG},
            {"long long", NRRDType::LONG},
            {"long long int", NRRDType::LONG},
            {"signed long long", NRRDType::LONG},
            {"signed long long int", NRRDType::LONG},
            {"int64", NRRDType::LONG},
            {"int64_t", NRRDType::LONG},
            {"ulonglong", NRRDType::UNSIGNED_LONG},
            {"unsigned long long", NRRDType::UNSIGNED_LONG},
            {"unsigned long long int", NRRDType::UNSIGNED_LONG},
            {"uint64", NRRDType::UNSIGNED_LONG},
            {"uint64_t", NRRDType::UNSIGNED_LONG},
            {"float", NRRDType::FLOAT},
            {"double", NRRDType::DOUBLE}};
    }
};

class EncodingTableBuilder
{
public:
    static std::unordered_map<std::string, NRRDEncoding> build()
    {
        return {
            {"raw", NRRDEncoding::RAW},
            {"txt", NRRDEncoding::ASCII},
            {"text", NRRDEncoding::ASCII},
            {"ascii", NRRDEncoding::ASCII},
            {"hex", NRRDEncoding::HEX},
            {"gz", NRRDEncoding::GZIP},
            {"gzip", NRRDEncoding::GZIP},
            {"bz2", NRRDEncoding::BZIP2},
            {"bzip2", NRRDEncoding::BZIP2}};
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
            {"domain", NRRDKind::DOMAIN},
            {"space", NRRDKind::SPACE},
            {"scalar", NRRDKind::SCALAR},
            {"vector", NRRDKind::VECTOR},
            {"2-vector", NRRDKind::VECTOR2D},
            {"3-vector", NRRDKind::VECTOR3D},
            {"3-normal", NRRDKind::NORMAL3D},
            {"quaternion", NRRDKind::QUATERNION},
            {"3-gradient", NRRDKind::GRADIENT3},
            {"4-color", NRRDKind::COLOR4},
            {"RGB-color", NRRDKind::RGBCOLOR},
            {"HSV-color", NRRDKind::HSVCOLOR},
            {"XYZ-color", NRRDKind::XYZCOLOR},
            {"RGBA-color", NRRDKind::RGBACOLOR},
            {"none", NRRDKind::NONE},
            {"???", NRRDKind::NONE}};
    }
};

class HeaderEntryParseUtils
{
public:
    static std::string sanitizeValue(std::string_view value)
    {
        std::string valueStr(value);
        brayns::string_utils::trim(valueStr);
        return brayns::string_utils::toLowercase(std::move(valueStr));
    }

    static std::vector<std::string> parseStringArray(std::string_view value, char delimiter = ' ')
    {
        auto valueStr = std::string(value);
        brayns::string_utils::trim(valueStr);
        return brayns::string_utils::split(valueStr, delimiter);
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
            if (tokens[i] != "none")
            {
                result.push_back(parseFloatVector(tokens[i]));
            }
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
            header.endian = NRRDEndianness::LITTLE;
            return;
        }

        header.endian = NRRDEndianness::BIG;
    }
};

class ContentParser
{
public:
    static void parse(std::string_view value, NRRDHeader &header)
    {
        auto valueStr = std::string(value);
        brayns::string_utils::trim(valueStr);
        header.content = std::move(valueStr);
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
        auto valueStr = std::string(value);
        brayns::string_utils::trim(valueStr);

        auto tokens = brayns::string_utils::split(valueStr, ' ');

        // 1st form: a filename
        if (tokens.size() == 1)
        {
            header.dataFiles = {valueStr};
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
        auto valueStr = std::string(value);
        brayns::string_utils::trim(valueStr);
        header.sampleUnits = std::move(valueStr);
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

        if (header.sizes.size() != header.dimensions)
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

        if (header.spacings->size() != header.dimensions)
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
        if (kindTokens.size() != header.dimensions)
        {
            throw std::invalid_argument("Ill-formed NRRD header: dimensions and kinds length missmatch");
        }

        const auto kindTable = KindsTableBuilder::build();
        std::vector<NRRDKind> kinds(kindTokens.size());
        for (size_t i = 0; i < kindTokens.size(); ++i)
        {
            auto token = kindTokens[i];
            brayns::string_utils::trim(token);
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
        brayns::string_utils::trim(valueStr);

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
        _addParser(entryParserMap, "dimension", DimensionParser::parse);
        _addParser(entryParserMap, "type", TypeParser::parse);
        _addParser(entryParserMap, "encoding", EncodingParser::parse);
        _addParser(entryParserMap, "endian", EndianParser::parse);
        _addParser(entryParserMap, "content", ContentParser::parse);
        _addParser(entryParserMap, "min", MinParser::parse);
        _addParser(entryParserMap, "max", MaxParser::parse);
        _addParser(entryParserMap, "data file", DatafileParser::parse);
        _addParser(entryParserMap, "datafile", DatafileParser::parse);
        _addParser(entryParserMap, "sample units", SampleUnitsParser::parse);
        _addParser(entryParserMap, "sampleunits", SampleUnitsParser::parse);
        _addParser(entryParserMap, "sizes", SizesParser::parse);
        _addParser(entryParserMap, "spacings", SpacingsParser::parse);
        _addParser(entryParserMap, "space", SpaceParser::parse);
        _addParser(entryParserMap, "space dimension", SpaceDimensionsParser::parse);
        _addParser(entryParserMap, "space units", SpaceUnitsParser::parse);
        _addParser(entryParserMap, "space origin", SpaceOriginParser::parse);
        _addParser(entryParserMap, "space directions", SpaceDirectionsParser::parse);
        _addParser(entryParserMap, "measurement frame", MeasurementFrameParser::parse);
        _addParser(entryParserMap, "kinds", KindsParser::parse);

        // Not implemented entries
        _addParser(entryParserMap, "block size", NOOPParser::parse);
        _addParser(entryParserMap, "blocksize", NOOPParser::parse);
        _addParser(entryParserMap, "old min", NOOPParser::parse);
        _addParser(entryParserMap, "oldmin", NOOPParser::parse);
        _addParser(entryParserMap, "old max", NOOPParser::parse);
        _addParser(entryParserMap, "oldmax", NOOPParser::parse);
        _addParser(entryParserMap, "line skip", NOOPParser::parse);
        _addParser(entryParserMap, "lineskip", NOOPParser::parse);
        _addParser(entryParserMap, "byte skip", NOOPParser::parse);
        _addParser(entryParserMap, "byteskip", NOOPParser::parse);
        _addParser(entryParserMap, "number", NOOPParser::parse);
        _addParser(entryParserMap, "thicknesses", NOOPParser::parse);
        _addParser(entryParserMap, "axis mins", NOOPParser::parse);
        _addParser(entryParserMap, "axismins", NOOPParser::parse);
        _addParser(entryParserMap, "axis maxs", NOOPParser::parse);
        _addParser(entryParserMap, "axismaxs", NOOPParser::parse);
        _addParser(entryParserMap, "centers", NOOPParser::parse);
        _addParser(entryParserMap, "centerings", NOOPParser::parse);
        _addParser(entryParserMap, "labels", NOOPParser::parse);
        _addParser(entryParserMap, "units", NOOPParser::parse);

        return entryParserMap;
    }

private:
    static void _addParser(EntryParserMap &entryParserMap, std::string_view entryName, ParserCallback callback)
    {
        entryParserMap[std::string(entryName)] = std::move(callback);
    }
};
}

HeaderEntryParser::HeaderEntryParser()
    : _table(HeaderEntryParserTableBuilder::build())
{
}

void HeaderEntryParser::parseEntry(std::string_view key, std::string_view value, NRRDHeader &header)
{
    auto keyStr = brayns::string_utils::toLowercase(std::string(key));
    auto parserIterator = _table.find(keyStr);
    if (parserIterator == _table.end())
    {
        throw std::invalid_argument("Unknown key " + std::string(key));
    }

    auto &callback = parserIterator->second;
    callback(value, header);
}
