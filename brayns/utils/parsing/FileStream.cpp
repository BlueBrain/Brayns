/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "FileStream.h"

#include <brayns/utils/string/StringExtractor.h>

#include "ParsingException.h"

namespace brayns
{
FileStream::FileStream(std::string_view data):
    _data(data)
{
}

std::string_view FileStream::getData() const
{
    return _data;
}

size_t FileStream::getLineNumber() const
{
    return _lineNumber;
}

std::string_view FileStream::getLine() const
{
    return _line;
}

ParsingException FileStream::error(const std::string &message) const
{
    return ParsingException(message, _lineNumber, std::string(_line));
}

bool FileStream::nextLine()
{
    if (_end)
    {
        return false;
    }
    _line = StringExtractor::extractLine(_data);
    if (_data.empty())
    {
        _end = true;
    }
    StringExtractor::extract(_data, 1);
    ++_lineNumber;
    return true;
}
} // namespace brayns
