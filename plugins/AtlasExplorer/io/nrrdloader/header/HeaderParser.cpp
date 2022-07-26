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

#include <io/nrrdloader/header/HeaderEntryParser.h>

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
    const auto headerLines = HeaderReader::readLines(nrrdContentView);
    const auto headerEntryList = HeaderEntrySplitter::split(headerLines);
    auto header = EntryListParser::parse(headerEntryList);
    header.filePath = std::move(nrrdPath);
    return header;
}
