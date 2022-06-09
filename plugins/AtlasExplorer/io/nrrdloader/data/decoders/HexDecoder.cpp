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

#include "HexDecoder.h"

#include <brayns/utils/StringUtils.h>

#include <io/nrrdloader/data/decoders/RawDecoder.h>

namespace
{
class HexContentParser
{
public:
    static std::string toBinary(std::string_view input)
    {
        auto tokens = brayns::string_utils::split(input, " /r/v/f/t/n");

        size_t expectedResultSize = 0;
        for (auto token : tokens)
        {
            expectedResultSize += token.size();
        }

        std::string result;
        result.reserve(expectedResultSize);

        std::string hexBuffer(3, '\0');
        size_t hexIndex = 0;

        for (auto token : tokens)
        {
            for (size_t i = 0; i < token.size(); ++i)
            {
                hexBuffer[hexIndex++] = token[i];

                if (hexIndex == 2)
                {
                    hexIndex = 0;
                    auto byte = std::stoi(hexBuffer, nullptr, 16);
                    result.push_back(static_cast<char>(byte));
                }
            }
        }

        return result;
    }
};
}

std::unique_ptr<IDataMangler> HexDecoder::decode(const NRRDHeader &header, std::string_view input) const
{
    auto binaryData = HexContentParser::toBinary(input);

    auto binaryParser = RawDecoder();
    return binaryParser.decode(header, binaryData);
}
