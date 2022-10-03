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

#include "Tokenizer.h"

#include <brayns/utils/string/StringCounter.h>
#include <brayns/utils/string/StringExtractor.h>

namespace
{
inline static const std::string_view delimiters = " /r/v/f/t/n";
}

std::vector<std::string_view> Tokenizer::fromView(std::string_view input)
{
    auto tokens = std::vector<std::string_view>();

    auto tokenCount = brayns::StringCounter::countOneOf(input, delimiters);
    tokens.reserve(tokenCount);

    while (!input.empty())
    {
        auto token = brayns::StringExtractor::extractUntilOneOf(input, delimiters);
        tokens.push_back(token);
        brayns::StringExtractor::extract(input, 1);
    }
    return tokens;
}
