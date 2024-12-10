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

#include "Filesystem.h"

#include <fstream>
#include <stdexcept>

#include <fmt/format.h>
#include <fmt/std.h>

namespace brayns
{
std::string readFile(const std::filesystem::path &path)
{
    if (!std::filesystem::exists(path))
    {
        throw std::invalid_argument(fmt::format("File '{}' does not exist", path));
    }

    if (!std::filesystem::is_regular_file(path))
    {
        throw std::invalid_argument(fmt::format("File '{}' is not a regular file", path));
    }

    auto stream = std::ifstream(path, std::ios::ate);

    if (!stream.is_open())
    {
        throw std::runtime_error(fmt::format("Failed to open file '{}' in read mode", path));
    }

    auto size = stream.tellg();
    auto data = std::string(static_cast<std::size_t>(size), '\0');

    stream.seekg(0);
    stream.read(data.data(), size);

    return data;
}

void writeFile(std::span<const char> data, const std::filesystem::path &path)
{
    auto stream = std::ofstream(path);

    if (!stream.is_open())
    {
        throw std::runtime_error(fmt::format("Failed to open file '{}' in write mode", path));
    }

    const auto *ptr = data.data();
    auto size = static_cast<std::streamsize>(data.size());

    stream.write(ptr, size);
}
}
