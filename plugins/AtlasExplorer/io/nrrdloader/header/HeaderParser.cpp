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

#include "HeaderParser.h"

#include <brayns/utils/string/StringCounter.h>
#include <brayns/utils/string/StringExtractor.h>
#include <brayns/utils/string/StringInfo.h>

#include <io/nrrdloader/header/HeaderEntryParser.h>

#include <filesystem>
#include <functional>
#include <unordered_map>
#include <vector>

namespace
{
class LineBreakFinder
{
public:
    static std::string fromView(std::string_view view)
    {
        auto windowEndingIndex = view.find("\r\n");
        auto unixEndingIndex = view.find("\n");
        if (windowEndingIndex < unixEndingIndex)
        {
            return "\r\n";
        }
        return "\n";
    }
};

class HeaderReader
{
public:
    static std::vector<std::string_view> readLines(std::string_view &data)
    {
        auto lineBreak = LineBreakFinder::fromView(data);
        auto result = std::vector<std::string_view>();
        result.reserve(10);

        // Extracts magic, which we do not use
        _extractLine(data, lineBreak);
        while (!data.empty())
        {
            auto sv = _extractLine(data, lineBreak);

            // Header ends with blank line
            if (sv.empty())
            {
                break;
            }
            // Comments starts with #, with no preceeding whitespace. Ignore them
            if (sv[0] == '#')
            {
                continue;
            }

            result.push_back(sv);
        }

        return result;
    }

private:
    static std::string_view _extractLine(std::string_view &data, std::string_view lineBreak)
    {
        auto line = brayns::StringExtractor::extractUntil(data, lineBreak);
        brayns::StringExtractor::extract(data, lineBreak.length());
        return line;
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
        std::vector<NRRDHeaderEntry> result;
        result.reserve(headerLines.size());

        for (auto line : headerLines)
        {
            if (!brayns::StringInfo::contains(line, ":"))
            {
                throw std::invalid_argument("Ill-formed NRRD header at " + std::string(line));
            }

            auto &entry = result.emplace_back();
            entry.key = brayns::StringExtractor::extractUntil(line, ":");
            brayns::StringExtractor::extract(line, 1);
            entry.value = line;
        }

        return result;
    }
};

class EntryListParser
{
public:
    static NRRDHeader parse(const std::vector<NRRDHeaderEntry> &entries)
    {
        NRRDHeader header;
        HeaderEntryParser parser;

        for (const auto &entry : entries)
        {
            parser.parseEntry(entry.key, entry.value, header);
        }

        return header;
    }
};
}

NRRDHeader HeaderParser::parse(std::string nrrdPath, std::string_view &nrrdContentView)
{
    auto headerLines = HeaderReader::readLines(nrrdContentView);
    auto headerEntryList = HeaderEntrySplitter::split(headerLines);
    auto header = EntryListParser::parse(headerEntryList);
    header.filePath = std::move(nrrdPath);
    return header;
}
