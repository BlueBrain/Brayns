/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include "LoaderFormat.h"

#include <brayns/utils/string/StringCase.h>
#include <brayns/utils/string/StringInfo.h>

namespace brayns
{
std::string LoaderFormat::fromPath(const std::filesystem::path &path)
{
    auto extension = path.extension().string();
    if (extension.empty())
    {
        return path.filename();
    }
    return fromExtension(extension);
}

std::string LoaderFormat::fromExtension(std::string_view extension)
{
    if (StringInfo::startsWith(extension, '.'))
    {
        extension = extension.substr(1);
    }
    return StringCase::toLower(extension);
}
} // namespace brayns
