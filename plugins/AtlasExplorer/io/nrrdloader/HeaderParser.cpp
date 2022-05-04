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

#include "HeaderParser.h"

#include <brayns/utils/StringUtils.h>

#include <filesystem>
#include <functional>
#include <unordered_map>
#include <vector>

namespace
{
class HeaderReader
{
public:
    static std::vector<std::string_view> readLines(std::string_view &nrrdContentView)
    {
        // Extracts magic, which we do not use
        _extractLine(nrrdContentView);

        std::vector<std::string_view> result;
        while (true)
        {
            auto sv = _extractLine(nrrdContentView);

            // Header ends with blank line
            if (sv.empty())
            {
                break;
            }

            // Comments starts with #, with no preceeding whitespace. Ignore them
            if (sv[0] != '#')
            {
                result.push_back(sv);
            }

            // On detached-data nrrd files, header may end by eof
            if (nrrdContentView.empty())
            {
                break;
            }
        }

        return result;
    }

private:
    static std::string_view _extractLine(std::string_view &src)
    {
        size_t pivot = 0;

        size_t srcSubstrStart = 0;
        while (true)
        {
            auto currentChar = src[pivot];

            if (currentChar == '\n')
            {
                srcSubstrStart = pivot + 1;
                break;
            }

            if (currentChar == '\r' && src.length() > pivot + 1 && src[pivot + 1] == '\n')
            {
                srcSubstrStart = pivot + 2;
            }

            ++pivot;
        }

        auto result = src.substr(0, pivot);

        auto srcNewStart = std::min(src.length(), srcSubstrStart);
        src = src.substr(srcNewStart);

        return result;
    }
};

struct NRRDHeaderEntry
{
    std::string_view key;
    std::string_view value;
};

class HeaderEntrySplitter
{
public:
    static std::vector<NRRDHeaderEntry> split(const std::vector<std::string_view> &headerLines)
    {
        std::vector<NRRDHeaderEntry> result(headerLines.size());

        for (size_t i = 0; i < headerLines.size(); ++i)
        {
            auto line = headerLines[i];

            auto colonPosition = line.find(":");
            if (colonPosition == std::string_view::npos)
            {
                throw std::invalid_argument("Ill-formed NRRD header at " + std::string(line));
            }

            auto &entry = result[i];
            entry.key = line.substr(0, colonPosition);
            entry.value = line.substr(colonPosition + 1);
        }

        return result;
    }
};

class HeaderParseUtils
{
public:
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
        std::vector<std::vector<float>> result(tokens.size());
        for (size_t i = 0; i < tokens.size(); ++i)
        {
            result[i] = parseFloatVector(tokens[i]);
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

        auto data = value.substr(firstParenthesis, secondParenthesis - firstParenthesis);
        const auto tokens = parseStringArray(data, ',');
        return parseStringListToFloat(tokens);
    }
};

class HeaderEntryParser
{
public:
    static NRRDHeader parse(const std::vector<NRRDHeaderEntry> &entries)
    {
        NRRDHeader header;

        for (const auto &entry : entries)
        {
            _parseEntry(entry, header);
        }

        return header;
    }

private:
    static void _parseEntry(const NRRDHeaderEntry &entry, NRRDHeader &dstHeader)
    {
        std::unordered_map<std::string, std::function<void(std::string_view, NRRDHeader &)>> entryNameMap;

        auto dimensionCallback = [](std::string_view value, NRRDHeader &header)
        { header.dimensions = std::stoi(std::string(value)); };

        auto typeCallback = [](std::string_view value, NRRDHeader &header)
        {
            auto valueStr = std::string(value);
            brayns::string_utils::trim(valueStr);
        };

        auto spaceDimensionCallback = [](std::string_view value, NRRDHeader &header)
        { header.spaceDimensions = std::stoi(std::string(value)); };

        auto spaceUnitsCallback = [](std::string_view value, NRRDHeader &header)
        {
            if (!header.spaceDimensions)
            {
                throw std::invalid_argument("Ill-formed NRRD header: space dimensions not setted before space units");
            }

            auto units = HeaderParseUtils::parseStringArray(value);
            if (units.size() != static_cast<size_t>(*header.spaceDimensions))
            {
                throw std::invalid_argument("Ill-formed NRRD header: space dimensions and space units size missmatch");
            }

            header.spaceUnits = units;
        };

        auto spaceOriginCallback = [](std::string_view value, NRRDHeader &header)
        {
            if (!header.spaceDimensions)
            {
                throw std::invalid_argument("Ill-formed NRRD header: space dimensions not setted before space origin");
            }

            auto origin = HeaderParseUtils::parseFloatVector(value);
            if (origin.size() != static_cast<size_t>(*header.spaceDimensions))
            {
                throw std::invalid_argument("Ill-formed NRRD header: space dimensions and space origin size missmatch");
            }

            header.spaceOrigin = origin;
        };

        auto spaceDirectionsCallback = [](std::string_view value, NRRDHeader &header)
        {
            if (!header.spaceDimensions)
            {
                throw std::invalid_argument(
                    "Ill-formed NRRD header: space dimensions not setted before space directions");
            }

            auto dimensions = static_cast<size_t>(*header.spaceDimensions);
            auto directions = HeaderParseUtils::parseFloatFrame(value);
            if (directions.size() != dimensions)
            {
                throw std::invalid_argument(
                    "Ill-formed NRRD header: space dimensions and space directions size missmatch");
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
        };

        auto measurementFrameCallback = [](std::string_view value, NRRDHeader &header)
        {
            if (!header.spaceDimensions)
            {
                throw std::invalid_argument(
                    "Ill-formed NRRD header: space dimensions not setted before measurement frame");
            }

            auto dimensions = static_cast<size_t>(*header.spaceDimensions);
            auto measurementFrame = HeaderParseUtils::parseFloatFrame(value);
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
        };
    }
};
}

NRRDHeader HeaderParser::parse(std::string_view &nrrdContentView)
{
    const auto headerLines = HeaderReader::readLines(nrrdContentView);
    const auto headerEntries = HeaderEntrySplitter::split(headerLines);
    return HeaderEntryParser::parse(headerEntries);
}
