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

#include "ParsingException.h"

#include <sstream>

namespace brayns
{
ParsingException::ParsingException(const std::string &message, size_t lineNumber, std::string line):
    std::runtime_error(message),
    _lineNumber(lineNumber),
    _line(std::move(line))
{
}

size_t ParsingException::getLineNumber() const
{
    return _lineNumber;
}

const std::string &ParsingException::getLine() const
{
    return _line;
}

std::string ParsingException::format() const
{
    std::ostringstream stream;
    stream << "Parsing error at line " << _lineNumber;
    stream << ": '" << what() << "'";
    stream << ". Line content: '" << _line << "'";
    return stream.str();
}
} // namespace brayns
